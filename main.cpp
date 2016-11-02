#include "appgui.h"
#include <QApplication>
#include "a51.h"
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
    QObject::connect(&w, SIGNAL(simulationChanged(bool)), &dirManip, SLOT(simulationMode(bool)));
    QObject::connect(&w, SIGNAL(nextStep()), &dirManip, SLOT(stepNext()));

    QObject::connect(&dirManip, SIGNAL(changeRegX(QString)), &w, SLOT(drawRegisterX(QString)));
    QObject::connect(&dirManip, SIGNAL(changeRegY(QString)), &w, SLOT(drawRegisterY(QString)));
    QObject::connect(&dirManip, SIGNAL(changeRegZ(QString)), &w, SLOT(drawRegisterZ(QString)));
    QObject::connect(&dirManip, SIGNAL(inDirFile(const QString &)), &w, SLOT(inDirFile(const QString &)));
    QObject::connect(&dirManip, SIGNAL(outDirFile(const QString &)), &w, SLOT(outDirFile(const QString &)));
    QObject::connect(&dirManip, SIGNAL(keyFile(const QString &)), &w, SLOT(keyFile(const QString &)));
    QObject::connect(&dirManip, SIGNAL(encryptionFile(const bool)), &w, SLOT(encryptionFile(bool)));
    QObject::connect(&dirManip, SIGNAL(runningFile(const bool)), &w, SLOT(runningFile(bool)));
    QObject::connect(&dirManip, SIGNAL(watchFile(const bool)), &w, SLOT(watchFile(bool)));
    QObject::connect(&dirManip, SIGNAL(changeDst(QString)), &w, SLOT(drawDst(QString)));
    QObject::connect(&dirManip, SIGNAL(changeSrc(QString)), &w, SLOT(drawSrc(QString)));

    dirManip.loadConfigFile();
    w.show();

    return a.exec();
}
