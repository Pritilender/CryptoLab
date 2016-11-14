#include "knapsack.h"
#include <QFile>
#include <QSaveFile>
#include <QByteArray>
#include <QDataStream>

#include <QDebug>

Knapsack::Knapsack(uint32_t *privateKey, uint32_t *publicKey, uint32_t im, uint32_t n)
{
    this->privateKey = privateKey;
    this->publicKey = publicKey;
    this->im = im;
    this->n = n;
}

QByteArray Knapsack::encryptByKnapsack(QByteArray input, uint32_t *publicKey)
{
    QByteArray res;
    for (int i = 0; i < input.length(); i++) {
        QByteArray b;
        QDataStream str(&b, QIODevice::WriteOnly);
        char m = input[i];
        uint32_t c = 0;

        for (int j = 0; j < 8; j++) {
            c += ((m >> j) & 1) * publicKey[j];
        }

        str << c;
        res += b;
    }

    return res;
}

QByteArray Knapsack::decryptByKnapsack(QByteArray input, uint32_t* privateKey, uint32_t im, uint32_t n){
    QByteArray res;

    for (int i = 0; i < input.length(); i += 4) {
        QByteArray in = input.mid(i, 4);
        QDataStream stream(&in, QIODevice::ReadOnly);
        uint32_t c, tc;

        stream >> c;

        tc = (c * im) % n;

        QString binStr;
        for (int j = 7; j >= 0; j--) {
            if (tc >= privateKey[j] && tc != 0) {
                binStr.append('1');
                tc -= privateKey[j];
            } else {
                binStr.append('0');
            }
        }

        char resByte = (char)binStr.toInt(nullptr, 2);

        res += resByte;
    }

    return res;
}

void Knapsack::runAlgo(const QString &inFile, const QString &outFile, bool encrypt)
{
    qDebug() << "Start for" << inFile << "and" << encrypt << "to" << outFile;

    QFile inF(inFile);
    QSaveFile outF(outFile);

    if (inF.open(QIODevice::ReadOnly) && outF.open(QIODevice::WriteOnly)) {
        QByteArray input = inF.readAll();
        if (encrypt) {
            outF.write(encryptByKnapsack(input, this->publicKey));
        }
        else {
            outF.write(decryptByKnapsack(input, this->privateKey, this->im, this->n));
        }
        outF.commit();
        inF.close();
    }

    qDebug() << "End for" << inFile;
}
