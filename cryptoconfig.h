#ifndef CRYPTOCONFIG_H
#define CRYPTOCONFIG_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QDateTime>

class CryptoConfig : public QObject
{
    Q_OBJECT
private:
    QFile *configFile;
    QMutex fileMutex;
    QString key;
    QString inDir;
    QString outDir;
    bool encryption = false;
    bool xMode = false;
    bool watchMode = false;
    uint lastEncrypted;

    void writeFile();
    void readFile();

public:
    explicit CryptoConfig(QString path, QObject *parent = 0);
    void read();

signals:
    void keyLoad(const QString &);
    void inDirLoad(const QString &);
    void outDirLoad(const QString &);
    void encryptionLoad(const bool);
    void watchModeLoad(const bool);
    void lastTime(const uint);
    void xModeLoad(const bool);

public slots:
    inline void setKey(const QString &key)
    {
        this->key = key;
    }
    inline void setInDir(const QString &inDir)
    {
        this->inDir = inDir;
    }
    inline void setOutDir(const QString &outDir)
    {
        this->outDir = outDir;
    }
    inline void setEncryption(const bool enc)
    {
        this->encryption = enc;
    }
    inline void setWatchMode(const bool wm)
    {
        this->watchMode = wm;
    }
    inline void write()
    {
        this->fileMutex.lock();
        this->writeFile();
        this->fileMutex.unlock();
    }
};

#endif // CRYPTOCONFIG_H
