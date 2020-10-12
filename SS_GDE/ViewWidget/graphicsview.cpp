#include "graphicsview.h"
#include "sensoritem.h"
#include "ui_graphicsview.h"
#include <QGraphicsSvgItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>
#include <QSqlError>
#include "dbthead.h"
#include <QGraphicsSceneContextMenuEvent>
#include "SqlManager/sqlmanager.h"
#define POS 150
#define SET 100
GraphicsView::GraphicsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphicsView)
{
    ui->setupUi(this);

    initVariable();
    initWidget();
    initConnect();

}

GraphicsView::~GraphicsView()
{
    delete ui;
    delete m_scene;

    //配置网络和端口，创建线程
    QThread *t;
    foreach (t, m_threadList) {
        t->quit();
        t->wait();
    }

    qDeleteAll(m_threadList.begin(),m_threadList.end());
    m_threadList.clear();
}

void GraphicsView::initWidget()
{

    initTableWidget(ui->tableWidgetAlarm);
    initTableWidget(ui->tableWidgetError);

    ui->tBtnSave->setEnabled(false);
    ui->tBtnZoomIn->setEnabled(false);
    ui->tBtnRestore->setEnabled(false);
    ui->tBtnZoomOut->setEnabled(false);
    ui->tBtnAlignTop->setEnabled(false);
    ui->tBtnAlignLeft->setEnabled(false);
    ui->tBtnAlignRight->setEnabled(false);
    ui->tBtnAlignBottom->setEnabled(false);

    DBThead *pDbThread = new DBThead;
    QThread *pThread = new QThread;
    pDbThread->moveToThread(pThread);
    pThread->start();
    connect(this,SIGNAL(sigNodeInfoZoom(QList<SensorItemInfo>,QList<QPair<qreal,qreal>>,QStringList,QString)),
            pDbThread,SLOT(slotNodeInfoZoom(QList<SensorItemInfo>,QList<QPair<qreal,qreal> >,QStringList,QString)));

    m_infoTimer = new QTimer;
    connect(m_infoTimer,SIGNAL(timeout()),this,SLOT(slotInfoTimeOut()));
    m_infoTimer->start(1000);

}

void GraphicsView::initVariable()
{
    m_viewScale = 1.0;
    m_editModeFlag = false;
    qRegisterMetaType<QList<SensorItemInfo>>("QList<SensorItemInfo>");
    qRegisterMetaType<QList<SensorItem *>>("QList<SensorItem *>");
    qRegisterMetaType<QPair<qreal, qreal>>("QPair<qreal, qreal>");
    qRegisterMetaType<QList<QPair<qreal,qreal>>>("QList<QPair<qreal,qreal>>");
}

void GraphicsView::initConnect()
{
    connect(ui->tBtnEdit,   SIGNAL(clicked(bool)),this,SLOT(slotBtnEdit()));
    connect(ui->tBtnSave,   SIGNAL(clicked(bool)),this,SLOT(slotBtnSave()));
    connect(ui->tBtnZoomIn, SIGNAL(clicked(bool)),this,SLOT(slotBtnZoomIn()));
    connect(ui->tBtnZoomOut,SIGNAL(clicked(bool)),this,SLOT(slotBtnZoomOut()));
    connect(ui->tBtnRestore,SIGNAL(clicked(bool)),this,SLOT(slotBtnRestore()));
    connect(ui->tableWidgetAlarm,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(slotFindItemAlarmPos(int,int)));
    connect(ui->tableWidgetError,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(slotFindItemErrorPos(int,int)));

    connect(ui->tBtnAlignTop,SIGNAL(clicked(bool)),this,SLOT(slotAlignTop()));
    connect(ui->tBtnAlignLeft,SIGNAL(clicked(bool)),this,SLOT(slotAlignLeft()));
    connect(ui->tBtnAlignRight,SIGNAL(clicked(bool)),this,SLOT(slotAlignRight()));
    connect(ui->tBtnAlignBottom,SIGNAL(clicked(bool)),this,SLOT(slotAlignBottom()));
}

