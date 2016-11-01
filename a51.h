#ifndef A51_H
#define A51_H
#include "cryptoalgorithm.h"

#include <QBitArray>



class A51 : public CryptoAlgorithm
{
private:
    QBitArray key;
    QBitArray x;
    QBitArray y;
    QBitArray z;

    void stepX();
    void stepY();
    void stepZ();
    bool majorityVote();
    void resetRegisters();
public:
    A51(const QString &key);
    QString runAlgo(const QString &src, bool encrypt);
    QString returnKey();
};

#endif // A51_H
