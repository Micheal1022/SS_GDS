#ifndef USERLGOIN_H
#define USERLGOIN_H

#include <QDialog>

namespace Ui {
class UserLogin;
}

class UserLogin : public QDialog
{
    Q_OBJECT

public:
    explicit UserLogin(QWidget *parent = 0);
    ~UserLogin();

    void confShow();
    void initConnect();
private:
    Ui::UserLogin *ui;

signals:
    void sigUserLoginOk();

private slots:
    void slotBtnLogin();
};

#endif // USERLGOIN_H
