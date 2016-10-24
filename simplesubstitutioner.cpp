#include "simplesubstitutioner.h"

SimpleSubstitutioner::SimpleSubstitutioner(QString key)
    : CryptoAlgorithm()
{
    char letterA = 'a';
    for (int i = 0; i < key.length(); i++) {
        this->keyMapping.insert((char)(letterA + i), key.at(i).toLatin1());
    }
}

QString SimpleSubstitutioner::runAlgo(const QString src, bool encrypt)
{
    QString result;


    for (int i = 0; i < src.length(); i++) {
        const char inLetter = src.at(i).toLatin1();
        const char outLetter = encrypt ? this->keyMapping[inLetter] : this->keyMapping.key(inLetter);

        result.append(outLetter);
    }

    return result;
}
