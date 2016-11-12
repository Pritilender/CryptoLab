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

CryptoWorker::CryptoWorker(const bool encryption, const QString &inPath,
                           const QString &outPath, uint32_t publicKey[8], uint32_t im, uint32_t privateKey[8], uint32_t n,
                           QObject *parent)
{
    Q_UNUSED(parent);
    this->encryption = encryption;
    this->algorithm = new Knapsack(privateKey, publicKey, im, n);
    this->inFile = inPath;
    this->outFile = getOutFileName(outPath);
}

void CryptoWorker::run()
{
    this->algorithm->runAlgo(this->inFile, this->outFile, this->encryption);
    emit this->algorithmEnd();
}