void GraphicsView::confView(QList<SensorItemInfo> itemInfoList, QString loop, QString hostName,
                            QString hostIP, QString port, QString &backGroundPath, QString dbPath)
{
    /*
     * 1.打开数据,获取节点的地、缩放、安装位置、初始化ItemInfo内容；
     * 2.创建场景,添加图元信息；
     * 3.添加背景图片；
     */
    m_hostIP = hostIP;
    m_dbPaht = dbPath;
    m_loop = loop;
    m_hostName = hostName;
    m_itemInfoList = itemInfoList;
    m_scene = new QGraphicsScene;
    m_scene->addPixmap(QPixmap(backGroundPath));

    QSqlDatabase pSqlDatabase = SqlManager::openConnection();
    qreal pViewScale = SqlManager::getViewZoom(pSqlDatabase,hostIP,loop);
    qreal pPngsScale = SqlManager::getPngsZoom(pSqlDatabase,hostIP,loop);
    SqlManager::closeConnection(pSqlDatabase);
    m_viewScale = pViewScale;

    //设置底图的缩放比例
    m_scene->items().value(0)->setScale(pPngsScale);
    //设置视窗的缩放比例
    ui->graphicsView->scale(pViewScale,pViewScale);

    ui->graphicsView->setScene(m_scene);
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    //初始化ItemInfo
    int pRowNum,pColumnNum;
    pRowNum = pColumnNum = 0;

    for (int ind = 0; ind < itemInfoList.count();ind++) {
        SensorItem *pItem = new SensorItem(itemInfoList.value(ind));

        pColumnNum = ind % 10;
        if (ind % 11 == 10) {
            pRowNum += 1;
        }
        //配置位置
        qreal pPosX = itemInfoList.value(ind).m_posX;
        qreal pPosY = itemInfoList.value(ind).m_posY;
        if (0 == pPosX && 0 == pPosY) {
            pItem->setPos(POS + SET * pColumnNum, POS + SET * pRowNum );
        } else {
            pItem->setPos(pPosX, pPosY);
        }
        //配置缩放
        qreal pZoom = itemInfoList.value(ind).m_zoom;
        if (1.0 == pZoom) {
            pItem->setScale(1.0);
        } else {
            pItem->setScale(itemInfoList.value(ind).m_zoom);
        }
        m_scene->addItem(pItem);
        //m_itemList.append(pItem);
    }

    m_selectItemList = m_scene->items();

    UdpThread *udpThread = new UdpThread(QHostAddress(hostIP) ,port.toUInt());
    QThread *thread = new QThread;
    udpThread->moveToThread(thread);
    m_threadList.append(thread);

    connect(udpThread,SIGNAL(sigHostData(QByteArray)),this,SLOT(slotHostData(QByteArray)));
    connect(thread,SIGNAL(finished()),udpThread,SLOT(deleteLater()),Qt::DirectConnection);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()),Qt::DirectConnection);
    thread->start();
}

void GraphicsView::initTableWidget(QTableWidget *tableWidget)
{
    QStringList headList;
    headList<<tr("主机名称")<<tr("回路")<<tr("编号")<<tr("事件类型")<<tr("事件时间");
    tableWidget->setColumnCount(headList.size());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(25);
    //tableWidget->horizontalHeader()->setDefaultSectionSize(140);
    QString hstyleStr ="QHeaderView::section{font: 12pt '微软雅黑'; background-color: rgb(0, 125, 165);color: white;}";
    QString vstyleStr ="QHeaderView::section{font: 12pt '微软雅黑'; background-color: rgb(0, 125, 165);color: white;}";
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->verticalHeader()->setStyleSheet(vstyleStr);
    tableWidget->verticalHeader()->setFixedWidth(35);
    tableWidget->verticalHeader()->setEnabled(false);
    tableWidget->verticalHeader()->setVisible(true);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);

    tableWidget->horizontalHeader()->setStyleSheet(hstyleStr);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);
    tableWidget->setStyleSheet("QTableWidget::item:selected {color: rgb(255, 255, 255);background-color: rgb(0,  125, 165);}");

    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::NoSelection); //设置选择模式，选择单行
    tableWidget->setColumnWidth(0,200);
    tableWidget->setColumnWidth(1,80);
    tableWidget->setColumnWidth(2,80);
    tableWidget->setColumnWidth(3,150);
    tableWidget->setColumnWidth(4,250);

}

