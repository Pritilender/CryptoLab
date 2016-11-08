#include "cryptoworker.h"
#include "tea.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>

#include <QDebug>

QString CryptoWorker::getOutFileName(const QString &outPath)
{
    QString base = QFileInfo(this->inFile).fileName();

    if (!base.endsWith(".bmp")) {
        if (this->encryption) {
            base += ".crypto";
        } else {
            base = base.remove(".crypto");
        }
    }
    return outPath + "/" + base;
}

CryptoWorker::CryptoWorker(const bool encryption, CryptoAlgorithm *alg, const QString &inPath,
                           const QString &outPath, const bool xMode, QObject *parent)
{
    Q_UNUSED(parent);
    this->encryption = encryption;
    this->xMode = xMode;
    this->algorithm = new TEA((TEA*)alg);
    this->inFile = inPath;
    this->outFile = getOutFileName(outPath);
}

void CryptoWorker::run()
{
    if (this->outFile.endsWith(".bmp")) {
        ((TEA *)this->algorithm)->encryptBMP(this->inFile, this->outFile, this->encryption, this->xMode);
    } else {
        this->algorithm->runAlgo(this->inFile, this->outFile, this->encryption, this->xMode);
    }
    emit this->algorithmEnd();
}
