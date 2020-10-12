#ifndef DBSENDWIDGET_H
#define DBSENDWIDGET_H

#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QByteArray>
#include <QFileDialog>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/qabstractsocket.h>

namespace Ui {
class DBSendWidget;
}

class DBSendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DBSendWidget(QWidget *parent = 0);
    ~DBSendWidget();

private:
    Ui::DBSendWidget *ui;
    void initWidget();
    void initConnect();
    QTcpSocket *tcpClient;
    QFile *localFile;      //要发送的文件
    qint64 totalBytes;     //数据总大小
    qint64 bytesWritten;   //已经发送数据大小
    qint64 bytesToWrite;   //剩余数据大小
    qint64 loadSize;       //每次发送数据的大小
    QString fileName;      //保存文件路径
    QByteArray outBlock;   //数据缓冲区，即存放每次要发送的数据
signals:
    void sigBack();
private slots:
    void slotOpenFile();
    void slotSendFile();
    void slotStartTransfer();//发送文件大小等信息
    void updateProgress(qint64 numBytes); //发送数据，更新进度条
    void displayError(QAbstractSocket::SocketError); //显示错误
};

#endif // DBSENDWIDGET_H
