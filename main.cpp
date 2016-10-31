#include "appgui.h"
#include <QApplication>
#include "cryptodirmanip.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CryptoDirManip dirManip;
    appGui w;

    QObject::connect(&w, SIGNAL(inDirLoad(const QString)), &dirManip, SLOT(loadInputDir(const QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &dirManip, SLOT(loadOutputDir(const QString)));
    QObject::connect(&w, SIGNAL(keyLoad(QString)), &dirManip, SLOT(loadKey(const QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &dirManip, SLOT(setWatchMode(bool)));
    QObject::connect(&w, SIGNAL(startAlgo(bool)), &dirManip, SLOT(run(bool)));

    QObject::connect(&dirManip, SIGNAL(inDirFile(const QString &)), &w, SLOT(inDirFile(const QString &)));
    QObject::connect(&dirManip, SIGNAL(outDirFile(const QString &)), &w, SLOT(outDirFile(const QString &)));
    QObject::connect(&dirManip, SIGNAL(keyFile(const QString &)), &w, SLOT(keyFile(const QString &)));
    QObject::connect(&dirManip, SIGNAL(encryptionFile(const bool)), &w, SLOT(encryptionFile(bool)));
    QObject::connect(&dirManip, SIGNAL(runningFile(const bool)), &w, SLOT(runningFile(bool)));
    QObject::connect(&dirManip, SIGNAL(watchFile(const bool)), &w, SLOT(watchFile(bool)));

    dirManip.loadConfigFile();
    w.show();

    return a.exec();
}
