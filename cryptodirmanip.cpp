#include "a51.h"
#include "cryptodirmanip.h"
#include <simplesubstitutioner.h>
#include <QDirIterator>
#include <QTextStream>
#include <sys/inotify.h>
#include <sys/types.h>
#include <poll.h>
#include <unistd.h>

#include <QDebug>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUF_LEN (1024*(EVENT_SIZE + 16))

void CryptoDirManip::inotifyThread()
{
    struct pollfd pfd = {this->fd, POLLIN, 0};

    while (!this->closeApp) {
        while (this->watchMode) {
            char buffer[BUF_LEN];
            int i = 0;
            int ret = poll(&pfd, 1, 500);

            if (ret < 0) {
                qDebug() << "Poll fail";
                return;
            } else if (ret == 0) {
                continue; // no new events
            } else {
                int length = read(this->fd, buffer, BUF_LEN);

                if (length < 0) {
                    qDebug() << "Read fail";
                    return;
                }

                this->mutex.lock();
                while (i < length) {
                    struct inotify_event *event = (struct inotify_event *) &buffer[i];
                    if (event->len && !(event->mask & IN_ISDIR)) {
                        if (event->mask & IN_CREATE) {
                            // New file, just add it to the queue
                            qDebug() << "The file" << event->name << "was created.";
                            this->fileNames.append(QString(event->name));
                        } else if (event->mask & IN_MODIFY) {
                            // File is modified and algo is running, so we need to readd it
                            qDebug() << "The file" << event->name << "was modified.";
                            if (!this->fileNames.contains(QString(event->name))) {
                                this->fileNames.append(QString(event->name));
                            }
                        } else if (event->mask & IN_DELETE && !this->running) {
                            // If algo is not running and we need to remove the file from queue
                            qDebug() << "The file" << event->name << "was deleted.";
                            this->fileNames.removeOne(QString(event->name));
                        }
                    }
                    i += EVENT_SIZE + event->len;

                }
                this->mutex.unlock();
            }
        }
    }
}

QList<QFileInfo> filterByBaseNames(QList<QFileInfo> inDir, QList<QFileInfo>outDir)
{
    //SWAP THIS AND NEXT ONE!
    QList<QFileInfo> filtered;
    QListIterator<QFileInfo> inIt(inDir);

    while (inIt.hasNext()) {
        QFileInfo input(inIt.next());

        bool found = false;
        for (int i = 0; !found && i < outDir.length(); i++) {
            if (outDir[i].baseName() == input.baseName()) {
                filtered.append(input);
                found = true;
            }
        }
    }

    return filtered;
}

QList<QString> filterByDateModified(QList<QFileInfo> inDir, uint lastModified)
{
    QList<QString> filtered;
    QListIterator<QFileInfo> inIt(inDir);

    while (inIt.hasNext()) {
        QFileInfo next(inIt.next());

        if (lastModified < next.lastModified().toTime_t()) {
            filtered.append(next.fileName());
        }
    }

    return filtered;
}

CryptoDirManip::CryptoDirManip()
    : QObject()
{
    this->fd = inotify_init();

    if (this->fd < 0) {
        qDebug() << "error for inotify";
        exit(0);
    }

    this->queueThread = QtConcurrent::run(this, &CryptoDirManip::inotifyThread);
    this->conc = QtConcurrent::run(this, &CryptoDirManip::queueManip);

    for (int i = 0; i < 8; i++) {
        this->mapThread.append(QFuture<void>());
    }
}

void CryptoDirManip::loadConfigFile()
{
    this->fileMutex.lock();
    QFile cFile(this->configPath);
    if (cFile.exists()) {
        this->readConfig();
        emit this->inDirFile(this->inputDir);
        emit this->outDirFile(this->outputDir);
        emit this->keyFile(this->key);
        emit this->encryptionFile(this->encryption);
        emit this->runningFile(this->running);
        emit this->watchFile(this->watchMode);
        this->setWatchMode(this->watchMode); ///TODO move this to a function
    }
    this->fileMutex.unlock();
}

CryptoDirManip::~CryptoDirManip()
{
    this->closeApp = true;

    close(this->fd);

    this->fileMutex.lock();
    this->writeConfig();
    this->fileMutex.unlock();
}

void CryptoDirManip::loadInputDir(const QString &input)
{
    QDirIterator inputDir(input, QStringList() << "*.txt" << "*.crypto", QDir::Files);
    this->inputDir = input;

    while (inputDir.hasNext()) {
        QFileInfo next(inputDir.next());
        this->fileNames.append(next.fileName());
    }
}

void CryptoDirManip::loadOutputDir(const QString &output)
{
    this->outputDir = output;
}

void CryptoDirManip::loadKey(const QString &key)
{
    //this->algoRunner = new SimpleSubstitutioner(key);
    QString x, y, z;
    for (int i = 0; i < 64; i++) {
        if (i < 19) {
            x.append(key.at(i));
        } else if (i < 19 + 22) {
            y.append(key.at(i));
        } else {
            z.append(key.at(i));
        }
    }
    emit this->changeRegX(x);
    emit this->changeRegY(y);
    emit this->changeRegZ(z);

    this->key = key;
}

