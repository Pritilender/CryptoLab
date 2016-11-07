#include "cryptoconfig.h"

#include <QTextStream>

void CryptoConfig::readFile()
{
    QTextStream file(this->configFile);
    int boolHelper;
    uint timeStamp;

    file >> timeStamp;

    file >> boolHelper;
    this->encryption = boolHelper;

    file >> boolHelper;
    this->xMode = boolHelper;

    file >> boolHelper;
    this->watchMode = boolHelper;

    file >> this->inDir;
    file >> this->outDir;
    file >> this->key;
}

void CryptoConfig::writeFile()
{
    if (this->configFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream file(this->configFile);

        this->lastEncrypted = QDateTime().currentDateTime().toTime_t();
        file << this->lastEncrypted << "\n";

        file << this->encryption << "\n";

        file << this->xMode << "\n";

        file << this->watchMode << "\n";

        file << this->inDir << "\n";
        file << this->outDir << "\n";
        file << this->key << "\n";

        this->configFile->close();
    }
}

CryptoConfig::CryptoConfig(QString path, QObject *parent) : QObject(parent)
{
    this->configFile = new QFile(path);
}

void CryptoConfig::read()
{
    if (this->configFile->exists()) {
        if (this->configFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
            this->fileMutex.lock();
            this->readFile();
            this->configFile->close();
            this->fileMutex.unlock();

            emit this->inDirLoad(this->inDir);
            emit this->outDirLoad(this->outDir);
            emit this->encryptionLoad(this->encryption);
            emit this->keyLoad(this->key);
            emit this->watchModeLoad(this->watchMode);
            emit this->lastTime(this->lastEncrypted);
            emit this->xModeLoad(this->xMode);
        }
    }
}
