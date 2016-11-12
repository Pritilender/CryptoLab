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

void Knapsack::runAlgo(const QString &inFile, const QString &outFile, bool encrypt)
{
    qDebug() << "Start for" << inFile << "and" << encrypt << "to" << outFile;

    QFile inF(inFile);
    QSaveFile outF(outFile);

    if (inF.open(QIODevice::ReadOnly) && outF.open(QIODevice::WriteOnly)) {
        QByteArray input = inF.readAll();
        QDataStream out(&outF);

//        if (encrypt) {
//            //TODO
//        }

        for (int i = 0; encrypt && i < input.length(); i++) {
            //encryption
            QByteArray b;
            QDataStream str(&b, QIODevice::WriteOnly);
            char m = input[i];
            uint32_t c = 0;

            for (int j = 0; j < 8; j++) {
                c += ((m >> j) & 1) * this->publicKey[j];
            }

            str << c;
            outF.write(b);
        }

        for (int i = 0; !encrypt && i < input.length(); i += 4) {
            //decryption
            QByteArray in = input.mid(i, 4);
            QDataStream stream(&in, QIODevice::ReadOnly);
            uint32_t c, tc;

            stream >> c;

            tc = (c * this->im) % this->n;

            QString res, reverse;
            for (int j = 7; j >= 0; j--) {
                if (tc >= this->privateKey[j] && tc != 0) {
                    res.append('1');
                    tc -= this->privateKey[j];
                } else {
                    res.append('0');
                }
            }

//            for (int i = 0; i < 8; i++) {
//                reverse.append(res.at(res.length() - i - 1));
//            }

            QByteArray outBA;
            QDataStream outStream(&outBA, QIODevice::WriteOnly);
            char resByte = (char)res.toInt(nullptr, 2);

            outBA.append(resByte);

            outF.write(outBA);
        }

        outF.commit();
        inF.close();
    }

    qDebug() << "End for" << inFile;
}
