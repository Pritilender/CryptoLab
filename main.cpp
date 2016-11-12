#include "appgui.h"
#include <QApplication>
#include "cryptoconfig.h"
#include "cryptodispatcher.h"
#include "cryptoqueue.h"
#include "cryptowatcher.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CryptoQueue queue;
    CryptoWatcher watcher;
    CryptoDispatcher dispatcher(&queue);
    CryptoConfig config("/home/miksa/QtProjects/CryptographyLab/testDir/knapsack.config");

    appGui w(&queue);
    QObject::connect(&w, SIGNAL(inDirLoad(QString)), &watcher, SLOT(setInputDir(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &watcher, SLOT(setOutputDir(QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &watcher, SLOT(watchModeChange(bool)));

    QObject::connect(&w, SIGNAL(arrayLoad(QString)), &dispatcher, SLOT(setArray(QString)));
    QObject::connect(&w, SIGNAL(mLoad(QString)), &dispatcher, SLOT(setM(QString)));
    QObject::connect(&w, SIGNAL(nLoad(QString)), &dispatcher, SLOT(setN(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &dispatcher, SLOT(setOutDir(QString)));
    QObject::connect(&w, SIGNAL(startAlgo(const bool)), &dispatcher, SLOT(runAlgo(const bool)));

    QObject::connect(&watcher, SIGNAL(newFile(const QString&)), &queue, SLOT(insertFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(modifiedFile(const QString&)), &queue, SLOT(updateFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(removedFile(const QString&)), &queue, SLOT(removeFile(const QString&)));

    QObject::connect(&w, SIGNAL(inDirLoad(QString)), &config, SLOT(setInDir(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &config, SLOT(setOutDir(QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &config, SLOT(setWatchMode(bool)));
    QObject::connect(&w, SIGNAL(arrayLoad(QString)), &config, SLOT(setArray(QString)));
    QObject::connect(&w, SIGNAL(nLoad(QString)), &config, SLOT(setN(QString)));
    QObject::connect(&w, SIGNAL(mLoad(QString)), &config, SLOT(setM(QString)));
    QObject::connect(&dispatcher, SIGNAL(writeConfig()), &config, SLOT(write()));
    QObject::connect(&config, SIGNAL(inDirLoad(QString)), &w, SLOT(inDirFile(QString)));
    QObject::connect(&config, SIGNAL(outDirLoad(QString)), &w, SLOT(outDirFile(QString)));
    QObject::connect(&config, SIGNAL(arrayLoad(QString)), &w, SLOT(arrayFile(QString)));
    QObject::connect(&config, SIGNAL(nLoad(QString)), &w, SLOT(nFile(QString)));
    QObject::connect(&config, SIGNAL(mLoad(QString)), &w, SLOT(mFile(QString)));
    QObject::connect(&config, SIGNAL(watchModeLoad(bool)), &w, SLOT(watchFile(bool)));
//    QObject::connect(&config, SIGNAL(lastTime(uint)), &queue, SLOT(filterTime(uint)));
    //QObject::connect(&config, SIGNAL(encryptionLoad(bool)), &dispatcher, SLOT(runAlgo(bool)));

    watcher.start();
    dispatcher.start();
    config.read();

    w.show();

    return a.exec();
}
