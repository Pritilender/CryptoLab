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
    uint32_t* iv;
    QMutex keyMutex;
    QMutex ivMutex;
    int getBytesAdded(QByteArray &inputBuffer);
    void fillBuffer(QSaveFile &outFile, QByteArray &inputBuffer);
    void removeAddedBytes(int bytesToRemove, QByteArray &v0, QByteArray &v1);
    void encrypt(uint32_t *v, bool TEA, bool e);
    uint32_t uintFromByteArray(QByteArray& arr);
    QByteArray byteArrayFromUint(uint32_t num);

public:
    TEA();
    TEA(TEA *alg);
    ~TEA();
    void runAlgo(const QString& inFile, const QString& outFile, bool encrypt, bool xMode);
    void encryptBMP(const QString& inFile, const QString& outFile, bool encrypt, bool xMode);
    QString returnKey();
    void setKey(QString key);
    void setIV(QString IV);
};

#endif // TEA_H
