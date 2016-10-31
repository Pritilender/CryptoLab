#include "simplesubstitutioner.h"

SimpleSubstitutioner::SimpleSubstitutioner(QString key)
    : CryptoAlgorithm()
{
    char letterA = 'a';
    for (int i = 0; i < key.length(); i++) {
        this->keyMapping.insert((char)(letterA + i), key.at(i).toLatin1());
    }
}

QString SimpleSubstitutioner::runAlgo(const QString& src, bool encrypt)
{
    QString in(src);
    QString result;

    // Clean up the string and lower case it
    in = in.remove(QRegExp("[^A-Za-z]")).toLower();

    for (int i = 0; i < in.length(); i++) {
        const char inLetter = in.at(i).toLatin1();
        const char outLetter = encrypt ? this->keyMapping[inLetter] : this->keyMapping.key(inLetter);

        result.append(outLetter);
    }

    return result;
}
