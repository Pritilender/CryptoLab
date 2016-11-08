#ifndef CRYPTODISPATCHER_H
#define CRYPTODISPATCHER_H

#include <cryptoworker.h>
#include <cryptoqueue.h>
#include <tea.h>

class CryptoDispatcher: public QThread
{
    Q_OBJECT
private:
    int runningThreads = 0;
    bool runMode = false;
    bool encryption = true;
    bool appRunning = true;
    bool xMode = false;
    QString outDir;
    CryptoAlgorithm* algo;
    CryptoQueue* queue;

    /**
     * @brief Dispatch a thread
     */
    void dispatch();

    void dispatchAllAndWait();
    void dispatchAllAndStop();
public:
    CryptoDispatcher(CryptoQueue* q);
    ~CryptoDispatcher();
    void run() Q_DECL_OVERRIDE;

signals:
    void writeConfig();

public slots:
    inline void setEncryption(bool enc) {
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

    inline void setKey(QString key) {
        this->algo->setKey(key);
    }

    inline void run(const bool enc) {
        this->queue->filterForEncryption(enc);
        this->encryption = enc;
        this->runMode = true;
    }

    inline void setXMode(const bool xMode) {
        this->xMode = xMode;
    }

    inline void setIV(QString iv) {
        ((TEA*)this->algo)->setIV(iv);
    }
};

#endif // CRYPTODISPATCHER_H
