#include "recordinfo.h"
#include "ui_recordinfo.h"
#include "SqlManager/sqlmanager.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QTextEdit>
#include <QFile>
#include <QFileDialog>
#include "MsgBox/msgbox.h"
#include "includes.h"

#define ROWS    20
#define STEP    ROWS

RecordInfo::RecordInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordInfo)
{
    ui->setupUi(this);
    initVariable();
    initWidget();
    initConnect();
}

RecordInfo::~RecordInfo()
{
    delete ui;
}

void RecordInfo::initWidget()
{
    initTableWidget(ui->tableWidget);
    ui->tBtnAuto->setEnabled(false);
    ui->tBtnPageUp->setEnabled(false);
    ui->tBtnPageDown->setEnabled(false);
}

void RecordInfo::initConnect()
{
    connect(ui->tBtnAuto,    SIGNAL(clicked(bool)),this,SLOT(slotBtnAuto()));
    connect(ui->tBtnManual,  SIGNAL(clicked(bool)),this,SLOT(slotBtnManual()));
    connect(ui->tBtnCheck,   SIGNAL(clicked(bool)),this,SLOT(slotBtnCheck()));
    connect(ui->tBtnAllDel,  SIGNAL(clicked(bool)),this,SLOT(slotBtnAllDel()));
    connect(ui->tBtnExport,  SIGNAL(clicked(bool)),this,SLOT(slotBtnExport()));
    connect(ui->tBtnPageUp,  SIGNAL(clicked(bool)),this,SLOT(slotBtnPageUp()));
    connect(m_dateTimer,     SIGNAL(timeout()),    this,SLOT(slotDateTimeOut()));
    connect(ui->tBtnPageDown,SIGNAL(clicked(bool)),this,SLOT(slotBtnPageDown()));
    connect(ui->tBtnBack,    SIGNAL(clicked(bool)),this,SIGNAL(sigViewWidget()));
    connect(this,SIGNAL(sigPageUp(int)),ui->tableWidget->verticalScrollBar(),SLOT(setValue(int)));
    connect(this,SIGNAL(sigPageDown(int)),ui->tableWidget->verticalScrollBar(),SLOT(setValue(int)));
}

void RecordInfo::initVariable()
{
    m_rowCount = 0;
    m_pageCount = 0;
    m_pageNumber = 0;
    m_dateTimer = new QTimer;
    m_dateTimer->start(1000);
}

void RecordInfo::initTableWidget(QTableWidget *tableWidget)
{
    QString styleStr = "QHeaderView::section{font: 12pt '微软雅黑';background-color:rgb(0, 125, 165);color: white;}";
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    tableWidget->setStyleSheet("QTableWidget::item:selected {color: rgb(255, 255, 255);background-color: rgb(0,  125, 165);}");

    QStringList headList;
    headList<<tr("主机名称")<<tr("主机IP")<<tr("主机回路")<<tr("ID编号")<<tr("探测器类型")<<tr("事件类型")<<tr("事件时间")<<tr("安装区域");
    tableWidget->setColumnCount(headList.count());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->verticalHeader()->setFixedWidth(50);


    tableWidget->setColumnWidth(R_NAME, 200);
    tableWidget->setColumnWidth(R_HOST, 200);
    tableWidget->setColumnWidth(R_LOOP, 120);
    tableWidget->setColumnWidth(R_IDNO, 120);
    tableWidget->setColumnWidth(R_TYPE, 200);
    tableWidget->setColumnWidth(R_STATE,200);
    tableWidget->setColumnWidth(R_TIME, 300);
    tableWidget->setColumnWidth(R_AREA, 300);

}

QString RecordInfo::confQuerySql()
{
    QString pQuerySql = "select NAME,HOST,LOOP,ID,TYPE,STS,TIME,AREA from RECORD where ";
    QString pStartTime = QString::number(ui->dateTimeEditStart->dateTime().toTime_t());
    QString pStopTime  = QString::number(ui->dateTimeEditStop->dateTime().toTime_t());
    pQuerySql += QString("TIME between %1 and %2 order by TIME desc;").arg(pStartTime).arg(pStopTime);
    return pQuerySql;
}

