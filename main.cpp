#include "appgui.h"
#include <QApplication>
#include "a51.h"
#include "cryptodirmanip.h"
#include "cryptoqueue.h"
#include "cryptowatcher.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    CryptoDirManip dirManip;
    CryptoQueue queue;
    CryptoWatcher watcher;
    //CryptoWorker worker;
    appGui w(&queue);
    QObject::connect(&w, SIGNAL(inDirLoad(const QString)), &watcher, SLOT(setInputDir(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &watcher, SLOT(setOutputDir(QString)));
//    QObject::connect(&w, SIGNAL(keyLoad(QString)), &dirManip, SLOT(loadKey(const QString)));
//    QObject::connect(&w, SIGNAL(startAlgo(bool)), &dirManip, SLOT(run(bool)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &watcher, SLOT(watchModeChange(bool)));
//    QObject::connect(&w, SIGNAL(simulationChanged(bool)), &dirManip, SLOT(simulationMode(bool)));
//    QObject::connect(&w, SIGNAL(nextStep()), &dirManip, SLOT(stepNext()));

    QObject::connect(&watcher, SIGNAL(newFile(const QString&)), &queue, SLOT(insertFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(modifiedFile(const QString&)), &queue, SLOT(updateFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(removedFile(const QString&)), &queue, SLOT(removeFile(const QString&)));

    watcher.start();
//    QObject::connect(&dirManip, SIGNAL(changeRegX(QString)), &w, SLOT(drawRegisterX(QString)));
//    QObject::connect(&dirManip, SIGNAL(changeRegY(QString)), &w, SLOT(drawRegisterY(QString)));
//    QObject::connect(&dirManip, SIGNAL(changeRegZ(QString)), &w, SLOT(drawRegisterZ(QString)));
//    QObject::connect(&dirManip, SIGNAL(inDirFile(const QString &)), &w, SLOT(inDirFile(const QString &)));
//    QObject::connect(&dirManip, SIGNAL(outDirFile(const QString &)), &w, SLOT(outDirFile(const QString &)));
//    QObject::connect(&dirManip, SIGNAL(keyFile(const QString &)), &w, SLOT(keyFile(const QString &)));
//    QObject::connect(&dirManip, SIGNAL(encryptionFile(const bool)), &w, SLOT(encryptionFile(bool)));
//    QObject::connect(&dirManip, SIGNAL(runningFile(const bool)), &w, SLOT(runningFile(bool)));
//    QObject::connect(&dirManip, SIGNAL(watchFile(const bool)), &w, SLOT(watchFile(bool)));
//    QObject::connect(&dirManip, SIGNAL(changeDst(QString)), &w, SLOT(drawDst(QString)));
//    QObject::connect(&dirManip, SIGNAL(changeSrc(QString)), &w, SLOT(drawSrc(QString)));

    //dirManip.loadConfigFile();
    w.show();

    return a.exec();
}
