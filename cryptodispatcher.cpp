#include "cryptodispatcher.h"
#include <QDebug>

CryptoDispatcher::CryptoDispatcher(CryptoQueue *q)
{
    this->queue = q;
    this->algo = new SimpleSubstitutioner();
}

CryptoDispatcher::~CryptoDispatcher()
{
    this->appRunning = false;
    delete this->algo;
}

void CryptoDispatcher::run()
{
    while (this->appRunning) {
        while (this->runMode) {
            if (this->runningThreads < 16 && this->queue->length() != 0) {
                this->runningThreads++;
                this->dispatch();
            }
        }
    }
}

void CryptoDispatcher::dispatch()
{
    QString inPath = this->queue->removeFirst();
    CryptoWorker *workerThread = new CryptoWorker(this->encryption, algo, inPath, this->outDir);
    QObject::connect(workerThread, &CryptoWorker::algorithmEnd, this, &CryptoDispatcher::threadEnd);
    QObject::connect(workerThread, &CryptoWorker::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
    this->runningThreads--;
}
