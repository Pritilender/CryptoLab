#include "cryptodispatcher.h"
#include <QDebug>

CryptoDispatcher::CryptoDispatcher(CryptoQueue *q)
{
    this->queue = q;
    this->algo = new TEA();
    this->array = nullptr;
    this->pub = new uint[DATA];
}

CryptoDispatcher::~CryptoDispatcher()
{
    this->appRunning = false;
    delete this->algo;
    if (this->array) {
        delete [] this->array;
    }
    delete [] this->pub;
}

void CryptoDispatcher::run()
{
    while (this->appRunning) {
        while (this->runMode) {
            this->runningThreads++;
            this->dispatch();
        }
    }
}

void CryptoDispatcher::runAlgo(const bool enc)
{
    for (int i = 0; i < 8; i++) {
        this->pub[i] = (this->array[i] * this->m) % this->n;
    }
    int k = 0;
    while ((1 + k * this->n) % this->m != 0) {
        k++;
    }
    this->im = (1 + k * this->n)/this->m;
    this->queue->filterForEncryption(enc);
    this->encryption = enc;
    this->runMode = true;
}

void CryptoDispatcher::dispatch()
{
    QString inPath = this->queue->removeFirst();
    CryptoWorker *workerThread = new CryptoWorker(this->encryption, algo, inPath, this->outDir);
    QObject::connect(workerThread, &CryptoWorker::algorithmEnd, this, &CryptoDispatcher::threadEnd);
    QObject::connect(workerThread, &CryptoWorker::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}
