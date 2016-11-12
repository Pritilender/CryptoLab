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
    QString array;
    uint32_t n;
    uint32_t m;
    QString inDir;
    QString outDir;
    bool encryption = false;
    bool watchMode = false;
    uint lastEncrypted;

    void writeFile();
    void readFile();

public:
    explicit CryptoConfig(QString path, QObject *parent = 0);
    void read();

signals:
    void arrayLoad(const QString &);
    void inDirLoad(const QString &);
    void outDirLoad(const QString &);
    void encryptionLoad(const bool);
    void watchModeLoad(const bool);
    void lastTime(const uint);
    void nLoad(const QString &);
    void mLoad(const QString &);

public slots:
    inline void setArray(const QString &array)
    {
        this->array = array;
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
    inline void setN(QString n){
        this->n = n.toUInt();
    }
    inline void setM(QString m){
        this->m = m.toUInt();
    }
};

#endif // CRYPTOCONFIG_H
