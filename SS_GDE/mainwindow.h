#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QMenu>
#include <QDebug>
#include "MsgBox/msgbox.h"
#include "ViewWidget/viewwidget.h"
#include "RecordInfo/recordinfo.h"
#include "SystemConf/systemconf.h"
#include "UserLgoin/userlogin.h"
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

    ViewWidget *m_viewWidget;
    RecordInfo *m_recordInfo;
    SystemConf *m_systemConf;
    UserLogin  *m_userLogin;
    QTimer *m_systemTimer;

    void systemTime();
    void initVariable();
    void initConnect();

private:
    void initWidegt();
private slots:
    void slotViewWidget();
    void slotSystemConf();
    void slotRecordInfo();
    void slotUserLogin();
    void slotAppQuit();
    void slotSystemTime();

    //QMenu *createSystemMenu();
    //void slotSystemMenu();
};

#endif // MAINWINDOW_H
