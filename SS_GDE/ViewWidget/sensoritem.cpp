#include "sensoritem.h"

#include <QPainter>
#include <QCursor>
#include <QBrush>
#include <QDebug>
#include <QToolTip>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "includes.h"
#define SIZE_WIDTH  40
#define SIZE_HEIGHT SIZE_WIDTH
#define X_POS   SIZE_WIDTH/20*7
#define Y_POS   SIZE_WIDTH/20*9
#define WIDTH   SIZE_WIDTH/20*9
#define HEIGHT  SIZE_WIDTH/20*5




SensorItem::SensorItem(SensorItemInfo itemInfo, QObject *parent, QGraphicsItem *parent1)
    :QObject(parent),QGraphicsItem(parent1)
{
    m_state = 100;
    m_times = 0;
    m_oldState = 1;
    m_loopStr = itemInfo.m_loopStr;
    m_idStr   = itemInfo.m_idStr;
    m_typeStr = itemInfo.m_typeStr;
    m_areaStr = itemInfo.m_areaStr;
    setScale(itemInfo.m_zoom);
    setPos(itemInfo.m_posX,itemInfo.m_posY);
    setItemType(m_typeStr.toInt());
    setToolTipString();
    setCursor(Qt::PointingHandCursor);

    m_stateTimer = new QTimer;
    connect(m_stateTimer,SIGNAL(timeout()),this,SLOT(slotStateTimeOut()));
    m_stateTimer->start(500);



}

void SensorItem::setItemState(int state)
{
    m_oldState = state;
}

void SensorItem::setItemType(int type)
{
    if (type == MOD_L1T4) {
        m_typeStr = tr("L1T4");
    } else if (type == MOD_L12T4) {
        m_typeStr = tr("L12T4");
    } else if (type == MOD_2VA) {
        m_typeStr = tr("2VA");
    } else if (type == MOD_VA) {
        m_typeStr = tr("VA");
    } else if (type == MOD_A) {
        m_typeStr = tr("A");
    } else if (type == MOD_3V) {
        m_typeStr = tr("3V");
    } else if (type == MOD_2V) {
        m_typeStr = tr("2V");
    } else if (type == MOD_V) {
        m_typeStr = tr("V");
    } else if (type == MOD_6V3A) {
        m_typeStr = tr("6V3A");
    } else if (type == MOD_6V) {
        m_typeStr = tr("6V");
    } else if (type == MOD_3A) {
        m_typeStr = tr("3A");
    }
}

void SensorItem::setToolTipString()
{
    QString toolTipStr1;
    if (m_idStr.toInt() < 10) {
        toolTipStr1 = QString("编号:%1-00%2\n").arg(m_loopStr).arg(m_idStr);
    } else if (m_idStr.toInt() >= 10 && m_idStr.toInt() < 100) {
        toolTipStr1 = QString("编号:%1-0%2\n").arg(m_loopStr).arg(m_idStr);
    } else if (m_idStr.toInt() >= 100) {
        toolTipStr1 = QString("编号:%1-%2\n").arg(m_loopStr).arg(m_idStr);
    }
    QString toolTipStr2 = QString("类型:%1\n").arg(m_typeStr);
    QString toolTipStr3 = QString("区域:%1").arg(m_areaStr);
    QString pToolTipStr = toolTipStr1+toolTipStr2+toolTipStr3;
    setToolTip(pToolTipStr);

}

QRectF SensorItem::boundingRect() const
{
    return QRectF(0, 0, 50, 50);
}


//#define SOLID
void SensorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    //绘制图片
    QRect rect(5, 5, SIZE_WIDTH, SIZE_HEIGHT);
    QPixmap pixmap(":/Image/detector.png");
    painter->drawPixmap(rect,pixmap);
    //绘制矩形区域
    QBrush brush;
    if (m_state == N_NORMAL) {
        brush.setColor(Qt::green);
    } else if (m_state == N_ERROR) {
        brush.setColor(Qt::yellow);
    } else if (m_state == N_POWERLOST) {
        brush.setColor(Qt::red);
    } else if (m_state == N_OVERVOL) {
        brush.setColor(Qt::yellow);
    } else if (m_state == N_OVERCUR) {
        brush.setColor(Qt::yellow);
    } else if (m_state == N_LACKVOL) {
        brush.setColor(Qt::yellow);
    } else if (m_state == N_LACKPHA) {
        brush.setColor(Qt::yellow);
    } else if (m_state == N_ERRORPHA) {
        brush.setColor(Qt::yellow);
    } else if (m_state == N_ALARM) {
        brush.setColor(Qt::red);
    } else if (m_state == N_OFFLINE) {
        brush.setColor(Qt::gray);
    } else {
        brush.setColor(QColor(0xA0,0xA0,0xA4,0));
    }
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);
    QRect r(X_POS,Y_POS,WIDTH,HEIGHT);
    painter->drawRect(r);

    // 选中时绘制
    if (option->state & QStyle::State_Selected) {
        // 边框区域颜色
        QColor color = QColor(Qt::black);
#ifdef SOLID
        // 绘制实线
        painter->setPen(QPen(color, 2, Qt::SolidLine));
#else
        // 绘制虚线
        painter->setPen(QPen(color, 2, Qt::DashLine));
#endif
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}

void SensorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void SensorItem::slotStateTimeOut()
{
    m_times++;
    if (m_times == 1) {
        m_state = 100;
    } else if (m_times == 2) {
        m_state = m_oldState;
        m_times = 0;
    }
    update();
}
