#include "cryptodispatcher.h"
#include <QDebug>

CryptoDispatcher::CryptoDispatcher(CryptoQueue *q)
{
    this->queue = q;
    this->algo = new TEA();
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
            if (!this->bmpMode && this->runningThreads < 16 && this->queue->length() != 0) {
                this->runningThreads++;
                this->dispatch();
            } else if (this->next && this->queue->length() > 0) {
                this->next = false;
                this->dispatch();
            }
        }
    }
}

void CryptoDispatcher::dispatch()
{
    QString inPath = this->queue->removeFirst();
    if (this->bmpMode && !inPath.endsWith(".bmp")) {
        this->next = true;
        return;
    }
    CryptoWorker *workerThread = new CryptoWorker(this->encryption, algo, inPath, this->outDir,
                                                  this->xMode, this->bmpMode);
    QObject::connect(workerThread, SIGNAL(inBMPReady(QString)), this, SLOT(inBMPReady(QString)));
    QObject::connect(workerThread, SIGNAL(outBMPReady(QString)), this, SLOT(outBMPReady(QString)));
    QObject::connect(workerThread, &CryptoWorker::algorithmEnd, this, &CryptoDispatcher::threadEnd);
    QObject::connect(workerThread, &CryptoWorker::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}