void GraphicsView::setItem(QGraphicsScene *scene, QString loopStr, QString idStr, QString typeStr, QString stateStr)
{
    Q_UNUSED(scene)
    SensorItem *pItem;
    for (int i = 1; i < m_selectItemList.count(); i++) {
        int index = m_selectItemList.count() - 1 - i;
        pItem = qgraphicsitem_cast<SensorItem*>(m_selectItemList.at(index));
        if (pItem->m_loopStr == loopStr && pItem->m_idStr == idStr) {
            pItem->setItemType(typeStr.toInt());
            pItem->setItemState(stateStr.toInt());
            pItem->setToolTipString();
        }
    }
}

void GraphicsView::setNodeInfoZoom(QString loop, QString id, QPair<qreal, qreal> pox, QString scale, QString path)
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
    } else {
        qDebug() << "Succeed to connect database : loop "<<loop;
    }

    QSqlQuery query(database);

    QString sqlQuery = QString("update NODELIST set ZOOM = %1,POS_X = %2,POS_Y = %3 where LOOP = %4 and ID = %5;").\
            arg(scale).arg(QString::number(pox.first)).arg(QString::number(pox.second)).arg(loop).arg(id);
    qDebug()<<sqlQuery;
    query.exec(sqlQuery);
    query.finish();
    query.clear();
    database.close();
    //QT数据库移除
    QSqlDatabase::removeDatabase("QSQLITE");
}


void GraphicsView::slotBtnZoomIn()
{
    //放大
    int pSelectCount = m_scene->selectedItems().count();
    if (pSelectCount > 0) {
        foreach (QGraphicsItem *item, m_scene->selectedItems()) {
            item->setScale(item->scale() * 1.1);
        }
    } else {
        m_viewScale *= 1.1;
        ui->graphicsView->scale(1.1,1.1);
    }
}

void GraphicsView::slotBtnZoomOut()
{
    //缩小
    int pSelectCount = m_scene->selectedItems().count();
    if (pSelectCount > 0) {
        foreach (QGraphicsItem *item, m_scene->selectedItems()) {
            item->setScale(item->scale()*0.9);
        }
    } else {
        ui->graphicsView->scale(0.9,0.9);
        m_viewScale *= 0.9;
    }
}

void GraphicsView::slotBtnRestore()
{
    //取消所有变换
    int pSelectCount = m_scene->selectedItems().count();
    if (pSelectCount > 0){
        foreach (QGraphicsItem *item, m_scene->selectedItems()) {
            item->setRotation(0);
            item->setScale(1.0);
        }
    } else {
        m_viewScale = 1.0;
        ui->graphicsView->scale(1.0,1.0);
    }
}


