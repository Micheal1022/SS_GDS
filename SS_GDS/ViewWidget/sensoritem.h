#ifndef SENSORITEM_H
#define SENSORITEM_H

#include <QGraphicsItem>
#include <QMouseEvent>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include "sensoriteminfo.h"


class SensorItem :public QObject , public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:

    SensorItem(SensorItemInfo itemInfo, QObject *parent = 0, QGraphicsItem *parent1 = nullptr);
    void setItemState(int state);
    void setItemType(int type);
    QTimer *m_stateTimer;
    int m_state;
    int m_oldState;
    int m_times;

    QString m_idStr;    //探测器地址
    QString m_loopStr;  //探测器回路
    QString m_typeStr;  //探测器类型
    QString m_areaStr;  //探测器位置

    void setToolTipString();


protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void slotStateTimeOut();
};

#endif // SENSORITEM_H
