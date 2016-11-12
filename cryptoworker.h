#ifndef CRYPTOWORKER_H
#define CRYPTOWORKER_H

#include <QObject>
#include <QThread>
#include <cryptoalgorithm.h>
#include <knapsack.h>

class CryptoWorker : public QThread
{
    Q_OBJECT

private:
    CryptoAlgorithm *algorithm;
    QString inFile;
    QString outFile;
    bool encryption = false;
    uint32_t *publicKey;
    uint32_t *privateKey;
    uint32_t im;
    uint32_t n;

    QString getOutFileName(const QString &outPath);

public:
    CryptoWorker(const bool encryption, const QString &inPath,
                 const QString &outPath, uint32_t publicKey[], uint32_t im, uint32_t privateKey[], uint32_t n,
                 QObject *parent = 0);
    void run() Q_DECL_OVERRIDE;

signals:
    void algorithmEnd();
    void inBMPReady(QString iB);
    void outBMPReady(QString oB);
};

#endif // CRYPTOWORKER_H
