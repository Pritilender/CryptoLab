#ifndef SIMPLESUBSTITUTIONER_H
#define SIMPLESUBSTITUTIONER_H
#include "QMap"
#include "QString"
#include "QVector"

class SimpleSubstitutioner
{
private:
    QMap<char, char> keyMapping;

public:
    SimpleSubstitutioner(QString key);
    QString substitute(QString src, bool encrypt);
};

#endif // SIMPLESUBSTITUTIONER_H
