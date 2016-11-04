#include "tea.h"

void TEA::feistelRound(quint32 *v, quint32 *k, bool isEncryption)
{
    quint32 v0 = v[0], v1 = v[1], k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    quint32 sum = isEncryption ? 0 : 0xC6EF3720;
    quint32 delta = 0x9e3779b9;

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

TEA::TEA()
{

}

QString TEA::runAlgo(const QString &src, bool encrypt)
{

}
