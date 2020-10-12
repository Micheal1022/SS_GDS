#include "viewwidget.h"
#include "ui_viewwidget.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include "sensoritem.h"

#include "SqlManager/sqlmanager.h"
#include <QTreeWidget>
#include <QDebug>

ViewWidget::ViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewWidget)
{
    ui->setupUi(this);

    initVariable();
    initWidget();
    initConnect();
    initConfNode();

    /* 1.打开系统配置，查看那些IP的回路是可用的；
     * 2.创建目录树结构；
     * 3.打开数据,获取节点的地、缩放、安装位置、初始化ItemInfo内容；
     * 4.创建场景,添加图元信息；
     */

}

ViewWidget::~ViewWidget()
{
    delete ui;

}

QList<QStringList> ViewWidget::getNodeInfoList(QString loop, QString path)
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
        database = QSqlDatabase::database("qt_sql_default_connection");
    else {
        database = QSqlDatabase::addDatabase("QSQLITE");
    }
    database.setDatabaseName(path);


    if (!database.open()) {
        //qDebug()<<"Error: Failed to connect database."<<database.lastError();
    } else {
        //qDebug()<<"Succeed to connect database : loop "<<loop;
    }

    QList<QStringList> nodeInfoStringList;
    QString pQuerySql = QString("select LOOP,ID,ABLE,AREA,ZOOM,POS_X,POS_Y from NODELIST where LOOP = %1 and ABLE = 1 order by ID asc;").arg(loop);
    QSqlQuery query(database);
    if (query.exec(pQuerySql)) {
        while (query.next()) {
            QStringList nodeStringList;
            nodeStringList.append(query.value(0).toString());
            nodeStringList.append(query.value(1).toString());
            nodeStringList.append(query.value(2).toString());
            nodeStringList.append(query.value(3).toString());
            nodeStringList.append(query.value(4).toString());
            nodeStringList.append(query.value(5).toString());
            nodeStringList.append(query.value(6).toString());
            nodeInfoStringList.append(nodeStringList);
        }
    }
    query.finish();
    query.clear();
    database.close();
    //QT数据库移除
    QSqlDatabase::removeDatabase("QSQLITE");
    return nodeInfoStringList;
}

void ViewWidget::initWidget()
{
    //默认展开的状态
    ui->treeWidget->setItemsExpandable(true);
    ui->treeWidget->expandAll();
    ui->treeWidget->setColumnWidth(0,300);

    ui->tBtnQuitApp->setEnabled(false);
    ui->tBtnRecord->setEnabled(false);
    ui->tBtnSysConf->setEnabled(false);
}

void ViewWidget::initConnect()
{
    connect(ui->tBtnRecord,   SIGNAL(clicked(bool)),this,SIGNAL(sigRecordInfo()));
    connect(ui->tBtnSysConf,  SIGNAL(clicked(bool)),this,SIGNAL(sigSystemConf()));
    connect(ui->tBtnQuitApp,  SIGNAL(clicked(bool)),this,SIGNAL(sigAppQuit()));
    connect(ui->tBtnUserLogin,SIGNAL(clicked(bool)),this,SLOT(slotUserLogin()));


    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(slotItemClicked(QTreeWidgetItem*,int)));
}

void ViewWidget::initVariable()
{
    m_loginState = false;
    m_LoopItemList.clear();
    m_viewWidgetList.clear();
}

