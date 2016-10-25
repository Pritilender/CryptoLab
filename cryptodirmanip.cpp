#include "cryptodirmanip.h"
#include <simplesubstitutioner.h>
#include <QDirIterator>
#include <QTextStream>
#include <functional>

#include <QDebug>

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
    QObject::connect(&this->fsWatcher, SIGNAL(directoryChanged(QString)), this,
                     SLOT(queueManip(QString)));
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
        this->fsWatcher.addPath(this->inputDir);
    } else {
        this->fsWatcher.removePath(this->inputDir);
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
    QDirIterator dirIt(path);

    mutex.lock();
    while (dirIt.hasNext()) {
        QFileInfo next = QFileInfo(dirIt.next());
        CryptoFileInfo element = {
            next.baseName(),
            this->encryption,
            next.absoluteFilePath(),
            next.suffix(),
            this->outputDir,
            this->algoRunner,
            &this->mutex,
            next.lastModified(),
            next.created(),
            next.lastRead()
        };

        int fileIndex = findByName(this->fileQueue, element.baseName);
        if (fileIndex == -1) {
            qDebug() << "New file";
            // new file
            this->fileQueue.append(element);
        } else if (isModifiedFile(this->fileQueue, element.baseName, element.lastModified) != -1) {
            // was already in queue and is just modified
            qDebug() << "Modified";
            continue;
        } else {
            qDebug() << "Deleted";
            // remove it
            this->fileQueue.removeAt(fileIndex);
        }
    }
    mutex.unlock();

}

void CryptoDirManip::printDir()
{
    while (true) {
        if (this->oldLength != this->fileQueue.length()) {
            this->oldLength = this->fileQueue.length();
            qDebug() << this->fileQueue.length();
        }
    }
}
