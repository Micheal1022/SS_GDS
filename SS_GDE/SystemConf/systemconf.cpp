#include "systemconf.h"
#include "ui_systemconf.h"
#include "MsgBox/msgbox.h"
#include "SqlManager/sqlmanager.h"


SystemConf::SystemConf(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemConf)
{
    ui->setupUi(this);
    initVariable();
    initWidget();
    initConnect();
}

SystemConf::~SystemConf()
{
    delete ui;
}

void SystemConf::delAllConf()
{
    QString pSqlQuery = "delete from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    SqlManager::delAllData(db,pSqlQuery);
    SqlManager::closeConnection(db);
}

void SystemConf::initWidget()
{
    initTableWidget(ui->tableWidget);
    confHostList(ui->tableWidget);
}

void SystemConf::initConnect()
{
    connect(ui->tBtnBack,   SIGNAL(clicked(bool)),this,SIGNAL(sigViewWidget()));
    connect(ui->tBtnAdd,    SIGNAL(clicked(bool)),this,SLOT(slotBtnAddHost()));
    connect(ui->tBtnDel,    SIGNAL(clicked(bool)),this,SLOT(slotBtnDelHost()));
    connect(ui->tBtnSave,   SIGNAL(clicked(bool)),this,SLOT(slotBtnSaveHost()));
    connect(ui->tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(slotCellCheckClick(int,int)));
}

void SystemConf::initVariable()
{

}

