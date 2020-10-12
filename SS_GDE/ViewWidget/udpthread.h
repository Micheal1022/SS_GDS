#ifndef UDPTHREAD_H
#define UDPTHREAD_H

#include <QObject>
#include <QHostAddress>
#include "includes.h"

class QUdpSocket;
class UdpThread : public QObject
{
    Q_OBJECT
public:
    explicit UdpThread(QObject *parent = nullptr);
    UdpThread(QHostAddress host, quint16 port);
    ~UdpThread();
    void initConf(const QHostAddress &host, quint16 port);

private:
    QUdpSocket *m_udpSocket;

signals:
    void sigHostData(QByteArray byteArray);

private slots:
    void slotReadReady();
};

#endif // UDPTHREAD_H
