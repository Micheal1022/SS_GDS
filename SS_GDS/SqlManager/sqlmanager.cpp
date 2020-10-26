#include "sqlmanager.h"




QMutex SqlManager::mutex;
QWaitCondition SqlManager::waitConnection;
SqlManager* SqlManager::instance = NULL;
SqlManager::SqlManager()
{
    dataBaseName = "./SS_GDS.db";
    dataBaseType = "QSQLITE";
    testOnBorrow = true;

    maxWaitTime  = 1000;
    waitInterval = 200;
    maxConntCount = 10;
}

QSqlDatabase SqlManager::createConnection(const QString &connectionName)
{
    // 连接已经创建过了,复用它,而不是重新创建
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase dbPre = QSqlDatabase::database(connectionName);

        if (testOnBorrow) {
            //返回连接前访问数据库，如果连接断开，重新建立连接
            QSqlQuery query(dbPre);
            if (query.lastError().type() != QSqlError::NoError && !dbPre.open()) {
                return QSqlDatabase();
            }
        }
        return dbPre;
    }

    // 创建一个新的连接
    QSqlDatabase db = QSqlDatabase::addDatabase(dataBaseType, connectionName);
    db.setDatabaseName(dataBaseName);
    if (!db.open()){
        return QSqlDatabase();
    }

    return db;
}

void SqlManager::release()
{
    QMutexLocker locker(&mutex);
    delete instance;
    instance = nullptr;
}

QSqlDatabase SqlManager::openConnection()
{
    SqlManager& pool = SqlManager::getInstance();
    QMutexLocker locker(&mutex);

    // 已创建连接数
    int conntCount = pool.unusedConntNames.size() + pool.usedConntNames.size();

    // 如果连接已经用完,等待 waitInterval 毫秒看看是否有可用连接,最长等待 maxWaitTime 毫秒
    for (int i=0;i<pool.maxWaitTime&&pool.unusedConntNames.size()==0&&conntCount==pool.maxConntCount;i+=pool.waitInterval) {
        waitConnection.wait(&mutex,pool.waitInterval);
        // 重新计算已创建连接数
        conntCount = pool.unusedConntNames.size() + pool.usedConntNames.size();
    }
    QString connectionName;
    if(pool.unusedConntNames.size() > 0) {
        // 有已经回收的连接，复用它们
        connectionName = pool.unusedConntNames.dequeue();
    } else if(conntCount < pool.maxConntCount) {
        // 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
        connectionName = QString("Connection-%1").arg(conntCount + 1);
    } else {
        // 已经达到最大连接数
        //qDebug() << "Cannot create more connections.";
        return QSqlDatabase();
    }

    // 创建连接
    QSqlDatabase db = pool.createConnection(connectionName);

    // 有效的连接才放入 usedConnectionNames
    if (db.isOpen()) {
        pool.usedConntNames.enqueue(connectionName);
    }

    return db;
}

void SqlManager::closeConnection(QSqlDatabase connection)
{
    SqlManager& pool = SqlManager::getInstance();
    QString connectionName = connection.connectionName();

    // 如果是我们创建的连接，从 used 里删除,放入 unused 里
    if (pool.usedConntNames.contains(connectionName)) {
        QMutexLocker locker(&mutex);
        pool.usedConntNames.removeOne(connectionName);
        pool.unusedConntNames.enqueue(connectionName);
        waitConnection.wakeOne();
    }
}

