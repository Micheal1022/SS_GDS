#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QDebug>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QtSql/QSql>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>




#define S_NAME      0
#define S_HOST      1
#define S_PATH      2
#define S_ABLE      3
#define S_PORT_1    4
#define S_PATH_1    5
#define S_PORT_2    6
#define S_PATH_2    7
#define S_PORT_3    8
#define S_PATH_3    9
#define S_PORT_4    10
#define S_PATH_4    11
#define S_PORT_5    12
#define S_PATH_5    13
#define S_PORT_6    14
#define S_PATH_6    15
#define S_PORT_7    16
#define S_PATH_7    17
#define S_PORT_8    18
#define S_PATH_8    19


#define N_LOOP  0
#define N_ID    1
#define N_ABLE  2
#define N_AREA  3
#define N_ZOOM  4
#define N_POSX  5
#define N_POSY  6

class SqlManager
{

public:

    //PUBLIC
    ~SqlManager();

    enum ListMode{
        DisplayList = 4,
        HostList = 5,
        RecordList = 7
    };

    // 关闭所有的数据库连接
    static void release();
    // 获取数据库连接
    static QSqlDatabase openConnection();
    // 释放数据库连接回连接池
    static void closeConnection(QSqlDatabase connection);


    //hostList
    static QList<QStringList> getHostList(QSqlDatabase db, const QString sqlQuery, int columnCount);
    static QList<QStringList> getEnableHostList(QSqlDatabase db);
    //
    static bool insertHostList(QSqlDatabase db, QStringList stringList);
    static bool delelteHostItem(QSqlDatabase db, QString name, QString host);
    //record
    static QList<QStringList> getRecordList(QSqlDatabase db, QString sqlQuery, int columnCount);
    static bool delAllData(QSqlDatabase db,const QString &sqlQuery);
    static bool delelteRecordItem(QSqlDatabase db, QStringList stringList);
    static bool insertAlarmRecord(QSqlDatabase db, QStringList stringList);
    //
    static qreal getPngsZoom(QSqlDatabase db, QString host, QString loop);
    static bool setPngsZoom(QSqlDatabase db, QString loop, QString level, QString host);
    static bool insertPngsZoom(QSqlDatabase db, QString host);

    static qreal getViewZoom(QSqlDatabase db, QString host, QString loop);
    static bool setViewZoom(QSqlDatabase db, QString loop, QString level, QString host);
    static bool insertViewZoom(QSqlDatabase db, QString host);


private:
    //互斥锁
    static QMutex mutex;
    SqlManager();
    static QWaitCondition waitConnection;
    static SqlManager *instance;
    static SqlManager &getInstance();

    // 创建数据库连接
    QSqlDatabase createConnection(const QString &connectionName);
    // 已使用的数据库连接名
    QQueue<QString> usedConntNames;
    // 未使用的数据库连接名
    QQueue<QString> unusedConntNames;
    // 数据库信息 ./S2N.db
    QString dataBaseName;
    // 数据库类型 QSQLITE
    QString dataBaseType;
    // 取得连接的时候验证连接是否有效
    bool testOnBorrow;
    // 测试访问数据库的 SQL
    QString testOnBorrowSql;
    // 获取连接最大等待时间
    int maxWaitTime;
    // 尝试获取连接时等待间隔时间
    int waitInterval;
    // 最大连接数
    int maxConntCount;
};

#endif // SQLMANAGER_H
