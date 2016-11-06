#include "cryptoworker.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>

QString CryptoWorker::getOutFileName(const QString &outPath)
{
    QString base = QFileInfo(this->inFile).baseName();

    if (this->encryption) {
        base += ".crypto";
    } else {
        base = base.remove(".crypto");
    }

    return outPath + base;
}

CryptoWorker::CryptoWorker(const bool encryption, CryptoAlgorithm *alg, const QString &inPath,
                           const QString &outPath, QObject *parent)
{
    this->encryption = encryption;
    this->algorithm = alg;
    this->inFile = inPath;
    this->outFile = getOutFileName(outPath);
}

void CryptoWorker::run()
{
    this->algorithm->runAlgo(this->inFile, this->outFile, this->encryption);
    emit this->algorithmEnd();
}
