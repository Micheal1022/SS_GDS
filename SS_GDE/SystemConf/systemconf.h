#ifndef SYSTEMCONF_H
#define SYSTEMCONF_H

#include <QWidget>
#include <QTableWidget>
namespace Ui {
class SystemConf;
}

class SystemConf : public QWidget
{
    Q_OBJECT

public:
    explicit SystemConf(QWidget *parent = 0);
    ~SystemConf();

private:
    Ui::SystemConf *ui;
signals:
    void sigSaveOk();
    void sigViewWidget();
private:
    void delAllConf();
    void initWidget();
    void initConnect();
    void initVariable();
    void confHostList(QTableWidget *tableWidget);
    void initTableWidget(QTableWidget *tableWidget);
private slots:

    void slotBtnAddHost();
    void slotBtnDelHost();
    void slotBtnSaveHost();
    void slotCellCheckClick(int row,int column);
};

#endif // SYSTEMCONF_H
