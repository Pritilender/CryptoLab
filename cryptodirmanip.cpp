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

bool isTxtCrypt(char* fName){
    QString file(fName);

    return file.contains(QRegularExpression("([.]txt|[.]crypto)$"));
}

void CryptoDirManip::inotifyThread()
{
    while (true) {
        while (this->watchMode) {
            char buffer[BUF_LEN];
            int i = 0;
            int length = read(this->fd, buffer, BUF_LEN);

            if (length < 0) {
                qDebug() << "read";
                exit(1);
            }

            while (i < length) {
                struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
                if ( event->len && !(event->mask & IN_ISDIR) && isTxtCrypt(event->name)) {
                    if ( event->mask & IN_CREATE ) {
                        qDebug() << "The file" << event->name << "was created.";
                    } else if ( event->mask & IN_DELETE ) {
                        qDebug() << "The file" << event->name << "was deleted.";
                    } else if ( event->mask & IN_MODIFY ) {
                        qDebug() << "The file" << event->name << "was modified.";
                    }
                }
                i += EVENT_SIZE + event->len;

            }
        }
    }
}

void fileToAlgo(const CryptoFileInfo &current)
{
    qDebug() << "Pocetak za " << current.baseName;

    const QString outExtension = current.encryption ? ".crypto" : ".txt";
    QFile inFile(current.absoluteFilePath);
    QSaveFile outFile(current.outputDir + "/" + current.baseName + outExtension);


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
            fileOut << current.algoRunner->runAlgo(inputTxt, current.encryption) << "\n";
        }

        qDebug() << "kraj za " << current.baseName;

        inFile.close();
        outFile.commit();
    }
}


CryptoDirManip::CryptoDirManip()
    : QObject()
{
    this->algoRunner = nullptr;
    //QObject::connect(&this->fsWatcher, SIGNAL(directoryChanged(QString)), this,
    //                 SLOT(queueManip(QString)));
    this->fd = inotify_init();

    if (this->fd < 0) {
        qDebug() << "error for inotify";
        exit(0);
    }

    this->queueThread = QtConcurrent::run(this, &CryptoDirManip::inotifyThread);
}

CryptoDirManip::~CryptoDirManip()
{
    close(this->fd);
}

void CryptoDirManip::loadInputDir(const QString &input)
{
    QDirIterator inputDir(input, QStringList() << "*.txt" << "*.crypto", QDir::Files);
    this->inputDir = input;

    while (inputDir.hasNext()) {
        QFileInfo next(inputDir.next());
        CryptoFileInfo element = {
            next.baseName(),
            true,
            next.absoluteFilePath(),
            next.suffix(),
            "",
            nullptr,
            nullptr,
            next.lastModified(),
            next.created(),
            next.lastRead()
        };
        this->fileQueue.append(element);
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
        //this->fsWatcher.addPath(this->inputDir);
        this->wd = inotify_add_watch(this->fd, this->inputDir.toLatin1(),
                                     IN_MODIFY | IN_CREATE | IN_DELETE);
    } else {
        inotify_rm_watch(this->fd, this->wd);
    }
}

void CryptoDirManip::run(const bool encryption)
{
    this->running = true;
    this->encryption = false;
    //this->conc = QtConcurrent::run(this, &CryptoDirManip::printDir);
    const QString inExtension = encryption ? "txt" : "crypto";

    QMutableListIterator<CryptoFileInfo> it(this->fileQueue);

    // remove unneeded files from the Queue
    while (it.hasNext()) {
        CryptoFileInfo next = it.next();
        if (next.suffix != inExtension) {
            it.remove();
        }
        next.encryption = encryption;
        next.algoRunner = this->algoRunner;
        next.outputDir = this->outputDir;
        next.mutex = &this->mutex;
        it.setValue(next);
    }

    it.toFront();

    this->mutex.lock();
    this->conc = QtConcurrent::map(this->fileQueue, fileToAlgo);
    this->fileQueue.clear();
    this->mutex.unlock();

    qDebug() << "byebye";
}

int isModifiedFile(const QList<CryptoFileInfo> &fileQueue, const QString fileName,
                   const QDateTime lastModified)
{
    QListIterator<CryptoFileInfo> fileIt(fileQueue);
    int index = -1;

    while (index == -1 && fileIt.hasNext()) {
        CryptoFileInfo next = fileIt.next();
        if (next.baseName == fileName && next.lastModified < lastModified) {
            index = fileQueue.indexOf(next);
        }
    }

    return index;
}

int findByName(const QList<CryptoFileInfo> &fileQueue, const QString fileName)
{
    QListIterator<CryptoFileInfo> fileIt(fileQueue);
    int index = -1;

    while (index == -1 && fileIt.hasNext()) {
        CryptoFileInfo next = fileIt.next();
        if (next.baseName == fileName) {
            index = fileQueue.indexOf(next);
        }
    }

    return index;
}

void CryptoDirManip::queueManip(const QString &path)
{

}