//编辑模式
void GraphicsView::slotBtnEdit()
{
    if (false == m_editModeFlag) {
        m_editModeFlag = true;
        ui->tBtnEdit->setText(tr("取消"));
        ui->tBtnSave->setEnabled(true);
        ui->tBtnZoomIn->setEnabled(true);
        ui->tBtnRestore->setEnabled(true);
        ui->tBtnZoomOut->setEnabled(true);
        ui->tBtnAlignTop->setEnabled(true);
        ui->tBtnAlignLeft->setEnabled(true);
        ui->tBtnAlignRight->setEnabled(true);
        ui->tBtnAlignBottom->setEnabled(true);
        QList<QGraphicsItem *> itemList = m_scene->items();
        for (int ind = 0; ind < itemList.count(); ind++) {
            if (ind < itemList.count()- 1) {
                QGraphicsItem *item = itemList.value(ind);
                item->setFlag(QGraphicsItem::ItemIsSelectable);
                item->setFlag(QGraphicsItem::ItemIsFocusable);
                item->setFlag(QGraphicsItem::ItemIsMovable);
            }
        }
    } else {
        m_editModeFlag = false;
        ui->tBtnEdit->setText(tr("编辑"));
        ui->tBtnSave->setEnabled(false);
        ui->tBtnZoomIn->setEnabled(false);
        ui->tBtnRestore->setEnabled(false);
        ui->tBtnZoomOut->setEnabled(false);
        ui->tBtnAlignTop->setEnabled(false);
        ui->tBtnAlignLeft->setEnabled(false);
        ui->tBtnAlignRight->setEnabled(false);
        ui->tBtnAlignBottom->setEnabled(false);
        QList<QGraphicsItem *> itemList = m_scene->items();
        for (int ind = 0; ind < itemList.count(); ind++) {
            if (ind < itemList.count()- 1) {
                QGraphicsItem *item = itemList.value(ind);
                item->setFlag(QGraphicsItem::ItemIsMovable,   false);
                item->setFlag(QGraphicsItem::ItemIsFocusable, false);
                item->setFlag(QGraphicsItem::ItemIsSelectable,false);
            }
        }
    }
}
//保存编辑
void GraphicsView::slotBtnSave()
{
    ui->tBtnEdit->setText(tr("编辑"));
    //ui->tBtnEdit->setEnabled(true);
    ui->tBtnSave->setEnabled(false);
    ui->tBtnZoomIn->setEnabled(false);
    ui->tBtnRestore->setEnabled(false);
    ui->tBtnZoomOut->setEnabled(false);
    ui->tBtnAlignTop->setEnabled(false);
    ui->tBtnAlignLeft->setEnabled(false);
    ui->tBtnAlignRight->setEnabled(false);
    ui->tBtnAlignBottom->setEnabled(false);
    QSqlDatabase pSqlDatabase = SqlManager::openConnection();
    SqlManager::setViewZoom(pSqlDatabase,m_loop,QString::number(m_viewScale),m_hostIP);
    SqlManager::closeConnection(pSqlDatabase);

    qreal pScale;
    QStringList pScaleList;
    QList<QPair<qreal, qreal>> poxList;
    //获取item的列表，列表的顺序是最小的位置保存最上层的item;
    QList<QGraphicsItem *> itemList = m_scene->items();
    for (int i = 0; i < itemList.count(); i++) {
        SensorItem* pItem = qgraphicsitem_cast<SensorItem*>(itemList.value(itemList.count()- 1 - i));
        if (itemList.count()- 1 - i == 0) {
            //查找背景图片，保存缩放等级
            pScale = pItem->scale();
            QSqlDatabase pSqlDatabase = SqlManager::openConnection();
            SqlManager::setPngsZoom(pSqlDatabase,m_loop,QString::number(pScale),m_hostIP);
            SqlManager::closeConnection(pSqlDatabase);
        } else {
            //查找节点土元，保存缩放等级
            QPointF pScenePos = pItem->scenePos();
            pScale = pItem->scale();
            QPair<qreal,qreal> pPairPos;
            pPairPos.first = pScenePos.x();
            pPairPos.second= pScenePos.y();
            poxList.append(pPairPos);
            pScaleList.append(QString::number(pScale));
        }
        pItem->setFlag(QGraphicsItem::ItemIsMovable,   false);
        pItem->setFlag(QGraphicsItem::ItemIsFocusable, false);
        pItem->setFlag(QGraphicsItem::ItemIsSelectable,false);
    }
    emit sigNodeInfoZoom(m_itemInfoList, poxList, pScaleList,m_dbPaht);
}

