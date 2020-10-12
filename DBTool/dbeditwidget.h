#ifndef DBEDITWIDGET_H
#define DBEDITWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
namespace Ui {
class DBEditWidget;
}

class DBEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DBEditWidget(QWidget *parent = 0);
    ~DBEditWidget();

private:
    Ui::DBEditWidget *ui;
    void initWidget();
    void initConnect();
    QList<QStringList> confTableInfo(QTableWidget *tableWidget);
    bool saveDbInfo(QList<QStringList> dbInfoList, QString path);
    void initTableWidget(QTableWidget *tableWidget);
signals:
    void sigBack();
private slots:
    void slotOpenExcelFile();
    void slotSaveDbFile();
    void slotCellCheckClick(int row,int column);
};

#endif // DBEDITWIDGET_H
