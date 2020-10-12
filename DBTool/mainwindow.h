#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbsendwidget.h"
#include "dbeditwidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DBSendWidget *m_DBSendWidget;
    DBEditWidget *m_DBEditWidget;
    void initWidget();
    void initConnect();

private slots:
    void slotEditFile();
    void slotSendFile();
    void slotMainWidget();
};

#endif // MAINWINDOW_H
