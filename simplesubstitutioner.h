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
    SimpleSubstitutioner();
    void runAlgo(const QString &inFile, const QString &outFile, bool encrypt);
    QString returnKey();
    void setKey(QString key);
};

#endif // SIMPLESUBSTITUTIONER_H
