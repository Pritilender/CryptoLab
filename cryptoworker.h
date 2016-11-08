#ifndef CRYPTOWORKER_H
#define CRYPTOWORKER_H

#include <QObject>
#include <QThread>
#include <cryptoalgorithm.h>

class CryptoWorker : public QThread
{
    Q_OBJECT

private:
    CryptoAlgorithm *algorithm;
    QString inFile;
    QString outFile;
    bool encryption = false;
    bool xMode = false;
    bool bmpMode = false;

    QString getOutFileName(const QString &outPath);

public:
    CryptoWorker(const bool encryption, CryptoAlgorithm *alg, const QString &inPath,
                 const QString &outPath, const bool xMode, const bool bmpMode, QObject *parent = 0);
    void run() Q_DECL_OVERRIDE;

signals:
    void algorithmEnd();
    void inBMPReady(QString iB);
    void outBMPReady(QString oB);
};

#endif // CRYPTOWORKER_H
