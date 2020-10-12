#include "dbthead.h"

DBThead::DBThead(QObject *parent) : QObject(parent)
{

}
/*
* @项目   TBus_SSEF
* @函数   bool slotInsertAlarmInfo(int loop, int type, int state, int canId, int value, uint time, QString area);
* @描述   通过线程槽函数进行存储，较少事务等待时间，防止冻结界面
* @作者   Micheal(陈明明)
* @时间   2019-04-09
*/

void DBThead::slotNodeInfoZoom(QList<SensorItemInfo> itemInfoList, QList<QPair<qreal, qreal> > poxList, QStringList scaleList, QString path)
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
        //qDebug() << "Succeed to connect database : loop "<<loop;
    }

    QSqlQuery query(database);
    database.transaction();
    for (int i = 0; i < itemInfoList.count(); i++) {
        QPair<qreal, qreal> pPairPos = poxList.value(i);
        SensorItemInfo pItemInfo = itemInfoList.value(i);
        QString sqlQuery = QString("update NODELIST set ZOOM = %1,POS_X = %2,POS_Y = %3 where LOOP = %4 and ID = %5;").\
                arg(scaleList.value(i)).arg(QString::number(pPairPos.first)).arg(QString::number(pPairPos.second)).arg(pItemInfo.m_loopStr).arg(pItemInfo.m_idStr);
        //qDebug()<<sqlQuery;
        query.exec(sqlQuery);
    }
    database.commit();
    query.finish();
    query.clear();
    database.close();
    //QT数据库移除
    QSqlDatabase::removeDatabase("QSQLITE");
}

