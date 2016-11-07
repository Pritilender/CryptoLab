#include "tea.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QSaveFile>

void TEA::feistelRound(uint32_t *v, bool isEncryption)
{
    uint32_t v0 = v[0], v1 = v[1], k0 = this->key[0], k1 = this->key[1], k2 = this->key[2],
             k3 = this->key[3];
    uint32_t sum = isEncryption ? 0 : 0xC6EF3720;
    uint32_t delta = 0x9e3779b9;

    if (isEncryption) {
        for (int i = 0; i < 32; i++) {
            sum += delta;
            v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
            v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        }
    } else {
        for (int i = 0; i < 32; i++) {
            v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
            v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
            sum -= delta;
        }
    }

    v[0] = v0;
    v[1] = v1;
}

int TEA::getBytesAdded(QByteArray &inputBuffer)
{
    QDataStream bytesAddedBuffer(&inputBuffer, QIODevice::ReadOnly);
    int bytesAdded = 0;

    bytesAddedBuffer >> bytesAdded;

    return bytesAdded;
}

void TEA::fillBuffer(QSaveFile &outFile, QByteArray &inputBuffer)
{
    QByteArray numberBuffer;
    QDataStream numberStream (&numberBuffer, QIODevice::WriteOnly);
    int bytesAdded = 0;

    while (inputBuffer.length() % 8) {
        bytesAdded++;
        inputBuffer.append('0');
    }

    numberStream << bytesAdded;

    outFile.write(numberBuffer);
}

void TEA::removeAddedBytes(int bytesToRemove, QByteArray &v0, QByteArray &v1)
{
    int bytesv0 = bytesToRemove % 4;
    int bytesv1 = bytesToRemove;

    v0.chop(bytesv0);
    v1.chop(bytesv1);
}

TEA::TEA()
{
    key = new uint32_t[4];
}

TEA::~TEA()
{
    delete[] key;
}

void TEA::runAlgo(const QString &inFile, const QString &outFile, bool encrypt)
{
    QFile inF(inFile);
    QSaveFile outF(outFile);

    if (inF.open(QIODevice::ReadWrite | QIODevice::Append) &&
            outF.open(QIODevice::WriteOnly)) {
        this->keyMutex.lock();
        this->keyMutex.unlock();

        inF.reset();
        QByteArray input = inF.readAll();
        int bytesAdded = 0;

        if (encrypt) {
            this->fillBuffer(outF, input);
        } else {
            bytesAdded = getBytesAdded(input);
        }
        for (int i = encrypt ? 0 : 4; i < input.length(); i += 8) {
            uint32_t v[2];
            QByteArray v0 = input.mid(i, 4);
            QByteArray v1 = input.mid(i + 4, 4);

            QDataStream streamIn0(&v0, QIODevice::ReadOnly);
            QDataStream streamIn1(&v1, QIODevice::ReadOnly);
            QDataStream streamOut0(&v0, QIODevice::WriteOnly);
            QDataStream streamOut1(&v1, QIODevice::WriteOnly);

            streamIn0 >> v[0];
            streamIn1 >> v[1];

            v0.clear();
            v1.clear();

            this->feistelRound(v, encrypt);

            streamOut0 << v[0];
            streamOut1 << v[1];

            if (!encrypt && i + 8 == input.length() && bytesAdded > 0) {
                this->removeAddedBytes(bytesAdded, v0, v1);
            }

            outF.write(v0);
            outF.write(v1);
        }

        outF.commit();
        inF.close();
    }
}

QString TEA::returnKey()
{
    QString res;
    for (int i = 0; i < 8; i++) {
        res.append(QString::number(this->key[i], 16));
    }
    return res;
}

void TEA::setKey(QString key)
{
    this->keyMutex.lock();
    QString block;
    for (int i = 0; i < key.length(); i++) {
        block.append(key.at(i));
        if ((i + 1) % 8 == 0) {
            bool ok;
            this->key[((i + 1) / 8) - 1] = block.toUInt(&ok, 16);
            block.clear();
            if (!ok) {
                qDebug() << "Lose";
                return;
            }
        }
    }
    this->keyMutex.unlock();
}
