#include "userlogin.h"
#include "ui_userlogin.h"
#include "MsgBox/msgbox.h"
UserLogin::UserLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint);
    hide();
    initConnect();
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::confShow()
{
    show();
    ui->lineEditPwd->clear();
}

void UserLogin::initConnect()
{
    connect(ui->tBtnLogin,SIGNAL(clicked(bool)),this,SLOT(slotBtnLogin()));
    connect(ui->tBtnHide,SIGNAL(clicked(bool)),this,SLOT(hide()));
}

void UserLogin::slotBtnLogin()
{
    if (ui->lineEditPwd->text() == QString(tr("111111"))) {
        emit sigUserLoginOk();
        hide();
    } else {
        MsgBox::showInformation(this,tr(" 系统提示"),tr("密码错误"),tr("关闭"));
    }

}

