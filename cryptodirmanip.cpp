#include "cryptodirmanip.h"
#include <simplesubstitutioner.h>
#include <QDirIterator>
#include <QTextStream>

#include <QDebug>

CryptoDirManip::CryptoDirManip()
    : QObject()
{
    this->algoRunner = nullptr;
    QObject::connect(&this->fsWatcher, SIGNAL(directoryChanged(QString)), this,
                     SLOT(queueManip(QString)));
}

void CryptoDirManip::loadInputDir(const QString &input)
{
    this->inputDir = input;

    this->fsWatcher.addPath(this->inputDir);
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
}

void CryptoDirManip::run(const bool encryption)
{
    //this->conc = QtConcurrent::run(this, &CryptoDirManip::printDir);
    const QString inExtension = encryption ? "*.txt" : "*.crypto";
    const QString outExtension = encryption ? ".crypto" : ".txt";
    //if (autowatch) {
    //autowatcher
    //} else {
    //just read dirs and encrypt files
    QDirIterator inputDir(this->inputDir, QStringList() << inExtension, QDir::Files);


    qDebug() << inputDir.path() << " i out: " << outputDir;

    while (inputDir.hasNext()) {
        QFile inFile(inputDir.next());
        QFile outFile(outputDir + "/" + inputDir.fileInfo().baseName() + outExtension);
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
                fileOut << this->algoRunner->runAlgo(inputTxt, encryption) << "\n";
            }

            qDebug() << "kraj za " << inputDir.fileInfo().baseName();
        }
    }
    this->fileQueue.enqueue(*(new QFileInfo()));
    //}
}

void CryptoDirManip::queueManip(const QString &path)
{
    this->fileQueue.enqueue(*(new QFileInfo()));
    qDebug() << "bla" << path;
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
