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
    QString returnKey(){
        QString key;
        foreach(char c, this->keyMapping) {
            key += c;
        }

        return key;
    }
};

#endif // SIMPLESUBSTITUTIONER_H