void ViewWidget::initConfNode()
{

    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> pHostList = SqlManager::getEnableHostList(db);
    SqlManager::closeConnection(db);
    for (int ind = 0 ; ind < pHostList.count(); ind++) {
        QStringList pItemStr = pHostList.value(ind);
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
        if (false == pHost.isEmpty() && false == pPath.isEmpty()) {
            QStringList pHostName;
            pHostName.append(pName);
            //根节点
            QTreeWidgetItem *pTreeRootItem = new QTreeWidgetItem(ui->treeWidget,pHostName);
            pTreeRootItem->setExpanded(true);
            QStringList pPortPathList;
            pPortPathList.append(pPort_1);     pPortPathList.append(pPath_1);
            pPortPathList.append(pPort_2);     pPortPathList.append(pPath_2);
            pPortPathList.append(pPort_3);     pPortPathList.append(pPath_3);
            pPortPathList.append(pPort_4);     pPortPathList.append(pPath_4);
            pPortPathList.append(pPort_5);     pPortPathList.append(pPath_5);
            pPortPathList.append(pPort_6);     pPortPathList.append(pPath_6);
            pPortPathList.append(pPort_7);     pPortPathList.append(pPath_7);
            pPortPathList.append(pPort_8);     pPortPathList.append(pPath_8);

            for (int index = 0; index < pPortPathList.count(); index+=2) {
                QString port = pPortPathList.at(index);
                QString path = pPortPathList.at(1+index);
                QString loop = QString::number(index/2+1);

                if (0 != port.toInt() && false == path.isEmpty()) {
                    //添加树形根节点
                    QStringList pLoopString;
                    pLoopString.append(QString("回路-%1").arg(loop));
                    QTreeWidgetItem *pLoopItem =  new QTreeWidgetItem(pTreeRootItem,pLoopString);
                    pTreeRootItem->addChild(pLoopItem);
                    m_LoopItemList.append(pLoopItem);
                    //获取回路的节点地址
                    SensorItemInfo itemInfo;
                    QList<SensorItemInfo> itemInfoList;
                    QList<QStringList> pNodeList = getNodeInfoList(loop,pPath);
                    //便利每个节点
                    for (int i = 0; i < pNodeList.count(); i++) {
                        itemInfo.m_loopStr = pNodeList.value(i).value(N_LOOP);
                        itemInfo.m_idStr   = pNodeList.value(i).value(N_ID);
                        itemInfo.m_typeStr = QString::number(1);
                        itemInfo.m_areaStr = pNodeList.value(i).value(N_AREA);
                        QString pZoomStr   = pNodeList.value(i).value(N_ZOOM);
                        itemInfo.m_zoom    = pZoomStr.toDouble();
                        QString pPosXstr   = pNodeList.value(i).value(N_POSX);
                        itemInfo.m_posX    = pPosXstr.toDouble();
                        QString pPosYstr   = pNodeList.value(i).value(N_POSY);
                        itemInfo.m_posY    = pPosYstr.toDouble();
                        itemInfoList.append(itemInfo);
                    }

                    GraphicsView *pView = new GraphicsView;
                    pView->confView(itemInfoList,loop,pName,pHost,port,path,pPath);
                    ui->stackedWidgetBuild->addWidget(pView);
                    m_viewWidgetList.append(pView);
                }
            }
        }
    }
}

void ViewWidget::slotItemClicked(QTreeWidgetItem *item, int index)
{
    Q_UNUSED(index)
    for (int ind = 0; ind < m_LoopItemList.count(); ind++) {
        if (item == m_LoopItemList.value(ind)) {
            ui->stackedWidgetBuild->setCurrentIndex(ind);
        }
    }
}

void ViewWidget::slotUserLoginOK()
{
    m_loginState = true;
    ui->tBtnQuitApp->setEnabled(true);
    ui->tBtnRecord->setEnabled(true);
    ui->tBtnSysConf->setEnabled(true);
    ui->tBtnUserLogin->setText(" 用户注销");
}

void ViewWidget::slotUserLogin()
{
    if (false == m_loginState) {
        emit sigUserLogin();
    } else {
        m_loginState = false;
        ui->tBtnQuitApp->setEnabled(false);
        ui->tBtnRecord->setEnabled(false);
        ui->tBtnSysConf->setEnabled(false);
        ui->tBtnUserLogin->setText(" 用户登录");
    }
}

void ViewWidget::slotConfNode()
{
    qDeleteAll(m_LoopItemList.begin(), m_LoopItemList.end());
    m_LoopItemList.clear();

    ui->treeWidget->clear();

    qDeleteAll(m_viewWidgetList.begin(), m_viewWidgetList.end());
    m_viewWidgetList.clear();
    initConfNode();
}












