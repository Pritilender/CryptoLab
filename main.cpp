#include "appgui.h"
#include <QApplication>
#include "cryptodispatcher.h"
#include "cryptoqueue.h"
#include "cryptowatcher.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CryptoQueue queue;
    CryptoWatcher watcher;
    CryptoDispatcher dispatcher(&queue);

    appGui w(&queue);
    QObject::connect(&w, SIGNAL(inDirLoad(QString)), &watcher, SLOT(setInputDir(QString)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &watcher, SLOT(setOutputDir(QString)));
    QObject::connect(&w, SIGNAL(watchModeChanged(bool)), &watcher, SLOT(watchModeChange(bool)));

    QObject::connect(&w, SIGNAL(keyLoad(QString)), &dispatcher, SLOT(setKey(QString)));
    QObject::connect(&w, SIGNAL(startAlgo(const bool)), &dispatcher, SLOT(run(const bool)));
    QObject::connect(&w, SIGNAL(outDirLoad(QString)), &dispatcher, SLOT(setOutDir(QString)));

    QObject::connect(&watcher, SIGNAL(newFile(const QString&)), &queue, SLOT(insertFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(modifiedFile(const QString&)), &queue, SLOT(updateFile(const QString&)));
    QObject::connect(&watcher, SIGNAL(removedFile(const QString&)), &queue, SLOT(removeFile(const QString&)));

    watcher.start();
    dispatcher.start();

    //dirManip.loadConfigFile();
    w.show();

    return a.exec();
}
