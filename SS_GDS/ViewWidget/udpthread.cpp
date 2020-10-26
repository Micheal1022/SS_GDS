#include "udpthread.h"
#include <QDebug>
#include <QThread>
#include <QUdpSocket>
#include <QNetworkInterface>


UdpThread::UdpThread(QObject *parent) : QObject(parent)
{

}

UdpThread::UdpThread(QHostAddress host, quint16 port)
{
    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(host,port);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpThread::slotReadReady);
}


UdpThread::~UdpThread()
{
    delete m_udpSocket;
    m_udpSocket = nullptr;

}

void UdpThread::slotReadReady()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray pHostData;
        pHostData.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(pHostData.data(), pHostData.size());
        //qDebug()<<"datagram : "<<pHostData;
        if (DATASIZE == pHostData.size()) {
            if ((uchar)HEAD_AA == (uchar)pHostData.at(DATA_HEAD) && (uchar)TAIL_FF == (uchar)pHostData.at(DATA_TAIL)) {
                emit sigHostData(pHostData);
            }
        }
    }
}


