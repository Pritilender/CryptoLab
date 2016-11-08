#include "tea.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QSaveFile>
#include <QImage>

void TEA::TEAFeistelRound(uint32_t *v, bool isEncryption)
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

void TEA::XTEAFeistelRound(uint32_t *v, bool isEncryption)
{
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t delta = 0x9e3779b9;
    uint32_t sum = isEncryption ? 0 : (delta * 32);

    if (isEncryption) {
        for (int i = 0; i < 32; i++) {
            v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + this->key[sum & 3]);
            sum += delta;
            v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + this->key[(sum >> 11) & 3]);
        }
    } else {
        for (int i = 0; i < 32; i++) {
            v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + this->key[(sum >> 11) & 3]);
            sum -= delta;
            v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + this->key[sum & 3]);
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

void TEA::encrypt(uint32_t *v, bool TEA, bool e)
{
    if (TEA) {
        this->TEAFeistelRound(v, e);
    } else {
        this->XTEAFeistelRound(v, e);
    }
}

uint32_t TEA::uintFromByteArray(QByteArray &arr)
{
    uint32_t res;
    QDataStream stream(&arr, QIODevice::ReadOnly);

    stream >> res;

    return res;
}

QByteArray TEA::byteArrayFromUint(uint32_t num)
{
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);

    stream << num;

    return res;
}

TEA::TEA()
{
    key = new uint32_t[4];
    iv = new uint32_t[2];
}

TEA::TEA(TEA *alg)
{
    qDebug() << "Cudan konstruktor";

    this->key = new uint32_t[4];
    this->key[0] = alg->key[0];
    this->key[1] = alg->key[1];
    this->key[2] = alg->key[2];
    this->key[3] = alg->key[3];

    this->iv = new uint32_t[2];
    this->iv[0] = alg->iv[0];
    this->iv[1] = alg->iv[1];
}

TEA::~TEA()
{
    delete[] key;
    delete[] iv;
}

void TEA::runAlgo(const QString &inFile, const QString &outFile, bool encrypt, bool xMode)
{
    qDebug() << "Start for" << inFile << "and" << encrypt;
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

        uint32_t iv[2] = {this->iv[0], this->iv[1]};
        uint32_t v[2];
        for (int i = encrypt ? 0 : 4; i < input.length(); i += 8) {
            QByteArray v0 = input.mid(i, 4);
            QByteArray v1 = input.mid(i + 4, 4);

            uint32_t p[2] = { this->uintFromByteArray(v0), this->uintFromByteArray(v1) };

            v0.clear();
            v1.clear();

            if (encrypt) {
                v[0] = p[0] ^ iv[0];
                v[1] = p[1] ^ iv[1];
            } else {
                v[0] = p[0];
                v[1] = p[1];
            }

            this->encrypt(v, xMode, encrypt);

            if (encrypt) {
                iv[0] = p[0] ^ v[0];
                iv[1] = p[1] ^ v[1];
            } else {
                v[0] ^= iv[0];
                v[1] ^= iv[1];

                iv[0] = p[0] ^ v[0];
                iv[1] = p[1] ^ v[1];
            }

            v0 = this->byteArrayFromUint(v[0]);
            v1 = this->byteArrayFromUint(v[1]);

            if (!encrypt && i + 8 == input.length() && bytesAdded > 0) {
                this->removeAddedBytes(bytesAdded, v0, v1);
            }

            outF.write(v0);
            outF.write(v1);
        }

        outF.commit();
        inF.close();
    }
    qDebug() << "End for" << inFile;
}

void TEA::encryptBMP(const QString &inFile, const QString &outFile, bool encrypt, bool xMode)
{
    qDebug() << "Start for" << inFile << "and" << encrypt;
    QFile inF(inFile);
    QSaveFile outF(outFile);

    if (inF.open(QIODevice::ReadWrite) &&
            outF.open(QIODevice::WriteOnly)) {
        this->keyMutex.lock();
        this->keyMutex.unlock();

        inF.reset();
        QByteArray input = inF.readAll();

        uint32_t pos = input[10] + 256 * (input[11] + 256 * (input[12] + 256 * input[13]));
        uint32_t iv[2] = {this->iv[0], this->iv[1]};
        uint32_t v[2];

        for (int i = encrypt ? 0 : 4; i < input.length(); i += 8) {
            QByteArray v0 = input.mid(i, 4);
            QByteArray v1 = input.mid(i + 4, 4);

            if (i > pos) {
                uint32_t p[2] = { this->uintFromByteArray(v0), this->uintFromByteArray(v1) };

                v0.clear();
                v1.clear();

                if (encrypt) {
                    v[0] = p[0] ^ iv[0];
                    v[1] = p[1] ^ iv[1];
                } else {
                    v[0] = p[0];
                    v[1] = p[1];
                }

                this->encrypt(v, xMode, encrypt);

                if (encrypt) {
                    iv[0] = p[0] ^ v[0];
                    iv[1] = p[1] ^ v[1];
                } else {
                    v[0] ^= iv[0];
                    v[1] ^= iv[1];

                    iv[0] = p[0] ^ v[0];
                    iv[1] = p[1] ^ v[1];
                }

                v0 = this->byteArrayFromUint(v[0]);
                v1 = this->byteArrayFromUint(v[1]);
            }

            outF.write(v0);
            outF.write(v1);
        }

        outF.commit();
        inF.close();
    }
    qDebug() << "End for" << inFile;
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

void TEA::setIV(QString IV)
{
    this->ivMutex.lock();
    QString block;
    for (int i = 0; i < IV.length(); i++) {
        block.append(IV.at(i));
        if ((i + 1) % 8 == 0) {
            bool ok;
            this->iv[((i + 1) / 8) - 1] = block.toUInt(&ok, 16);
            block.clear();
            if (!ok) {
                qDebug() << "Lose";
                return;
            }
        }
    }
    this->ivMutex.unlock();
}
