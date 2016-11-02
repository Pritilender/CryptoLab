#include "a51.h"
#include "QDebug"

A51::A51(const QString &key)
{
    this->key = QBitArray(64);
    this->x = QBitArray(19);
    this->y = QBitArray(22);
    this->z = QBitArray(23);

    for (int i = 0; i < key.length(); i++) {
        this->key.setBit(i, key.at(i).toLatin1() == '0' ? false : true);
    }
}

QString A51::runAlgo(const QString &src, bool encrypt)
{
    Q_UNUSED(encrypt);
    QString res;

    this->resetRegisters();

    for (int i = 0; i < src.length(); i++) {
        res.append(this->stepByStep(src.at(i)));
    }

    return res;
}

QString A51::stepByStep(const QChar &srcChar){
    bool m = this->majorityVote();
    bool s;
    bool input = srcChar.toLatin1() == '1' ? true : false;

    if (this->x.at(8) == m) {
        this->stepX();
        emit this->xStepped(this->bitArrayToString(this->x));
    }
    if (this->y.at(10) == m) {
        this->stepY();
        emit this->yStepped(this->bitArrayToString(this->y));
    }
    if (this->z.at(10) == m) {
        this->stepZ();
        emit this->zStepped(this->bitArrayToString(this->z));
    }

    s = this->x.at(18) ^ this->y.at(21) ^ this->z.at(22);

    return (s ^ input) ? "1" : "0";
}

QString A51::returnKey()
{
    QString retKey;
    for (int i = 0; i < this->key.count(); i++) {
        retKey.append(this->key.at(i) ? '1' : '0');
    }
    return retKey;
}

void A51::stepX()
{
    bool t = this->x.at(13) ^ this->x.at(16) ^ this->x.at(17) ^ this->x.at(18);

    for (int i = 18; i > 0; i--) {
        this->x.setBit(i, this->x.at(i - 1));
    }

    this->x.setBit(0, t);
}

void A51::stepY()
{
    bool t = this->y.at(20) ^ this->y.at(21);

    for (int i = 21; i > 0; i--) {
        this->y.setBit(i, this->y.at(i - 1));
    }

    this->y.setBit(0, t);
}

void A51::stepZ()
{
    bool t = this->z.at(7) ^ this->z.at(20) ^ this->z.at(21) ^ this->z.at(22);

    for (int i = 22; i > 0; i--) {
        this->z.setBit(i, this->z.at(i - 1));
    }

    this->z.setBit(0, t);
}

/**
 * @brief A51::majorityVote
 * @return
 */
bool A51::majorityVote()
{
    int vote = 0;

    if (this->x.at(8)) vote++;
    if (this->y.at(10)) vote++;
    if (this->z.at(10)) vote++;

    return vote >= 2;
}

void A51::resetRegisters()
{
    for (int i = 0; i < 64; i++) {
        if (i < 19) {
            this->x.setBit(i, this->key.at(i));
        } else if (i < 19 + 22) {
            this->y.setBit(i - 19, this->key.at(i));
        } else {
            this->z.setBit(i - 19 - 22, this->key.at(i));
        }
    }
}

QString A51::bitArrayToString(const QBitArray &bArr)
{
    QString res;
    //qDebug() << bArr.size();
    for (int i = 0; i < bArr.size(); i++) {
        res.append(bArr.at(i) ? "1" : "0");
    }
    return res;
}
