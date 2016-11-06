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

    QString getOutFileName(const QString &outPath);

public:
    CryptoWorker(const bool encryption, CryptoAlgorithm *alg, const QString &inPath,
                 const QString &outPath, QObject *parent = 0);
    void run() Q_DECL_OVERRIDE;

signals:
    void algorithmEnd();
};

#endif // CRYPTOWORKER_H
