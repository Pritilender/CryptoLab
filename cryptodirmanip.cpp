#include "a51.h"
#include "cryptodirmanip.h"
#include <simplesubstitutioner.h>
#include <QDirIterator>
#include <QTextStream>

#include <QDebug>

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
//    this->queueThread = QtConcurrent::run(this, &CryptoDirManip::inotifyThread);
//    this->conc = QtConcurrent::run(this, &CryptoDirManip::queueManip);

//    for (int i = 0; i < 8; i++) {
//        this->mapThread.append(QFuture<void>());
//    }
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
       // this->setWatchMode(this->watchMode); ///TODO move this to a function
    }
    this->fileMutex.unlock();
}

CryptoDirManip::~CryptoDirManip()
{
    this->closeApp = true;

    this->fileMutex.lock();
    this->writeConfig();
    this->fileMutex.unlock();
}

void CryptoDirManip::loadInputDir(const QString &input)
{

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

void CryptoDirManip::fileToAlgo(const QString current)
{
    qDebug() << "Pocetak za " << current;

    QString base = current;
    QString outFilename = this->outputDir + "/";
    if (this->encryption) {
        outFilename += base + ".crypto";
    } else {
        outFilename += base.remove(".crypto");
    }

    QFile inFile(this->inputDir + "/" + current);
    QSaveFile outFile(outFilename);

    A51 algoRunner(this->key);

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
                emit this->changeSrc(QString(inputChar));
                QChar outputChar = algoRunner.stepByStep(inputChar).at(0);
                emit this->changeDst(QString(outputChar));
                fileOut << outputChar;
                this->nextStep = false;
            } else if (!this->simulation) {
                inputTxt = inReader.readLine();
                //maybe not add a new line?
                fileOut << algoRunner.runAlgo(inputTxt, this->encryption) << "\n";
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

    this->running = true;
    this->encryption = encryption;

}

void CryptoDirManip::queueManip()
{
//    while (!this->closeApp) {
//        if (this->running) {
//            this->mutex.lock();
//            if (this->simulation) {
//                while (this->runningThreads != 0) {};
//                this->runningThreads++;
////                this->fileToAlgo(this->fileNames.takeFirst());
//            }
//            if (this->fileNames.length() != 0 && this->runningThreads < 16) {
//                this->runningThreads++;

//                QString fName = this->fileNames.takeFirst();
//                QtConcurrent::run(this, &CryptoDirManip::fileToAlgo, fName);
//            }
//            if (!this->watchMode && this->fileNames.length() == 0) {
//                this->running = false;
//            }
//            this->mutex.unlock();
//        }
//    }
//    this->fileNames.clear();
}
