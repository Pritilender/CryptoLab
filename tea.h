#ifndef TEA_H
#define TEA_H

#include "cryptoalgorithm.h"

#include <QMutex>



class TEA : public CryptoAlgorithm
{
private:
    void feistelRound(uint32_t* v, bool isEncryption);
    uint32_t* key;
    QMutex keyMutex;


public:
    TEA();
    ~TEA();
    void runAlgo(const QString& inFile, const QString& outFile, bool encrypt);
    QString returnKey();
    void setKey(QString key);
};

#endif // TEA_H
