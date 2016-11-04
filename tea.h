#ifndef TEA_H
#define TEA_H

#include "cryptoalgorithm.h"



class TEA : public CryptoAlgorithm
{
private:
    void feistelRound(quint32* v, quint32* k, bool isEncryption);
public:
    TEA();
    QString runAlgo(const QString &src, bool encrypt);
    QString returnKey();
};

#endif // TEA_H
