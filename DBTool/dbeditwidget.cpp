#include "dbeditwidget.h"
#include "ui_dbeditwidget.h"
#include <QFileDialog>
#include <QAxObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "MsgBox/msgbox.h"
DBEditWidget::DBEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DBEditWidget)
{
    ui->setupUi(this);
    initWidget();
    initConnect();
}

DBEditWidget::~DBEditWidget()
{
    delete ui;
}

void DBEditWidget::initWidget()
{
    initTableWidget(ui->tableWidget);
}

void DBEditWidget::initConnect()
{
    connect(ui->tBtnBack,SIGNAL(clicked(bool)),this,SIGNAL(sigBack()));
    connect(ui->tBtnImport,SIGNAL(clicked(bool)),this,SLOT(slotOpenExcelFile()));
    connect(ui->tBtnSaveDb,SIGNAL(clicked(bool)),this,SLOT(slotSaveDbFile()));
    connect(ui->tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(slotCellCheckClick(int,int)));
}

QList<QStringList> DBEditWidget::confTableInfo(QTableWidget *tableWidget)
{
    QList<QStringList> dbInfoList;
    for (int row = 0; row < tableWidget->rowCount();row++) {
        QStringList itemList;
        for (int column = 0; column <tableWidget->columnCount(); column++) {
            if (2 == column) {
                if (tr("启用") == tableWidget->item(row,column)->text()) {
                    itemList.append(tr("1"));
                } else {
                    itemList.append(tr("0"));
                }
            } else {
                itemList.append(tableWidget->item(row,column)->text());
            }
        }
        dbInfoList.append(itemList);
    }
    return dbInfoList;
}

bool DBEditWidget::saveDbInfo(QList<QStringList> dbInfoList, QString path)
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        database = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE");
    }
    database.setDatabaseName(path);

    if (!database.open()) {
        qDebug() << "Error: Failed to connect database."<<database.lastError();
    }

    QSqlQuery query(database);
    bool pFlag = true;
    database.transaction();
    for (int i = 0; i < dbInfoList.count(); i++) {
        QStringList pInfoList = dbInfoList.value(i);
        QString pLoop  = pInfoList.value(0);
        QString pID    = pInfoList.value(1);
        QString pState = pInfoList.value(2);
        QString pArea  = pInfoList.value(3);
        QString sqlQuery = QString("update NODELIST set ABLE = %1,AREA = '%2' where LOOP = %3 and ID = %4;").arg(pState).arg(pArea).arg(pLoop).arg(pID);
        bool flag = query.exec(sqlQuery);
        if (false == flag)
            break;
    }
    database.commit();
    query.finish();
    query.clear();
    database.close();
    //QT数据库移除
    QSqlDatabase::removeDatabase("QSQLITE");
    return pFlag;

}

void DBEditWidget::initTableWidget(QTableWidget *tableWidget)
{
    QStringList headList;
    headList<<tr("主机回路")<<tr("探测器编号")<<tr("状态")<<tr("安装位置");
    tableWidget->setColumnCount(headList.size());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    //tableWidget->horizontalHeader()->setDefaultSectionSize(140);
    QString pHeaderStyle ="QHeaderView::section{font: 12pt '微软雅黑'; background-color: rgb(0, 125, 165);color: white;}";
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->verticalHeader()->setStyleSheet(pHeaderStyle);
    tableWidget->verticalHeader()->setFixedWidth(35);
    tableWidget->verticalHeader()->setEnabled(false);
    tableWidget->verticalHeader()->setVisible(true);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

    tableWidget->horizontalHeader()->setStyleSheet(pHeaderStyle);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);
    tableWidget->setStyleSheet("QTableWidget::item:selected {color: rgb(255, 255, 255);background-color: rgb(0,  125, 165);}");

    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::NoSelection); //设置选择模式，选择单行
    tableWidget->setColumnWidth(0,100);
    tableWidget->setColumnWidth(1,100);
    tableWidget->setColumnWidth(2, 70);
    tableWidget->setColumnWidth(3,190);
}

void DBEditWidget::slotOpenExcelFile()
{
    QString strFile = QFileDialog::getOpenFileName(this,tr("选择Excel文件"),"",tr("Exel file(*.xls *.xlsx)"));
    if (strFile.isEmpty())  {
        return;
    }

    QAxObject excel("Excel.Application"); //加载Excel驱动
    excel.setProperty("Visible", false); //不显示Excel界面，如果为true会看到启动的Excel界面
    QAxObject* pWorkBooks = excel.querySubObject("WorkBooks");
    pWorkBooks->dynamicCall("Open (const QString&)", strFile);//打开指定文
    QAxObject* pWorkBook = excel.querySubObject("ActiveWorkBook");
    QAxObject* sheet = pWorkBook->querySubObject("WorkSheets(int)", 1);//获取工作表集合的工作表1，即sheet1
    QAxObject* range = sheet->querySubObject("UsedRange"); //获取该sheet的使用范围对象
    QVariant var = range->dynamicCall("Value");
    delete range;
    QVariantList varRows = var.toList();  //得到表格中的所有数据
    if (varRows.isEmpty()) {
        return;
    }
    int rowCount = varRows.size();
    ui->tableWidget->setRowCount(rowCount-1);
    QFont ft("微软雅黑",14);
    QTableWidgetItem *item;
    for (int row = 1; row < rowCount; row++) {
        ui->tableWidget->setRowHeight(row,25);
        QVariantList rowData = varRows[row].toList();
        for (int column = 0; column < ui->tableWidget->columnCount(); column++) {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextColor(QColor(0,0,0));
            if (2 == column) {
                item->setTextAlignment(Qt::AlignLeft);
                if (0 == rowData[column].toInt()) {
                    item->setText(tr("停用"));
                    item->setCheckState(Qt::Unchecked);
                } else {
                    item->setText(tr("启用"));
                    item->setCheckState(Qt::Checked);
                }
            }  else {
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(rowData[column].toString());
            }
            ui->tableWidget->setItem(row-1,column,item);
        }
    }
    pWorkBooks->dynamicCall("Close()");

}

void DBEditWidget::slotSaveDbFile()
{
    QString pFilePath = QFileDialog::getOpenFileName(this,tr("选择更新的数据库文件"),"",tr("SQL file(*.db)"));
    if (pFilePath.isEmpty()) {
        MsgBox::showInformation(this,tr(" 操作提示"),tr("数据库信息文件不存在!!!"),tr("关闭"));
        return;
    }

    bool pFlag = saveDbInfo(confTableInfo(ui->tableWidget),pFilePath);
    if (true == pFlag) {
        MsgBox::showInformation(this,tr(" 操作提示"),tr("数据库文件保存成功!!!"),tr("关闭"));
    } else {
        MsgBox::showInformation(this,tr(" 操作提示"),tr("数据库文件保存失败!!!"),tr("关闭"));
    }

}

void DBEditWidget::slotCellCheckClick(int row, int column)
{
    if (2 == column) {
        if (ui->tableWidget->item(row,column)->checkState() == Qt::Unchecked) {
            ui->tableWidget->item(row,column)->setText(tr("启用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Checked);
        } else {
            ui->tableWidget->item(row,column)->setText(tr("停用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
        }
    }
}