void RecordInfo::showRecordList(QTableWidget *tableWidget, QString querySql)
{
    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    int pColumnCount = tableWidget->columnCount();
    //获取数据列表
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getRecordList(db,querySql,pColumnCount);
    SqlManager::closeConnection(db);

    m_rowCount = nodeList.count();
    m_pageNumber = 0;
    m_pageCount = m_rowCount / ROWS;
    if ((m_rowCount % ROWS) > 0) {
        m_pageCount += 1;
    }
    //如果页数大于1则可用
    if (m_rowCount > 1) {
        ui->tBtnPageDown->setEnabled(true);
    }

    tableWidget->setRowCount(m_rowCount);
    QFont ft("微软雅黑",12);
    QTableWidgetItem *item;
    for (int row = 0; row < nodeList.count(); row++) {
        QStringList itemStr = nodeList.at(row);

        QString pName = itemStr.at(R_NAME);
        QString pHost = itemStr.at(R_HOST);
        QString pLoop = itemStr.at(R_LOOP);
        QString pID   = itemStr.at(R_IDNO);
        QString pType = itemStr.at(R_TYPE);
        QString pState= itemStr.at(R_STATE);
        QString pTime = itemStr.at(R_TIME);
        QString pArea = itemStr.at(R_AREA);
        tableWidget->setRowHeight(row,29);
        for (int column = 0;column < pColumnCount; column++) {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setForeground(Qt::black);
            item->setTextAlignment(Qt::AlignCenter);
            switch (column) {
            case R_NAME:
                item->setText(pName);
                break;
            case R_HOST://主机地址
                item->setText(pHost);
                break;
            case R_LOOP://回路
                item->setText(pLoop);
                break;
            case R_IDNO://地址
                item->setText(pID);
                break;
            case R_TYPE://探测器类型
                switch (pType.toInt()) {
                case MOD_L12T4:
                    item->setText(tr("组合式电气火灾探测器"));
                    break;
                case MOD_L1T4:
                    item->setText(tr("一体式电气火灾探测器"));
                    break;
                }
                break;
            case R_STATE://事件类型
                item->setText(pState);
                break;
            case R_TIME://事件时间
                item->setText(QDateTime::fromTime_t(pTime.toUInt()).toString("yyyy/MM/dd hh:mm:ss"));
                break;
            case R_AREA://安装区域
                item->setText(pArea);
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void RecordInfo::slotBtnManual()
{
    m_dateTimer->stop();
    ui->tBtnAuto->setEnabled(true);
    ui->tBtnManual->setEnabled(false);
    ui->dateTimeEditStop->setEnabled(true);
    ui->dateTimeEditStart->setEnabled(true);
}

void RecordInfo::slotBtnAuto()
{
    m_dateTimer->start();
    ui->tBtnAuto->setEnabled(false);
    ui->tBtnManual->setEnabled(true);
    ui->dateTimeEditStop->setEnabled(false);
    ui->dateTimeEditStart->setEnabled(false);
}

void RecordInfo::slotBtnCheck()
{
    //显示查询结果
    showRecordList(ui->tableWidget,confQuerySql());
    //查询数据后向上按钮不可用
    ui->tBtnPageUp->setEnabled(false);
}

void RecordInfo::slotBtnAllDel()
{
    int ret = MsgBox::showQuestion(this,tr(" 记录删除"),tr("你确定要删除全部的记录信息吗？"),tr("确定"),tr("取消"));
    if (ret == 0) {
        QString sql = "delete from RECORD;";
        QSqlDatabase db = SqlManager::openConnection();

        if(SqlManager::delAllData(db,sql)) {
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);
        }
        SqlManager::closeConnection(db);
    }
}

void RecordInfo::slotBtnExport()
{
    int pRowCount = ui->tableWidget->rowCount();
    int pColumnCount = ui->tableWidget->columnCount();
    if (pRowCount == 0 || pColumnCount == 0) {
        MsgBox::showInformation(this,tr("记录导出"),tr("没有可用的数据,无法导出!!!"),tr("关闭"));
        return;
    }

    QString pFileName = QString(tr("历史记录_")+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm"));
    QString pFilePath = QFileDialog::getSaveFileName(this,tr("历史记录导出..."),pFileName,tr("EXCEL files(*.xls);;HTML-Files(*.txt);;"));
    if (pFilePath.isEmpty()) {
        MsgBox::showInformation(this,tr("记录导出"),tr("路径不存在,无法导出!!!"),tr("关闭"));
        return;
    }

    QList<QString> pStringList;
    //添加列标题
    QString pHeaderRow;
    for (int row = 0; row < pColumnCount; row++) {
        pHeaderRow.append(ui->tableWidget->horizontalHeaderItem(row)->text()+"\t");
    }
    pStringList.append(pHeaderRow);
    //添加item内容
    for(int row = 0; row < pRowCount; row++) {
        QString content;
        content.clear();
        for(int column = 0; column < pColumnCount; column++) {
            content += ui->tableWidget->item(row,column)->text() + "\t";
        }
        pStringList.append(content);
    }

    QTextEdit textEdit;
    for(int i = 0; i < pStringList.size(); i++) {
        textEdit.append(pStringList.at(i));
    }
    //文件保存
    QFile file(pFilePath);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        ts<<textEdit.document()->toPlainText();
        file.close();
    }
    MsgBox::showInformation(this,tr("记录导出"),tr("已经将所有数据导出"),tr("关闭"));
}

void RecordInfo::slotBtnPageDown()
{
    m_pageNumber++;
    if (m_pageNumber < m_pageCount) {
        emit sigPageDown(m_pageNumber * STEP);
        if (m_pageNumber == m_pageCount -1) {
            ui->tBtnPageDown->setEnabled(false);
            MsgBox::showInformation(this,tr("操作提示"),tr("已经到达记录底部."),tr("关闭"));
        }
        ui->tBtnPageUp->setEnabled(true);
    } else {
        m_pageNumber = m_pageCount - 1;
    }
}

void RecordInfo::slotDateTimeOut()
{
    QDateTime pDateTime = QDateTime::currentDateTime();
    ui->dateTimeEditStop->setDateTime(pDateTime);
}

void RecordInfo::slotBtnPageUp()
{
    m_pageNumber--;
    if (m_pageNumber >= 0) {
        emit sigPageUp(m_pageNumber * STEP);
        if (m_pageNumber == 0) {
            ui->tBtnPageUp->setEnabled(false);
            MsgBox::showInformation(this,tr("操作提示"),tr("已经到达数据顶部."),tr("关闭"));
        }
        ui->tBtnPageDown->setEnabled(true);
    } else {
        m_pageNumber = 0;
    }
}