void GraphicsView::analysisData(QByteArray hostData)
{
    int pLoop = hostData.at(DATA_LOOP);
    int pID   = (uchar)hostData.at(DATA_ID);
    int pType = hostData.at(DATA_TYPE);
    QString pLoopStr = QString::number(pLoop);
    QString pIDStr   = QString::number(pID);
    QString pTypeStr = QString::number(pType);

    int index;
    QString pDateTimeStr = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    QString pAlarmTime = QString::number(QDateTime::currentDateTime().toTime_t());
    QStringList pInfoList,pAlarmList;
    int pState =  hostData.at(DATA_STATE);
    switch (pState) {
    case N_NORMAL:

        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (true == m_itemInfoList.value(index).m_powerLostFlag) {
            m_itemInfoList[index].m_powerLostFlag = false;
            delStringList(m_alarmInfoList,pLoop,pID,N_POWERLOST);
        }
        if (true == m_itemInfoList.value(index).m_overVolFlag) {
            m_itemInfoList[index].m_overVolFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_OVERVOL);
        }
        if (true == m_itemInfoList.value(index).m_overCurFlag) {
            m_itemInfoList[index].m_overCurFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_OVERCUR);
        }
        if (true == m_itemInfoList.value(index).m_lackVolFlag) {
            m_itemInfoList[index].m_lackVolFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_LACKVOL);
        }
        if (true == m_itemInfoList.value(index).m_lackPhaFlag) {
            m_itemInfoList[index].m_lackPhaFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_LACKPHA);
        }
        if (true == m_itemInfoList.value(index).m_errorPhaFlag) {
            m_itemInfoList[index].m_errorPhaFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_ERRORPHA);
        }
        if (true == m_itemInfoList.value(index).m_errorFlag) {
            m_itemInfoList[index].m_errorFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_ERROR);
        }
        if (true == m_itemInfoList.value(index).m_alarmFlag) {
            m_itemInfoList[index].m_alarmFlag = false;
            delStringList(m_alarmInfoList,pLoop,pID,N_ALARM);
        }
        if (true == m_itemInfoList.value(index).m_offLineFlag) {
            m_itemInfoList[index].m_offLineFlag = false;
            delStringList(m_errorInfoList,pLoop,pID,N_OFFLINE);
        }
        break;
    case N_POWERLOST:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_powerLostFlag) {
            m_itemInfoList[index].m_powerLostFlag = true;
            QString pStateStr = tr("电源中断");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_alarmInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    case N_OVERVOL:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_overVolFlag) {
            m_itemInfoList[index].m_overVolFlag = true;
            QString pStateStr = tr("过压故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    case N_OVERCUR:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_overCurFlag) {
            m_itemInfoList[index].m_overCurFlag = true;
            QString pStateStr = tr("过流故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    case N_LACKVOL:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_lackVolFlag) {
            m_itemInfoList[index].m_lackVolFlag = true;
            QString pStateStr = tr("欠压故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    case N_LACKPHA:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_lackPhaFlag) {
            m_itemInfoList[index].m_lackPhaFlag = true;
            QString pStateStr = tr("缺相故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    case N_ERRORPHA:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_errorPhaFlag) {
            m_itemInfoList[index].m_errorPhaFlag = true;
            QString pStateStr = tr("错相故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    case N_ERROR:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_errorFlag) {
            m_itemInfoList[index].m_errorFlag = true;
            QString pStateStr = tr("探测器故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时故障列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史故障列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;

    case N_ALARM:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_alarmFlag) {
            m_itemInfoList[index].m_alarmFlag = true;
            QString pStateStr = QString("探测器报警");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时报警列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_alarmInfoList.append(pInfoList);
            //历史报警列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;

    case N_OFFLINE:
        index = findItemIndex(m_itemInfoList,pLoopStr,pIDStr);
        if (index < 0) {
            return;
        }
        if (false == m_itemInfoList.value(index).m_offLineFlag) {
            m_itemInfoList[index].m_offLineFlag = true;
            QString pStateStr = QString("通讯故障");
            QString pAreaStr  = m_itemInfoList.value(index).m_areaStr;
            //实时掉线列表
            pInfoList<<m_hostName<<pLoopStr<<pIDStr<<pStateStr<<pDateTimeStr;
            m_errorInfoList.append(pInfoList);
            //历史掉线列表
            pAlarmList<<m_hostName<<m_hostIP<<pLoopStr<<pIDStr<<pTypeStr<<pStateStr<<pAlarmTime<<pAreaStr;
            QSqlDatabase pDbDatabase = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(pDbDatabase,pAlarmList);
            SqlManager::closeConnection(pDbDatabase);
        }
        break;
    }
}

void GraphicsView::slotHostData(QByteArray hostData)
{
    QString pLoopStr = QString::number(hostData.at(DATA_LOOP));
    QString pIDStr   = QString::number((uchar)hostData.at(DATA_ID));
    QString pType    = QString::number(hostData.at(DATA_TYPE));
    QString pState   = QString::number(hostData.at(DATA_STATE));
    setItem(m_scene,pLoopStr,pIDStr,pType,pState);
    analysisData(hostData);

}

void GraphicsView::showInfoList(QTableWidget *tableWidget, QList<QStringList> infoList)
{
    tableWidget->clearContents();
    int columnCount = tableWidget->columnCount();
    //获取数据列表

    tableWidget->setRowCount(infoList.count());
    QFont ft("微软雅黑",12);
    int pRowCount = infoList.count();
    QTableWidgetItem *item;
    for (int row = 0; row < infoList.count();row++) {
        QStringList itemStr = infoList.at(pRowCount - 1 - row);
        tableWidget->setRowHeight(row,25);
        for (int column = 0;column < columnCount;column++) {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor(0,0,0));
            item->setText(itemStr.at(column));
            tableWidget->setItem(row,column,item);
        }
    }
}

void GraphicsView::delStringList(QList<QStringList> &infoList, int loop, int Id, int state)
{
    QString pLoopStr = QString::number(loop);
    QString canIdStr= QString::number(Id);
    QString pStateStr;

    switch (state) {
    case N_POWERLOST:
        pStateStr = tr("电源中断");
        break;
    case N_OVERVOL:
        pStateStr = tr("过压故障");
        break;
    case N_OVERCUR:
        pStateStr = tr("过流故障");
        break;
    case N_LACKVOL:
        pStateStr = tr("欠压故障");
        break;
    case N_LACKPHA:
        pStateStr = tr("缺相故障");
        break;
    case N_ERRORPHA:
        pStateStr = tr("错相故障");
        break;
    case N_ERROR:
        pStateStr = tr("探测器故障");
        break;
    case N_ALARM:
        pStateStr = tr("探测器报警");
        break;
    case N_OFFLINE:
        pStateStr = tr("通讯故障");
        break;
    }
    for( int index = 0;index < infoList.count();index++) {
        QString loopStr  = infoList.at(index).at(1);
        QString idStr    = infoList.at(index).at(2);
        QString stateStr = infoList.at(index).at(3);
        if(loopStr == pLoopStr && idStr == canIdStr && stateStr == pStateStr) {
            infoList.removeAt(index);
        }
    }
}

int GraphicsView::findItemIndex(QList<SensorItemInfo> itemInfoList, QString loop, QString id)
{
    for (int index = 0; index < itemInfoList.count(); index++) {
        SensorItemInfo itemInfo = itemInfoList.value(index);
        if (loop == itemInfo.m_loopStr && id == itemInfo.m_idStr) {
            return index;
        }
    }
    return -1;
}

void GraphicsView::slotInfoTimeOut()
{
    showInfoList(ui->tableWidgetAlarm,m_alarmInfoList);
    showInfoList(ui->tableWidgetError,m_errorInfoList);
}

void GraphicsView::slotFindItemAlarmPos(int row, int column)
{
    if (true == ui->tableWidgetAlarm->item(row,column)->isSelected()) {
        QString pLoopStr = ui->tableWidgetAlarm->item(row,1)->text();
        QString pIdStr   = ui->tableWidgetAlarm->item(row,2)->text();

        int index = findItemIndex(m_itemInfoList,pLoopStr,pIdStr);
        QPoint itemPos;
        itemPos.setX(m_itemInfoList.at(index).m_posX);
        itemPos.setY(m_itemInfoList.at(index).m_posY);
    }
}

void GraphicsView::slotAlignTop()
{
    if (m_scene->selectedItems().count()) {
        QList<QGraphicsItem *> itemList = m_scene->selectedItems();
        qreal pPosYmin = m_scene->selectedItems().at(0)->pos().y();
        //查找最小值
        for (int ind = 0; ind < itemList.count(); ind++) {
            QGraphicsItem *item = itemList.value(ind);
            qreal pItemPosY = item->pos().y();
            if (pPosYmin > pItemPosY) {
                pPosYmin = pItemPosY;
            }
        }
        //设置左对齐X轴坐标
        for (int ind = 0; ind < itemList.count(); ind++) {
            itemList.value(ind)->setY(pPosYmin);
        }
    }
}

void GraphicsView::slotAlignBottom()
{
    if (m_scene->selectedItems().count()) {
        QList<QGraphicsItem *> itemList = m_scene->selectedItems();
        qreal pPosYmax = m_scene->selectedItems().at(0)->pos().y();
        //查找最小值
        for (int ind = 0; ind < itemList.count(); ind++) {
            QGraphicsItem *item = itemList.value(ind);
            qreal pItemPosY = item->pos().y();
            if (pPosYmax < pItemPosY) {
                pPosYmax = pItemPosY;
            }
        }
        //设置左对齐X轴坐标
        for (int ind = 0; ind < itemList.count(); ind++) {
            itemList.value(ind)->setY(pPosYmax);
        }
    }
}

void GraphicsView::slotAlignLeft()
{
    if (m_scene->selectedItems().count()) {
        QList<QGraphicsItem *> itemList = m_scene->selectedItems();
        qreal pPosXmin = m_scene->selectedItems().at(0)->pos().x();
        //查找最小值
        for (int ind = 0; ind < itemList.count(); ind++) {
            QGraphicsItem *item = itemList.value(ind);
            qreal pItemPosX = item->pos().x();
            if (pPosXmin > pItemPosX) {
                pPosXmin = pItemPosX;
            }
        }
        //设置左对齐X轴坐标
        for (int ind = 0; ind < itemList.count(); ind++) {
            itemList.value(ind)->setX(pPosXmin);
        }
    }
}

void GraphicsView::slotAlignRight()
{
    if (m_scene->selectedItems().count()) {
        QList<QGraphicsItem *> itemList = m_scene->selectedItems();
        qreal pPosXmax = m_scene->selectedItems().at(0)->pos().x();
        //查找最小值
        for (int ind = 0; ind < itemList.count(); ind++) {
            QGraphicsItem *item = itemList.value(ind);
            qreal pItemPosX = item->pos().x();
            if (pPosXmax < pItemPosX) {
                pPosXmax = pItemPosX;
            }
        }
        //设置左对齐X轴坐标
        for (int ind = 0; ind < itemList.count(); ind++) {
            itemList.value(ind)->setX(pPosXmax);
        }
    }
}

void GraphicsView::slotFindItemErrorPos(int row, int column)
{
    if (true == ui->tableWidgetError->item(row,column)->isSelected()) {
        QString pLoopStr = ui->tableWidgetError->item(row,1)->text();
        QString pIdStr   = ui->tableWidgetError->item(row,2)->text();

        int index = findItemIndex(m_itemInfoList,pLoopStr,pIdStr);
        QPointF itemPos;//item的坐标
        itemPos.setX(m_itemInfoList.at(index).m_posX);
        itemPos.setY(m_itemInfoList.at(index).m_posY);
        //qDebug()<<"*********itemPos**********";
        //qDebug()<<"m_posX    -----> "<<itemPos.x();
        //qDebug()<<"m_posY    -----> "<<itemPos.y();
    }
}






