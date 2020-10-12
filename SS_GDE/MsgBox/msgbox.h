#ifndef MSGBOX_H
#define MSGBOX_H

#include <QWidget>
#include <QDebug>
#include "infobox.h"
#include "questbox.h"

class  MsgBox
{

public:

    MsgBox();

    static int showQuestion(QWidget *parent, const QString &title,
                            const QString &text, const QString &btn1, const QString &btn2)
    {

        QuestBox questBox(parent, title, text, btn1, btn2);

        if (parent != NULL) {
            int wd = parent->width();
            int wt = parent->height();

            int dd = questBox.width();
            int dt = questBox.height();

            int x = parent->mapToGlobal(QPoint(0,0)).x();
            int y = parent->mapToGlobal(QPoint(0,0)).y();

            int move_x = x + (wd-dd)/2+3;
            int move_y = y + (wt-dt)/2+40;
            questBox.move(move_x,move_y);
        } else {
            questBox.move(295,200);
        }
        questBox.exec();
        return questBox.btnResult();
    }

    static void showInformation(QWidget *parent, const QString &title,const QString &text, const QString &btn)
    {
        InfoBox infoBox(parent, title, text, btn);

        if(parent != NULL)
        {
            int wd = parent->width();
            int wt = parent->height();

            int dd = infoBox.width();
            int dt = infoBox.height();

            int x = parent->mapToGlobal(QPoint(0,0)).x();
            int y = parent->mapToGlobal(QPoint(0,0)).y();

            int move_x = x + (wd-dd)/2+3;
            int move_y = y + (wt-dt)/2+20;
            infoBox.move(move_x,move_y);
        }
        else {
            infoBox.move(295,200);
        }
        infoBox.exec();
    }

};

#endif // MSGBOX_H
