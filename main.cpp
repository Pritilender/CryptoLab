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
    CryptoConfig config("/home/miksa/QtProjects/CryptographyLab/testDir/simpleSub.config");

    appGui w(&queue);
    QObject::connect(&w, SIGNAL(inDirLoad(QString)), &watcher, SLOT(setInputDir(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &watcher, SLOT(setOutputDir(QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &watcher, SLOT(watchModeChange(bool)));

    QObject::connect(&w, SIGNAL(keyLoad(QString)), &dispatcher, SLOT(setKey(QString)));
    QObject::connect(&w, SIGNAL(iVLoad(QString)), &dispatcher, SLOT(setIV(QString)));
    QObject::connect(&w, SIGNAL(startAlgo(const bool)), &dispatcher, SLOT(run(const bool)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &dispatcher, SLOT(setOutDir(QString)));

    QObject::connect(&watcher, SIGNAL(newFile(const QString&)), &queue, SLOT(insertFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(modifiedFile(const QString&)), &queue, SLOT(updateFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(removedFile(const QString&)), &queue, SLOT(removeFile(const QString&)));

    QObject::connect(&w, SIGNAL(inDirLoad(QString)), &config, SLOT(setInDir(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &config, SLOT(setOutDir(QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &config, SLOT(setWatchMode(bool)));
    QObject::connect(&w, SIGNAL(keyLoad(QString)), &config, SLOT(setKey(QString)));
    QObject::connect(&dispatcher, SIGNAL(writeConfig()), &config, SLOT(write()));
    QObject::connect(&config, SIGNAL(inDirLoad(QString)), &w, SLOT(inDirFile(QString)));
    QObject::connect(&config, SIGNAL(outDirLoad(QString)), &w, SLOT(outDirFile(QString)));
    QObject::connect(&config, SIGNAL(keyLoad(QString)), &w, SLOT(keyFile(QString)));
    QObject::connect(&config, SIGNAL(watchModeLoad(bool)), &w, SLOT(watchFile(bool)));
//    QObject::connect(&config, SIGNAL(lastTime(uint)), &queue, SLOT(filterTime(uint)));
    QObject::connect(&config, SIGNAL(encryptionLoad(bool)), &dispatcher, SLOT(run(bool)));
    QObject::connect(&config, SIGNAL(xModeLoad(bool)), &dispatcher, SLOT(setXMode(bool)));

    watcher.start();
    dispatcher.start();
    config.read();

    w.show();

    return a.exec();
}
