#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "cryptoalgorithm.h"


class Knapsack : public CryptoAlgorithm
{
private:
    uint32_t* privateKey;
    uint32_t* publicKey;
    uint32_t im;
    uint32_t n;
public:
    Knapsack(){}
    ~Knapsack(){}
    Knapsack(uint32_t* privateKey, uint32_t* publicKey, uint32_t im, uint32_t n);
    void runAlgo(const QString &inFile, const QString &outFile, bool encrypt);
    static QByteArray encryptByKnapsack(QByteArray input, uint32_t *publicKey);
    static QByteArray decryptByKnapsack(QByteArray input, uint32_t *privateKey, uint32_t im, uint32_t n);
};

#endif // KNAPSACK_H
