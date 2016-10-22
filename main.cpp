#include "appgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    appGui w;
    w.show();

    return a.exec();
}
