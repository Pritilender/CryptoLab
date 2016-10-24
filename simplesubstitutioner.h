#ifndef SIMPLESUBSTITUTIONER_H
#define SIMPLESUBSTITUTIONER_H
#include <QMap>
#include <QString>
#include <QVector>
#include <cryptoalgorithm.h>

class SimpleSubstitutioner: public CryptoAlgorithm
{
private:
    QMap<char, char> keyMapping;

public:
    SimpleSubstitutioner(QString key);
    QString runAlgo(QString src, bool encrypt);
};

#endif // SIMPLESUBSTITUTIONER_H
