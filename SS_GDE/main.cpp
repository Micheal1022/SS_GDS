#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (QFile::exists("SS_GDS_zh_CN.qm")){
        QTranslator *translator = new QTranslator;
        QString load = "SS_GDS_zh_CN.qm";
        translator->load(load);
        a.installTranslator(translator);
    }
    MainWindow w;
    w.show();






    return a.exec();
}
