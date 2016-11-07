#ifndef TEA_H
#define TEA_H

#include "cryptoalgorithm.h"

#include <QMutex>
#include <QSaveFile>



class TEA : public CryptoAlgorithm
{
private:
    void TEAFeistelRound(uint32_t* v, bool isEncryption);
    void XTEAFeistelRound(uint32_t* v, bool isEncryption);
    uint32_t* key;
    QMutex keyMutex;
    int getBytesAdded(QByteArray &inputBuffer);
    void fillBuffer(QSaveFile &outFile, QByteArray &inputBuffer);
    void removeAddedBytes(int bytesToRemove, QByteArray &v0, QByteArray &v1);
    void encrypt(QByteArray& v0, QByteArray& v1, bool TEA, bool e);

public:
    TEA();
    ~TEA();
    void runAlgo(const QString& inFile, const QString& outFile, bool encrypt, bool xMode);
    void encryptBMP(const QString& inFile, const QString& outFile, bool encrypt, bool xMode);
    QString returnKey();
    void setKey(QString key);
};

#endif // TEA_H
