#include "simplesubstitutioner.h"
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QDebug>


SimpleSubstitutioner::SimpleSubstitutioner()
    : CryptoAlgorithm()
{

}

void SimpleSubstitutioner::runAlgo(const QString &inF, const QString &outF, bool encrypt)
{
    qDebug() << "Simple substitution for" << inF << "to" << outF;
    QFile inFile(inF);
    QSaveFile outFile(outF);

    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)
            && outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream inReader(&inFile);
        QTextStream outReader(&outFile);

        while (!inReader.atEnd()) {
            QString input;

            inReader >> input;

            input = input.remove(QRegExp("[^A-Za-z]")).toLower();

            for (int i = 0; i < input.length(); i++) {
                const char inLetter = input.at(i).toLatin1();
                const char outLetter = encrypt ? this->keyMapping[inLetter] : this->keyMapping.key(inLetter);

                outReader << outLetter;
            }
        }

        inFile.close();
        outFile.commit();
    }
}

QString SimpleSubstitutioner::returnKey()
{
    QString key;
    foreach (char c, this->keyMapping) {
        key += c;
    }

    return key;
}

void SimpleSubstitutioner::setKey(QString key)
{
    char letterA = 'a';
    for (int i = 0; i < key.length(); i++) {
        this->keyMapping.insert((char)(letterA + i), key.at(i).toLatin1());
    }
}
