#ifndef CRYPTOQUEUE_H
#define CRYPTOQUEUE_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>



class CryptoQueue: public QObject
{
    Q_OBJECT

private:
    QList<QString> fileNames;
    QMutex listMutex;
public:
    CryptoQueue():QObject() {}
    QList<QString> filterForEncryption(bool encryption);
    inline int length(){
        return this->fileNames.length();
    }
    inline QList<QString> content(){
        return this->fileNames;
    }
public slots:
    void insertFile(const QString& file);
    void updateFile(const QString& file);
    void removeFile(const QString& file);
    QString removeFirst();
};

#endif // CRYPTOQUEUE_H
