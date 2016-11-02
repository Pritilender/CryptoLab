#ifndef A51_H
#define A51_H
#include "cryptoalgorithm.h"

#include <QBitArray>
#include <QObject>



class A51 : public QObject, public CryptoAlgorithm
{
    Q_OBJECT
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

    QString bitArrayToString(const QBitArray& bArr);
public:
    A51(const QString &key);
    QString runAlgo(const QString &src, bool encrypt);
    QString returnKey();
    QString stepByStep(const QChar &srcChar);
    void setKey(const QString &src) {
        for (int i = 0; i < src.length(); i++) {
            this->key.setBit(i, src.at(i).toLatin1() == '0' ? false : true);
        }
    }

signals:
    void xStepped(const QString);
    void yStepped(const QString);
    void zStepped(const QString);
};

#endif // A51_H
