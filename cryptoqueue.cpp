#include "cryptoqueue.h"

void CryptoQueue::insertFile(const QString &file)
{
    this->listMutex.lock();

    this->fileNames.append(file);

    this->listMutex.unlock();
}

void CryptoQueue::updateFile(const QString &file)
{
    this->listMutex.lock();

    if (!this->fileNames.contains(file)) {
        this->fileNames.append(file);
    }

    this->listMutex.unlock();
}

void CryptoQueue::removeFile(const QString &file)
{
    this->listMutex.lock();

    if (this->fileNames.contains(file)) {
        this->fileNames.removeOne(file);
    }

    this->listMutex.unlock();
}

QString CryptoQueue::removeFirst()
{
    QString first;

    if (this->fileNames.length() != 0) {
        this->listMutex.lock();

        first = this->fileNames.at(0);
        this->fileNames.removeFirst();

        this->listMutex.unlock();
    }

    return first;
}

QList<QString> CryptoQueue::filterForEncryption(bool encryption)
{
    QRegExp inExtension(encryption ? "^(?:(?!.\\.crypto).)+$" : "[.]crypto$");
    QList<QString> removed;
    QMutableStringListIterator it(this->fileNames);

    this->listMutex.lock();

    while (it.hasNext()) {
        QString next = it.next();

        if (inExtension.indexIn(next) == -1) {
            removed.append(removed);
            it.remove();
        }
    }

    this->listMutex.unlock();

    return removed;
}
