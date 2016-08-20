#ifndef PRIMEMEMORYDUMP_H
#define PRIMEMEMORYDUMP_H

#include <QObject>

class PrimeMemoryDump : public QObject
{
    Q_OBJECT
public:
    explicit PrimeMemoryDump(QObject *parent = 0);

signals:

public slots:
};

#endif // PRIMEMEMORYDUMP_H