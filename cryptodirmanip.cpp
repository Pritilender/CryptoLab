#include "cryptodirmanip.h"
#include <simplesubstitutioner.h>
#include <QDirIterator>
#include <QTextStream>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

#include <QDebug>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUF_LEN (1024*(EVENT_SIZE + 16))

void CryptoDirManip::inotifyThread()
{
    while (!this->closeApp) {
        while (this->watchMode) {
            char buffer[BUF_LEN];
            int i = 0;
            int length = read(this->fd, buffer, BUF_LEN);

            if (length < 0) {
                qDebug() << "read";
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
    this->algoRunner = nullptr;

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

//    this->fileMutex.lock();
//    QFile configFile(this->configPath);
//    if (configFile.exists()) {
//        if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//            QTextStream cFile(&configFile);
//            QString key;
//            uint lastModified;

//            this->encryption = (bool)cFile.readLine().toInt();
//            this->running = (bool)cFile.readLine().toInt();
//            this->watchMode = (bool)cFile.readLine().toInt();

//            cFile >> this->inputDir;
//            cFile >> this->outputDir;
//            cFile >> key;
//            this->algoRunner = new SimpleSubstitutioner(key);
//            qDebug() << cFile.readLine();
//            lastModified = cFile.readLine().toUInt();
//            configFile.close();

//            QString inExt = this->encryption ? "*.txt" : "*.crypto";
//            QString outExt = this->encryption ? "*.crypto" : "*.txt";

//            this->mutex.lock();
//            QDirIterator outDir(this->outputDir, QStringList() << outExt, QDir::Files);
//            QList<QFileInfo> outDirFInfo;
//            QDirIterator inDir(this->inputDir, QStringList() << inExt, QDir::Files);
//            QList<QFileInfo> inDirFInfo;

//            while (outDir.hasNext()) {
//                outDirFInfo.append(QFileInfo(outDir.next()));
//            }

//            while (inDir.hasNext()) {
//                inDirFInfo.append(QFileInfo(inDir.next()));
//            }

//            this->fileNames = filterByDateModified(filterByBaseNames(inDirFInfo, outDirFInfo), lastModified);
//            this->mutex.unlock();
//        }
//    }
    //    this->fileMutex.unlock();
}

void CryptoDirManip::loadConfigFile()
{
    this->fileMutex.lock();
    this->readConfig();
    emit this->inDirFile(this->inputDir);
    emit this->outDirFile(this->outputDir);
    emit this->keyFile(this->algoRunner->returnKey());
    emit this->encryptionFile(this->encryption);
    emit this->runningFile(this->running);
    emit this->watchFile(this->watchMode);
    this->fileMutex.unlock();
}

CryptoDirManip::~CryptoDirManip()
{
    this->closeApp = true;
    for (int i = 0; i < this->mapThread.length(); i++) {
        this->mapThread[i].cancel();
    }
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
    this->algoRunner = new SimpleSubstitutioner(key);
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

QString inputDirPath;
QString outputDirPath;
bool isEncryptionRunning;
CryptoAlgorithm *ptAlgoRunner;

void CryptoDirManip::fileToAlgo(const QString current)
{
    qDebug() << "Pocetak za " << current;

    QString base = current;
    const QString outExtension = isEncryptionRunning ? ".crypto" : ".txt";
    const QString inExtension = isEncryptionRunning ? ".txt" : ".crypto";
    QFile inFile(inputDirPath + "/" + current);
    QSaveFile outFile(outputDirPath + "/" + base.remove(inExtension) + outExtension);


    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)
            && outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream inReader(&inFile);
        QTextStream fileOut(&outFile);
        QString inputTxt;

        while (!inReader.atEnd()) {
            inputTxt = inReader.readLine();
            //maybe not add a new line?
            fileOut << ptAlgoRunner->runAlgo(inputTxt, isEncryptionRunning) << "\n";
        }

        qDebug() << "kraj za " << current;

        inFile.close();
        outFile.commit();

        this->fileMutex.lock();
        this->writeConfig();
        this->fileMutex.unlock();
    }
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
        cfileOut << this->algoRunner->returnKey() << "\n";
        outFile.commit();
    }
}

void CryptoDirManip::readConfig()
{
    QFile inFile(this->configPath);
    QTextStream cfileIn(&inFile);
    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        int boolHelper;
        uint timeStamp;
        QString key;

        cfileIn >> boolHelper;
        this->encryption = boolHelper;
        qDebug() << boolHelper;

        cfileIn >> boolHelper;
        this->running = boolHelper;
        qDebug() << boolHelper;

        cfileIn >> boolHelper;
        this->watchMode = boolHelper;
        qDebug() << boolHelper;

        cfileIn >> this->inputDir;
        qDebug() << this->inputDir;
        cfileIn >> this->outputDir;
        qDebug() << this->outputDir;

        cfileIn >> timeStamp;
        qDebug() << timeStamp;

        cfileIn >> key;
        qDebug() << key;
        this->algoRunner = new SimpleSubstitutioner(key);
        inFile.close();
    }
}

void CryptoDirManip::run(const bool encryption)
{
    const QString inExtension = encryption ? ".txt" : ".crypto";

    QMutableStringListIterator it(this->fileNames);

    this->mutex.lock();
    qDebug() << "Neko zvao run?";
    this->running = true;
    this->encryption = encryption;
    while (it.hasNext()) {
        QString next = it.next();

        if (!next.endsWith(inExtension)) {
            it.remove();
        }
    }
    qDebug() << "Valjda ce radi";
    this->mutex.unlock();
}

void CryptoDirManip::queueManip()
{
    while (!this->closeApp) {
        if (this->running) {
            this->mutex.lock();
            if (this->fileNames.length() != 0) {
                qDebug() << "Enkriptuj!";
                inputDirPath = this->inputDir;
                outputDirPath = this->outputDir;
                isEncryptionRunning = this->encryption;
                ptAlgoRunner = this->algoRunner;
                foreach (QString fn, this->fileNames) {
                    qDebug() << fn;
                }

                for (int i = 0; i < this->fileNames.length() && i < 8; i++) {
                    if (!this->mapThread[i].isRunning()) {
                        QString fName = this->fileNames.takeFirst();
                        this->mapThread[i] = QtConcurrent::run(this, &CryptoDirManip::fileToAlgo, fName);
                    }
                }
                qDebug() << "Ce zavrsi!";
            }
            if (!this->watchMode && this->fileNames.length() == 0) {
                this->running = false;
            }
            this->mutex.unlock();
        }
    }
    this->fileNames.clear();
}
