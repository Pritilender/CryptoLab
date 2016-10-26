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

bool isTxtCrypt(char *fName)
{
    QString file(fName);

    return file.contains(QRegularExpression("([.]txt|[.]crypto)$"));
}

void CryptoDirManip::inotifyThread()
{
    while (!this->closeApp) {
        while (this->watchMode) {
            char buffer[BUF_LEN];
            int i = 0;
            int length = read(this->fd, buffer, BUF_LEN);

            if (length < 0) {
                qDebug() << "read";
                exit(1);
            }

            this->mutex.lock();
            while (i < length) {
                struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
                if ( event->len && !(event->mask & IN_ISDIR) && isTxtCrypt(event->name)) {
                    if ( event->mask & IN_CREATE ) {
                        // New file, just add it to the queue
                        qDebug() << "The file" << event->name << "was created.";
                        this->fileNames.append(QString(event->name));
                    } else if ( event->mask & IN_MODIFY && this->running ) {
                        // File is modified and algo is running, so we need to readd it
                        qDebug() << "The file" << event->name << "was modified.";
                        this->fileNames.append(QString(event->name));
                    } else if ( event->mask & IN_DELETE && !this->running) {
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
}

CryptoDirManip::~CryptoDirManip()
{
    this->closeApp = true;
    //this->conc.cancel();
    //this->queueThread.cancel();
    for (int i = 0; i < this->mapThread.length(); i++){
        this->mapThread[i].cancel();
    }
    //this->conc.waitForFinished();
    //this->queueThread.waitForFinished();
    close(this->fd);
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

            inputTxt.remove(QRegExp("[^A-Za-z]"));

            inputTxt = inputTxt.toLower();

            //maybe not add a new line?
            fileOut << ptAlgoRunner->runAlgo(inputTxt, isEncryptionRunning) << "\n";
        }

        qDebug() << "kraj za " << current;

        inFile.close();
        outFile.commit();

        this->mutex.lock();
        QFile outFile(this->configPath);
        QTextStream cfileOut(&outFile);
        if (outFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
            cfileOut << QDateTime().currentDateTime().toTime_t();
            outFile.close();
        }
        this->mutex.unlock();
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
