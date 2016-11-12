#ifndef CRYPTOALGORITHM_H
#define CRYPTOALGORITHM_H

#include <QString>



class CryptoAlgorithm
{
public:
    CryptoAlgorithm() {}
    virtual ~CryptoAlgorithm() {}
    /**
     * @brief Run the cryptography algorithm in encryption or decryption mode.
     * @param src Source string
     * @param encrypt Choose encryption or decryption mode
     * @return Encrypted or decrypted string
     */
    virtual void runAlgo(const QString &inFile, const QString &outFile, bool encrypt) = 0;
    //virtual void setKey(QString key) = 0;
    //virtual QString returnKey() = 0;
};

#endif // CRYPTOALGORITHM_H
