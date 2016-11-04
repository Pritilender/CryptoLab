#ifndef CRYPTOWORKER_H
#define CRYPTOWORKER_H

#include <QObject>

class cryptoworker : public QObject
{
    Q_OBJECT
public:
    explicit cryptoworker(QObject *parent = 0);

signals:

public slots:
};

#endif // CRYPTOWORKER_H