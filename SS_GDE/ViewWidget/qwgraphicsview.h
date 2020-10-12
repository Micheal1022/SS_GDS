#ifndef QWGRAPHICSVIEW_H
#define QWGRAPHICSVIEW_H

#include <QPoint>
#include <QEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QGraphicsView>
class QWGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    QWGraphicsView(QWidget *widget);
signals:
    void keyPress(QPoint point);
    void mouseClicked(QPoint point);
    void mouseMovePiont(QPoint point);
    void mouseDoubleClick(QPoint point);
protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
private:

};

#endif // QWGRAPHICSVIEW_H
