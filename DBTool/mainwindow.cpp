#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initWidget();
    initConnect();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidget()
{
    m_DBEditWidget = new DBEditWidget(ui->pageDbEdit);
    m_DBSendWidget = new DBSendWidget(ui->pageDbSend);
}

void MainWindow::initConnect()
{
    connect(m_DBEditWidget,SIGNAL(sigBack()),this,SLOT(slotMainWidget()));
    connect(m_DBSendWidget,SIGNAL(sigBack()),this,SLOT(slotMainWidget()));
    connect(ui->tBtnDbEdit,SIGNAL(clicked(bool)),this,SLOT(slotEditFile()));
    connect(ui->tBtnDbSend,SIGNAL(clicked(bool)),this,SLOT(slotSendFile()));
}


void MainWindow::slotEditFile()
{
    ui->stackedWidget->setCurrentWidget(ui->pageDbEdit);
}

void MainWindow::slotSendFile()
{
    ui->stackedWidget->setCurrentWidget(ui->pageDbSend);
}

void MainWindow::slotMainWidget()
{
    ui->stackedWidget->setCurrentWidget(ui->pageFunction);
}