void CryptoDirManip::setWatchMode(const bool mode)
{
    this->watchMode = mode;

    if (this->watchMode) {
        this->wd = inotify_add_watch(this->fd, this->inputDir.toLatin1(),
                                     IN_MODIFY | IN_CREATE | IN_DELETE);
    } else {
        inotify_rm_watch(this->fd, this->wd);
    }
}

bool isEncryptionRunning;

void CryptoDirManip::fileToAlgo(const QString current)
{
    qDebug() << "Pocetak za " << current;

    QString base = current;
    const QString outExtension = isEncryptionRunning ? ".crypto" : ".txt";
    const QString inExtension = isEncryptionRunning ? ".txt" : ".crypto";
    QFile inFile(this->inputDir + "/" + current);
    QSaveFile outFile(this->outputDir + "/" + base.remove(inExtension) + outExtension);

    A51 algoRunner(this->key);

    // Ovo pomeriti u drugu funkciju
    // jedna funkcija koja se poziva kada se izvrsava step by step i druga funkcija koja se poziva kada ide sve odjednom
    //
    if (this->simulation) {
        QObject::connect(&algoRunner, SIGNAL(xStepped(QString)), this, SLOT(passXStepped(QString)));
        QObject::connect(&algoRunner, SIGNAL(yStepped(QString)), this, SLOT(passYStepped(QString)));
        QObject::connect(&algoRunner, SIGNAL(zStepped(QString)), this, SLOT(passZStepped(QString)));
        this->loadKey(this->key);
    }


    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)
            && outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream inReader(&inFile);
        QTextStream fileOut(&outFile);
        QString inputTxt;

        while (!inReader.atEnd()) {
            if (this->simulation && this->nextStep) {
                QChar inputChar = inReader.read(1).at(0);
                emit this->changeSrc(' ' + inputChar);
                QChar outputChar = algoRunner.stepByStep(inputChar).at(0);
                emit this->changeDst(' ' + outputChar);
                fileOut << outputChar;
                this->nextStep = false;
            } else if (!this->simulation) {
                inputTxt = inReader.readLine();
                //maybe not add a new line?
                fileOut << algoRunner.runAlgo(inputTxt, isEncryptionRunning) << "\n";
            }
        }

        qDebug() << "kraj za " << current;

        inFile.close();
        outFile.commit();

        this->fileMutex.lock();
        this->writeConfig();
        this->fileMutex.unlock();
        this->runningThreads--;
    }
}

void CryptoDirManip::fTAStepByStep(A51 &alg, const QString src)
{

}

void CryptoDirManip::writeConfig()
{
    QSaveFile outFile(this->configPath);
    QTextStream cfileOut(&outFile);
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cfileOut << this->encryption << "\n";
        cfileOut << this->running << "\n";
        cfileOut << this->watchMode << "\n";
        cfileOut << this->inputDir << "\n";
        cfileOut << this->outputDir << "\n";
        cfileOut << QDateTime().currentDateTime().toTime_t() << "\n";
        cfileOut << this->key << "\n";
        outFile.commit();
    }
}

void CryptoDirManip::readConfig()
{
    QFile inFile(this->configPath);
    QTextStream cfileIn(&inFile);
    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        int boolHelper;
        uint timeStamp;;

        cfileIn >> boolHelper;
        this->encryption = boolHelper;

        cfileIn >> boolHelper;
        this->running = boolHelper;

        cfileIn >> boolHelper;
        this->watchMode = boolHelper;

        cfileIn >> this->inputDir;
        cfileIn >> this->outputDir;

        cfileIn >> timeStamp;

        cfileIn >> this->key;
        //this->algoRunner = new SimpleSubstitutioner(key);
        inFile.close();
    }
}

void CryptoDirManip::run(const bool encryption)
{
    const QString inExtension = encryption ? ".txt" : ".crypto";

    QMutableStringListIterator it(this->fileNames);

    this->mutex.lock();

    this->running = true;
    this->encryption = encryption;

    while (it.hasNext()) {
        QString next = it.next();

        if (!next.endsWith(inExtension)) {
            it.remove();
        }
    }

    this->mutex.unlock();
}

void CryptoDirManip::queueManip()
{
    while (!this->closeApp) {
        if (this->running) {
            this->mutex.lock();
            if (this->simulation) {
                while (this->runningThreads != 0) {};
                this->runningThreads++;
                this->fileToAlgo(this->fileNames.takeFirst());
            }
            if (this->fileNames.length() != 0 && this->runningThreads < 16) {
                this->runningThreads++;
                isEncryptionRunning = this->encryption;

                QString fName = this->fileNames.takeFirst();
                QtConcurrent::run(this, &CryptoDirManip::fileToAlgo, fName);
            }
            if (!this->watchMode && this->fileNames.length() == 0) {
                this->running = false;
            }
            this->mutex.unlock();
        }
    }
    this->fileNames.clear();
}
