#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QTimer>
//#include "qwgraphicsview.h"
#include <QTableWidget>
#include "sensoriteminfo.h"
#include "udpthread.h"
#include "sensoritem.h"
#include "SqlManager/sqlmanager.h"
namespace Ui {
class GraphicsView;
}

class GraphicsView : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = 0);
    ~GraphicsView();
    void confView(QList<SensorItemInfo> itemInfoList, QString loop, QString hostName, QString hostIP, QString port, QString &backGroundPath, QString dbPath);

private:
    Ui::GraphicsView *ui;
    QGraphicsScene *m_scene;
    void initWidget();
    void initVariable();
    void initConnect();
    void initTableWidget(QTableWidget *tableWidget);
    bool m_editModeFlag;
    qreal m_viewScale;
    QString m_dbPaht;
    QString m_loop;
    QString m_hostIP;
    QString m_hostName;
    QTimer *m_infoTimer;
    QList<QStringList> m_errorInfoList;//故障列表
    QList<QStringList> m_alarmInfoList;//报警列表
    QList<QThread *> m_threadList;
    QList<SensorItemInfo> m_itemInfoList;
    QList<QGraphicsItem *> m_selectItemList;

    void analysisData(QByteArray hostData);
    void setItem(QGraphicsScene *scene, QString loopStr, QString idStr, QString typeStr,QString stateStr);
    void setNodeInfoZoom(QString loop, QString id, QPair<qreal, qreal> pox, QString scale, QString path);
    void showInfoList(QTableWidget *tableWidget, QList<QStringList> infoList);
    void delStringList(QList<QStringList> &infoList, int loop, int Id, int state);
    int  findItemIndex(QList<SensorItemInfo> itemInfoList, QString loop,QString id);
signals:
    void sigNodeInfoZoom(QString loop, QString id, QPair<qreal, qreal> pox, QString scale, QString path);
    void sigNodeInfoZoom(QList<SensorItemInfo> itemInfoList, QList<QPair<qreal, qreal>> poxList, QStringList scale, QString path);

private slots:

    void slotBtnZoomIn();
    void slotBtnZoomOut();
    void slotBtnRestore();
    void slotBtnEdit();
    void slotBtnSave();
    void slotHostData(QByteArray hostData);

    void slotInfoTimeOut();
    void slotFindItemErrorPos(int row, int column);
    void slotFindItemAlarmPos(int row, int column);

    void slotAlignTop();
    void slotAlignLeft();
    void slotAlignRight();
    void slotAlignBottom();
};

#endif // GRAPHICSVIEW_H
