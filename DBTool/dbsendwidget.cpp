#include "dbsendwidget.h"
#include "ui_dbsendwidget.h"

DBSendWidget::DBSendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DBSendWidget)
{
    ui->setupUi(this);
    initWidget();
    initConnect();
}

DBSendWidget::~DBSendWidget()
{
    delete ui;
}

void DBSendWidget::initWidget()
{
    loadSize = 4*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    tcpClient = new QTcpSocket(this);
    ui->progressBar->setValue(0);
    ui->tBtnSendFile->setEnabled(false);
}

void DBSendWidget::initConnect()
{
    connect(ui->tBtnBack,SIGNAL(clicked(bool)),this,SIGNAL(sigBack()));
    //打开文件
    connect(ui->tBtnOpenFile,SIGNAL(clicked(bool)),this,SLOT(slotOpenFile()));
    //发送文件
    connect(ui->tBtnSendFile,SIGNAL(clicked(bool)),this,SLOT(slotSendFile()));
    //当连接服务器成功时，发出connected()信号，我们开始传送文件
    connect(tcpClient,SIGNAL(connected()),this,SLOT(slotStartTransfer()));
    //当有数据发送成功时，我们更新进度条
    connect(tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(updateProgress(qint64)));
    //显示故障信息
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
}

void DBSendWidget::slotOpenFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty()) {
        totalBytes = 0;
        bytesWritten = 0;
        ui->tBtnOpenFile->setEnabled(true);
        QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        ui->progressBar->setValue(0);
        ui->clientStatusLabel->setText(tr("打开文件: %1").arg(currentFileName));
        ui->tBtnSendFile->setEnabled(true);
    }
}

void DBSendWidget::slotSendFile()
{
    ui->tBtnSendFile->setEnabled(false);
    bytesWritten = 0;//初始化已发送字节为0
    ui->clientStatusLabel->setText(tr("连接中....."));
    tcpClient->connectToHost(ui->lineEditTargetHost->text(),ui->lineEditTargetPort->text().toInt());//连接
}

void DBSendWidget::slotStartTransfer()
{
    localFile = new QFile(fileName);
    if (!localFile->open(QFile::ReadOnly)) {
        return;
    }
    totalBytes = localFile->size();//文件总大小

    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    //文件全名大小,最后一次出现'/'的位置
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    //依次写入总大小信息空间，文件名大小信息空间，文件名
    sendOut << qint64(0) << qint64(0) << currentFileName;
    //这里的总大小是文件名大小等信息和实际文件大小的总和
    totalBytes += outBlock.size();
    sendOut.device()->seek(0);
    //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut<<totalBytes<<qint64((outBlock.size() - sizeof(qint64)*2));
    //发送完头数据后剩余数据的大小
    bytesToWrite = totalBytes - tcpClient->write(outBlock);
    ui->clientStatusLabel->setText("已连接");
    outBlock.resize(0);
}

void DBSendWidget::updateProgress(qint64 numBytes)
{
    bytesWritten += (int)numBytes;
    //已经发送数据的大小
    if (bytesToWrite > 0) {
        //如果已经发送了数据
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));
      //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
      //就发送剩余数据的大小
        bytesToWrite -= (int)tcpClient->write(outBlock);
       //发送完一次数据后还剩余数据的大小
        outBlock.resize(0);
        //清空发送缓冲区
    } else {
        localFile->close(); //如果没有发送任何数据，则关闭文件
    }
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesWritten);
    //更新进度条,发送完毕
    if (bytesWritten == totalBytes)  {
        QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
        ui->clientStatusLabel->setText(tr("传送文件 %1 成功").arg(currentFileName));
        localFile->close();
        tcpClient->close();
    }
}

void DBSendWidget::displayError(QAbstractSocket::SocketError)
{
    tcpClient->close();
    ui->progressBar->reset();
    ui->clientStatusLabel->setText(tr("客户端就绪"));
    ui->tBtnSendFile->setEnabled(true);
}
