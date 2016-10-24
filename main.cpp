#include "appgui.h"
#include <QApplication>
#include "cryptodirmanip.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CryptoDirManip dirManip;
    appGui w;
    w.show();

    QObject::connect(&w, SIGNAL(inDirLoad(const QString)), &dirManip,
                     SLOT(loadInputDir(const QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &dirManip,
                     SLOT(loadOutputDir(const QString)));
    QObject::connect(&w, SIGNAL(keyLoad(QString)), &dirManip,
                     SLOT(loadKey(const QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &dirManip,
                     SLOT(setWatchMode(bool)));
    QObject::connect(&w, SIGNAL(startAlgo(bool)), &dirManip,
                     SLOT(run(bool)));

    return a.exec();
}
