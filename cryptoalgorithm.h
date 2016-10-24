#ifndef CRYPTOALGORITHM_H
#define CRYPTOALGORITHM_H


class CryptoAlgorithm
{
public:
    CryptoAlgorithm(){}
    /**
     * @brief Run the cryptography algorithm in encryption or decryption mode.
     * @param src Source string
     * @param encrypt Choose encryption or decryption mode
     * @return Encrypted or decrypted string
     */
    virtual QString runAlgo(const QString src, bool encrypt) = 0;
};

#endif // CRYPTOALGORITHM_H
