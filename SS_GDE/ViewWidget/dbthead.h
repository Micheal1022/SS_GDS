#ifndef DBTHEAD_H
#define DBTHEAD_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include "sensoriteminfo.h"
class DBThead : public QObject
{
    Q_OBJECT
public:
    explicit DBThead(QObject *parent = 0);

signals:

public slots:
    void slotNodeInfoZoom(QList<SensorItemInfo> itemInfoList, QList<QPair<qreal, qreal>> poxList, QStringList scaleList, QString path);
};

#endif // DBTHEAD_H