SqlManager::~SqlManager()
{
    // 销毁连接池的时候删除所有的连接
    foreach (QString connectionName, usedConntNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool SqlManager::insertAlarmRecord(QSqlDatabase db, QStringList stringList)
{
    QString pName = stringList.value(0);
    QString pHost = stringList.value(1);
    QString pLoop = stringList.value(2);
    QString pID   = stringList.value(3);
    QString pType = stringList.value(4);
    QString pSts  = stringList.value(5);
    QString pTime = stringList.value(6);
    QString pArea = stringList.value(7);

    QString pSqlQuery = QString("insert into RECORD values('%1','%2',%3,%4,%5,'%6',%7,'%8');").arg(pName).arg(pHost).arg(pLoop).\
            arg(pID).arg(pType).arg(pSts).arg(pTime).arg(pArea);
    qDebug()<<"pSqlQuery -----> "<<pSqlQuery;
    QSqlQuery pQuery(db);
    if (!pQuery.exec(pSqlQuery)) {
        return false;
    }
    pQuery.finish();
    pQuery.clear();
    return true;
}
//返回图片的缩放倍数
qreal SqlManager::getPngsZoom(QSqlDatabase db, QString host, QString loop)
{
    qreal pZoom = 1.0;
    QString pSqlQuery = QString("select ZOOM_%1 from PNGZOOM where HOST = '%2';").arg(loop).arg(host);
    //qDebug()<<"pSqlQuery -----> "<<pSqlQuery;
    QSqlQuery pQuery(db);
    if (pQuery.exec(pSqlQuery)) {
        if(pQuery.next()) {
            pZoom = pQuery.value(0).toReal();
        }
    }
    pQuery.finish();
    pQuery.clear();
    return pZoom;
}

bool SqlManager::setPngsZoom(QSqlDatabase db, QString loop, QString level, QString host)
{
    QString pSqlQuery = QString("update PNGZOOM set ZOOM_%1 = %2 where HOST = '%3';").arg(loop).arg(level).arg(host);
    //qDebug()<<pSqlQuery;
    QSqlQuery pQuery(db);
    if (!pQuery.exec(pSqlQuery)) {
        return false;
    }
    pQuery.finish();
    pQuery.clear();
    return true;
}

bool SqlManager::insertPngsZoom(QSqlDatabase db, QString host)
{
    QString sqlQuery = QString("replace into PNGZOOM values('%1',1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);").arg(host);
    //qDebug()<<"sqlQuery --->"<<sqlQuery;
    QSqlQuery query(db);
    if (!query.exec(sqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

qreal SqlManager::getViewZoom(QSqlDatabase db, QString host, QString loop)
{
    qreal pZoom = 1.0;
    QString pSqlQuery = QString("select VIEW_%1 from VIEWZOOM where HOST = '%2';").arg(loop).arg(host);
    //qDebug()<<"pSqlQuery -----> "<<pSqlQuery;
    QSqlQuery pQuery(db);
    if (pQuery.exec(pSqlQuery)) {
        if(pQuery.next()) {
            pZoom = pQuery.value(0).toReal();
        }
    }
    pQuery.finish();
    pQuery.clear();
    return pZoom;
}

bool SqlManager::setViewZoom(QSqlDatabase db, QString loop, QString level, QString host)
{
    QString pSqlQuery = QString("update VIEWZOOM set VIEW_%1 = %2 where HOST = '%3';").arg(loop).arg(level).arg(host);
    //qDebug()<<pSqlQuery;
    QSqlQuery pQuery(db);
    if (!pQuery.exec(pSqlQuery)) {
        return false;
    }
    pQuery.finish();
    pQuery.clear();
    return true;
}

bool SqlManager::insertViewZoom(QSqlDatabase db, QString host)
{
    QString sqlQuery = QString("replace into VIEWZOOM values('%1',1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);").arg(host);
    //qDebug()<<"sqlQuery --->"<<sqlQuery;
    QSqlQuery query(db);
    if (!query.exec(sqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

SqlManager &SqlManager::getInstance()
{
    if (nullptr == instance) {
        QMutexLocker locker(&mutex);
        if (nullptr == instance) {
            instance = new SqlManager();
        }
    }
    return *instance;
}

bool SqlManager::insertHostList(QSqlDatabase db, QStringList stringList)
{
    QString pName   = stringList.value(0);
    QString pHost   = stringList.value(1);
    QString pPath   = stringList.value(2);
    QString pAble   = stringList.value(3);
    QString pPort_1 = stringList.value(4);
    QString pPath_1 = stringList.value(5);
    QString pPort_2 = stringList.value(6);
    QString pPath_2 = stringList.value(7);
    QString pPort_3 = stringList.value(8);
    QString pPaht_3 = stringList.value(9);
    QString pPort_4 = stringList.value(10);
    QString pPath_4 = stringList.value(11);
    QString pPort_5 = stringList.value(12);
    QString pPath_5 = stringList.value(13);
    QString pPort_6 = stringList.value(14);
    QString pPath_6 = stringList.value(15);
    QString pPort_7 = stringList.value(16);
    QString pPath_7 = stringList.value(17);
    QString pPort_8 = stringList.value(18);
    QString pPath_8 = stringList.value(19);


    QString sqlQuery = QString("replace into HOSTINFO values('%1','%2','%3',%4,"
                               "%5, '%6' ,%7,'%8' ,"
                               "%9, '%10',%11,'%12',"
                               "%13,'%14',%15,'%16',"
                               "%17,'%18',%19,'%20');").\
            arg(pName).arg(pHost).arg(pPath).arg(pAble).\
            arg(pPort_1).arg(pPath_1).arg(pPort_2).arg(pPath_2).\
            arg(pPort_3).arg(pPaht_3).arg(pPort_4).arg(pPath_4).\
            arg(pPort_5).arg(pPath_5).arg(pPort_6).arg(pPath_6).\
            arg(pPort_7).arg(pPath_7).arg(pPort_8).arg(pPath_8);
    qDebug()<<"sqlQuery --->"<<sqlQuery;
    QSqlQuery query(db);
    if (!query.exec(sqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

bool SqlManager::delelteHostItem(QSqlDatabase db, QString name, QString host)
{
    QString pSqlQuery = QString("delete from HOSTINFO where NAME = '%1' and HOST = '%2';").arg(name).arg(host);
    QSqlQuery query(db);
    if (!query.exec(pSqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();
    return true;
}

QList<QStringList> SqlManager::getRecordList(QSqlDatabase db, QString sqlQuery, int columnCount)
{
    QList<QStringList> pRecordList;
    QSqlQuery pQuery(db);
    if (pQuery.exec(sqlQuery)) {
        while(pQuery.next()) {
            QStringList pNodeList;
            for(int index = 0;index < columnCount; index++) {
                pNodeList.append(pQuery.value(index).toString());
            }
            pRecordList.append(pNodeList);
        }
    }
    pQuery.finish();
    pQuery.clear();
    return pRecordList;
}

bool SqlManager::delAllData(QSqlDatabase db,const QString &sqlQuery)
{

    QSqlQuery query(db);
    if(!query.exec(sqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();

    return true;
}

bool SqlManager::delelteRecordItem(QSqlDatabase db, QStringList stringList)
{
    QString pName = stringList.value(0);
    QString pHost = stringList.value(1);
    QString pLoop = stringList.value(2);
    QString pID   = stringList.value(3);
    QString pSts  = stringList.value(4);
    QString pTime = stringList.value(5);
    QString sqlQuery = QString("delete from RECORD where "
                               "NAME = '%1' and HOST = '%2' and "
                               "LOOP = %3 and ID = '%4' and STS = '%5' and TIME = %6;").arg(pName).arg(pHost).arg(pLoop).arg(pID).arg(pSts).arg(pTime);
    QSqlQuery query(db);
    if (!query.exec(sqlQuery)) {
        return false;
    }
    query.finish();
    query.clear();

    return true;
}

QList<QStringList> SqlManager::getHostList(QSqlDatabase db, const QString sqlQuery, int columnCount)
{
    QList<QStringList> list;
    QSqlQuery query(db);
    if (query.exec(sqlQuery)) {
        while(query.next()) {
            QStringList nodeList;
            for(int index = 0;index < columnCount;index++) {
                nodeList.append(query.value(index).toString());
            }
            list.append(nodeList);
        }
    }
    query.finish();
    query.clear();

    return list;
}

QList<QStringList> SqlManager::getEnableHostList(QSqlDatabase db)
{
    QString pSqlQuery = "select NAME,HOST,PATH,ABLE,PORT_1,PATH_1,PORT_2,PATH_2,PORT_3,PATH_3,PORT_4,PATH_4,"
                        "PORT_5,PATH_5,PORT_6,PATH_6,PORT_7,PATH_7,PORT_8,PATH_8 from HOSTINFO where ABLE = 1;";
    //qDebug()<<"pSqlQuery --->"<<pSqlQuery;
    QList<QStringList> list;
    QSqlQuery query(db);
    if (query.exec(pSqlQuery)) {
        while(query.next()) {
            QStringList nodeList;
            for(int index = 0;index < 20;index++) {
                nodeList.append(query.value(index).toString());
            }
            list.append(nodeList);
        }
    }
    query.finish();
    query.clear();
    return list;
}
