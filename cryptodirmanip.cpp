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
    qDebug() << input;
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
    const QString inExtension = encryption ? "*.txt" : "*.crypto";
    const QString outExtension = encryption ? "*.crypto" : "*.txt";
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
                inputTxt += inReader.readLine();
            }

            inputTxt.remove(QChar(' '), Qt::CaseInsensitive);
            inputTxt.remove(QChar('\n'), Qt::CaseInsensitive);

            qDebug() << inputTxt;

            inputTxt = inputTxt.toLower();

            fileOut << this->algoRunner->runAlgo(inputTxt, false);

            qDebug() << "kraj za " << inputDir.fileInfo().baseName();
        }
    }
    //}
}

void CryptoDirManip::queueManip(const QString &path)
{
    qDebug() << "bla" << path;
}