void SystemConf::confHostList(QTableWidget *tableWidget)
{
    tableWidget->clearContents();
    int pColumnCount = tableWidget->columnCount();
    QString pSqlQuery = "select NAME,HOST,PATH,ABLE,PORT_1,PATH_1,PORT_2,PATH_2,PORT_3,PATH_3,PORT_4,PATH_4,"
                        "PORT_5,PATH_5,PORT_6,PATH_6,PORT_7,PATH_7,PORT_8,PATH_8 from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> pNodeList = SqlManager::getHostList(db,pSqlQuery,pColumnCount);
    SqlManager::closeConnection(db);

    tableWidget->setRowCount(pNodeList.count());
    QFont ft("楷体",14);
    QTableWidgetItem *item;
    for (int row = 0; row < pNodeList.count(); row++) {
        QStringList pItemStr = pNodeList.at(row);

        QString pName   = pItemStr.at(S_NAME);
        QString pHost   = pItemStr.at(S_HOST);
        QString pPath   = pItemStr.at(S_PATH);
        QString pAble   = pItemStr.at(S_ABLE);
        QString pPort_1 = pItemStr.at(S_PORT_1);
        QString pPath_1 = pItemStr.at(S_PATH_1);
        QString pPort_2 = pItemStr.at(S_PORT_2);
        QString pPath_2 = pItemStr.at(S_PATH_2);
        QString pPort_3 = pItemStr.at(S_PORT_3);
        QString pPath_3 = pItemStr.at(S_PATH_3);
        QString pPort_4 = pItemStr.at(S_PORT_4);
        QString pPath_4 = pItemStr.at(S_PATH_4);
        QString pPort_5 = pItemStr.at(S_PORT_5);
        QString pPath_5 = pItemStr.at(S_PATH_5);
        QString pPort_6 = pItemStr.at(S_PORT_6);
        QString pPath_6 = pItemStr.at(S_PATH_6);
        QString pPort_7 = pItemStr.at(S_PORT_7);
        QString pPath_7 = pItemStr.at(S_PATH_7);
        QString pPort_8 = pItemStr.at(S_PORT_8);
        QString pPath_8 = pItemStr.at(S_PATH_8);

        tableWidget->setRowHeight(row,30);
        for (int column = 0;column < pColumnCount;column++) {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(Qt::black);
            switch (column) {
            case S_NAME:
                item->setText(pName);
                break;
            case S_HOST:
                item->setText(pHost);
                break;
            case S_PATH:
                item->setText(pPath);
                break;
            case S_ABLE:
                item->setFlags(Qt::NoItemFlags);
                if (pAble.toInt() == 1) {
                    item->setText("启用");
                    item->setCheckState(Qt::Checked);
                } else {
                    item->setText("禁用");
                    item->setCheckState(Qt::Unchecked);
                }
                break;
            case S_PORT_1:
                item->setText(pPort_1);
                break;
            case S_PATH_1:
                item->setText(pPath_1);
                break;
            case S_PORT_2:
                item->setText(pPort_2);
                break;
            case S_PATH_2:
                item->setText(pPath_2);
                break;
            case S_PORT_3:
                item->setText(pPort_3);
                break;
            case S_PATH_3:
                item->setText(pPath_3);
                break;
            case S_PORT_4:
                item->setText(pPort_4);
                break;
            case S_PATH_4:
                item->setText(pPath_4);
                break;
            case S_PORT_5:
                item->setText(pPort_5);
                break;
            case S_PATH_5:
                item->setText(pPath_5);
                break;
            case S_PORT_6:
                item->setText(pPort_6);
                break;
            case S_PATH_6:
                item->setText(pPath_6);
                break;
            case S_PORT_7:
                item->setText(pPort_7);
                break;
            case S_PATH_7:
                item->setText(pPath_7);
                break;
            case S_PORT_8:
                item->setText(pPort_8);
                break;
            case S_PATH_8:
                item->setText(pPath_8);
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void SystemConf::initTableWidget(QTableWidget *tableWidget)
{
    QString styleStr = "QHeaderView::section{font: 12pt '微软雅黑';background-color:rgb(0, 125, 165);color: white;}";
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    //tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    QStringList headList;
    headList<<tr("主机名称")<<tr("主机IP地址")<<tr("数据库")<<tr("状态")
           <<tr("端口-1")<<tr("图纸-1")<<tr("端口-2")<<tr("图纸-2")<<tr("端口-3")<<tr("图纸-3")<<tr("端口-4")<<tr("图纸-4")
          <<tr("端口-5")<<tr("图纸-5")<<tr("端口-6")<<tr("图纸-6")<<tr("端口-7")<<tr("图纸-7")<<tr("端口-8")<<tr("图纸-8");
    tableWidget->setColumnCount(headList.count());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->verticalHeader()->setFixedWidth(30);
    tableWidget->setColumnWidth(S_NAME,   250);
    tableWidget->setColumnWidth(S_HOST,   250);
    tableWidget->setColumnWidth(S_PATH,   90);
    tableWidget->setColumnWidth(S_ABLE,   80);
    tableWidget->setColumnWidth(S_PORT_1, 90);
    tableWidget->setColumnWidth(S_PATH_1, 90);
    tableWidget->setColumnWidth(S_PORT_2, 90);
    tableWidget->setColumnWidth(S_PATH_2, 90);
    tableWidget->setColumnWidth(S_PORT_3, 90);
    tableWidget->setColumnWidth(S_PATH_3, 90);
    tableWidget->setColumnWidth(S_PORT_4, 90);
    tableWidget->setColumnWidth(S_PATH_4, 90);
    tableWidget->setColumnWidth(S_PORT_5, 90);
    tableWidget->setColumnWidth(S_PATH_5, 90);
    tableWidget->setColumnWidth(S_PORT_6, 90);
    tableWidget->setColumnWidth(S_PATH_6, 90);
    tableWidget->setColumnWidth(S_PORT_7, 90);
    tableWidget->setColumnWidth(S_PATH_7, 90);
    tableWidget->setColumnWidth(S_PORT_8, 90);
    tableWidget->setColumnWidth(S_PATH_8, 90);


}


void SystemConf::slotBtnAddHost()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    QFont ft("微软雅黑",12);
    QTableWidgetItem *item;
    for (int column = 0;column < ui->tableWidget->columnCount();column++) {
        item = new QTableWidgetItem;
        item->setFont(ft);
        item->setTextColor(Qt::black);
        item->setTextAlignment(Qt::AlignCenter);
        switch (column) {
        case S_NAME:
            item->setText(tr("主机名称"));
            break;
        case S_HOST:
            item->setText(tr("主机IP地址"));
            break;
        case S_PATH:
            item->setText(tr("数据库路径"));
            break;
        case S_ABLE:
            item->setText("启用");
            item->setCheckState(Qt::Checked);
            item->setFlags(Qt::NoItemFlags);
            break;
        case S_PORT_1:
            item->setText(tr("0"));
            break;
        case S_PATH_1:
            item->setText(tr("0"));
            break;
        case S_PORT_2:
            item->setText(tr("0"));
            break;
        case S_PATH_2:
            item->setText(tr("0"));
            break;
        case S_PORT_3:
            item->setText(tr("0"));
            break;
        case S_PATH_3:
            item->setText(tr("0"));
            break;
        case S_PORT_4:
            item->setText(tr("0"));
            break;
        case S_PATH_4:
            item->setText(tr("0"));
            break;
        case S_PORT_5:
            item->setText(tr("0"));
            break;
        case S_PATH_5:
            item->setText(tr("0"));
            break;
        case S_PORT_6:
            item->setText(tr("0"));
            break;
        case S_PATH_6:
            item->setText(tr("0"));
            break;
        case S_PORT_7:
            item->setText(tr("0"));
            break;
        case S_PATH_7:
            item->setText(tr("0"));
            break;
        case S_PORT_8:
            item->setText(tr("0"));
            break;
        case S_PATH_8:
            item->setText(tr("0"));
            break;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,column,item);
    }
}

void SystemConf::slotBtnDelHost()
{
    int pRow = ui->tableWidget->currentRow();
    if (pRow >= 0 && ui->tableWidget->rowCount() != 1) {
        QString pName = ui->tableWidget->item(pRow,0)->text();
        QString pHost = ui->tableWidget->item(pRow,1)->text();
        ui->tableWidget->removeRow(pRow);
        QSqlDatabase db = SqlManager::openConnection();
        bool flag = SqlManager::delelteHostItem(db,pName,pHost);
        SqlManager::closeConnection(db);
        if (true == flag) {
            MsgBox::showInformation(this,tr("删除提示"),tr("该条信息已经删除!"),tr("关闭"));
        }
    } else {
        MsgBox::showInformation(this,tr("删除提示"),tr("地址信息至少要有一条!"),tr("关闭"));
    }
}

void SystemConf::slotBtnSaveHost()
{
    int pRowCount = ui->tableWidget->rowCount();
    if (pRowCount > 0) {
        for (int row = 0; row < pRowCount; row++) {
            QStringList pStringList;
            QString pName   = ui->tableWidget->item(row,S_NAME)->text();
            QString pHost   = ui->tableWidget->item(row,S_HOST)->text();
            QString pPath   = ui->tableWidget->item(row,S_PATH)->text();
            QString pAble   = ui->tableWidget->item(row,S_ABLE)->text();
            QString pPort_1 = ui->tableWidget->item(row,S_PORT_1)->text();
            QString pPath_1 = ui->tableWidget->item(row,S_PATH_1)->text();
            QString pPort_2 = ui->tableWidget->item(row,S_PORT_2)->text();
            QString pPath_2 = ui->tableWidget->item(row,S_PATH_2)->text();
            QString pPort_3 = ui->tableWidget->item(row,S_PORT_3)->text();
            QString pPath_3 = ui->tableWidget->item(row,S_PATH_3)->text();
            QString pPort_4 = ui->tableWidget->item(row,S_PORT_4)->text();
            QString pPath_4 = ui->tableWidget->item(row,S_PATH_4)->text();
            QString pPort_5 = ui->tableWidget->item(row,S_PORT_5)->text();
            QString pPath_5 = ui->tableWidget->item(row,S_PATH_5)->text();
            QString pPort_6 = ui->tableWidget->item(row,S_PORT_6)->text();
            QString pPath_6 = ui->tableWidget->item(row,S_PATH_6)->text();
            QString pPort_7 = ui->tableWidget->item(row,S_PORT_7)->text();
            QString pPath_7 = ui->tableWidget->item(row,S_PATH_7)->text();
            QString pPort_8 = ui->tableWidget->item(row,S_PORT_8)->text();
            QString pPath_8 = ui->tableWidget->item(row,S_PATH_8)->text();
            pStringList.append(pName);
            pStringList.append(pHost);
            pStringList.append(pPath);
            if (pAble == QString("启用")) {
                pStringList.append(QString("1"));
            } else {
                pStringList.append(QString("0"));
            }
            pStringList.append(pPort_1);
            pStringList.append(pPath_1);
            pStringList.append(pPort_2);
            pStringList.append(pPath_2);
            pStringList.append(pPort_3);
            pStringList.append(pPath_3);
            pStringList.append(pPort_4);
            pStringList.append(pPath_4);
            pStringList.append(pPort_5);
            pStringList.append(pPath_5);
            pStringList.append(pPort_6);
            pStringList.append(pPath_6);
            pStringList.append(pPort_7);
            pStringList.append(pPath_7);
            pStringList.append(pPort_8);
            pStringList.append(pPath_8);
            QSqlDatabase db = SqlManager::openConnection();
            bool pHostFlag = SqlManager::insertHostList(db,pStringList);
            bool pPngsFlag = SqlManager::insertPngsZoom(db,pHost);
            bool pViewFlag = SqlManager::insertViewZoom(db,pHost);
            SqlManager::closeConnection(db);
            if (false == pPngsFlag || false == pViewFlag || false == pHostFlag) {
                MsgBox::showInformation(this,tr("保存提示"),tr("信息保存失败！"),tr("关闭"));
                return;
            }
        }
        MsgBox::showInformation(this,tr("保存提示"),tr("信息保存成功！"),tr("关闭"));
        emit sigSaveOk();
    }
}

void SystemConf::slotCellCheckClick(int row, int column)
{
    if (column == S_ABLE) {
        if (ui->tableWidget->item(row,column)->checkState() == Qt::Unchecked) {
            ui->tableWidget->item(row,column)->setText(tr("启用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Checked);
        } else {
            ui->tableWidget->item(row,column)->setText(tr("禁用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
        }
    }
}
