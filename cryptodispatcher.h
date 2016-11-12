#ifndef CRYPTODISPATCHER_H
#define CRYPTODISPATCHER_H

#include <cryptoworker.h>
#include <cryptoqueue.h>
#include <tea.h>

#define DATA 8

class CryptoDispatcher: public QThread
{
    Q_OBJECT
private:
    int runningThreads = 0;
    bool runMode = false;
    bool encryption = true;
    bool appRunning = true;
    QString outDir;
    uint32_t m;
    uint32_t n;
    uint32_t im;
    uint* array;
    uint* pub;
    CryptoAlgorithm *algo;
    CryptoQueue *queue;

    /**
     * @brief Dispatch a thread
     */
    void dispatch();

    void dispatchAllAndWait();
    void dispatchAllAndStop();
public:
    CryptoDispatcher(CryptoQueue *q);
    ~CryptoDispatcher();
    void run() Q_DECL_OVERRIDE;

signals:
    void writeConfig();

public slots:
    inline void setEncryption(bool enc)
    {
        this->encryption = enc;
    }

    inline void setOutDir(const QString &outDir)
    {
        this->outDir = outDir;
    }

    inline void threadEnd()
    {
        this->runningThreads--;
        emit writeConfig();
    }

    inline void setArray(QString array)
    {
        //this->algo->setKey(key);
        QStringList ar = array.split(',');

        if (this->array) {
            delete [] this->array;
        }
        this->array = new uint[ar.length()];

        for (int i = 0; i < ar.length(); i++) {
            this->array[i] = ar.at(i).toUInt();
        }
    }

    void runAlgo(const bool enc);

    inline void setM(QString m)
    {
        this->m = m.toUInt();
    }

    inline void setN(QString n)
    {
        this->n = n.toUInt();
    }
};

#endif // CRYPTODISPATCHER_H
