#include "cryptowatcher.h"

#include <QDebug>
#include <QDirIterator>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUF_LEN (1024*(EVENT_SIZE + 16))

int CryptoWatcher::deduceEvent(struct inotify_event *event)
{
    int retVal = 0;

    if (event->len && !(event->mask & IN_ISDIR)) {
        if (event->mask & IN_CREATE) {
            // New file, just add it to the queue
            qDebug() << "The file" << event->name << "was created.";
            retVal = 1;
        } else if (event->mask & IN_MODIFY) {
            // File is modified and algo is running, so we need to readd it
            qDebug() << "The file" << event->name << "was modified.";
            retVal = 2;
        } else if (event->mask & IN_DELETE) {
            // If algo is not running and we need to remove the file from queue
            qDebug() << "The file" << event->name << "was deleted.";
            retVal = 3;
        }
    }

    return retVal;
}

void CryptoWatcher::dispatchSignal(struct inotify_event *event)
{
    int deduceVal = this->deduceEvent(event);

    QString path = this->inputDirPath + '/' + event->name;
    switch (deduceVal) {
    case 1:
        emit this->newFile(path);
        break;
    case 2:
        emit this->modifiedFile(path);
        break;
    case 3:
        emit this->removedFile(path);
        break;
    }
}

QList<QString> CryptoWatcher::getAllFromFolder(const QString &folderName)
{
    QDirIterator folder(folderName, QDir::Files);
    QList<QString> absolutPaths;

    while (folder.hasNext()) {
        QFileInfo next(folder.next());
        absolutPaths.append(next.absoluteFilePath());
    }

    return absolutPaths;
}

CryptoWatcher::CryptoWatcher(QObject *parent)
{
    this->setParent(parent);

    this->fd = inotify_init();

    if (this->fd < 0) {
        qDebug() << "Error for inotify";
        exit(0);
    }
}

CryptoWatcher::~CryptoWatcher()
{
    this->appRunning = false;
    close(this->fd);
}

void CryptoWatcher::run()
{
    struct pollfd pfd = {this->fd, POLLIN, 0};

    while (this->appRunning) {
        while (this->watchMode) {
            char buffer[BUF_LEN];
            int i = 0;
            int ret = poll(&pfd, 1, 500);

            if (ret < 0) {
                qDebug() << "Poll fail";
                return;
            } else if (ret == 0) {
                continue; // no new events
            } else {
                int length = read(this->fd, buffer, BUF_LEN);

                if (length < 0) {
                    qDebug() << "Read fail";
                    return;
                }

                while (i < length) {
                    struct inotify_event *event = (struct inotify_event *) &buffer[i];
                    this->dispatchSignal(event);
                    i += EVENT_SIZE + event->len;
                }
            }
        }
    }
}

void CryptoWatcher::setInputDir(const QString &inDir)
{
    this->inputDirPath = inDir;

    QList<QString> inFiles(this->getAllFromFolder(this->inputDirPath));

    for(int i = 0; i < inFiles.length(); i++) {
        emit this->newFile(inFiles[i]);
    }
}

void CryptoWatcher::setOutputDir(const QString &outDir)
{
    this->outputDirPath = outDir;
}

QString CryptoWatcher::getInputDir()
{
    return this->inputDirPath;
}

QString CryptoWatcher::getOutputDir()
{
    return this->outputDirPath;
}

void CryptoWatcher::watchModeChange(bool watchMode)
{
    ///TODO autoWatch must be disabled while no input dir is made
    this->watchMode = watchMode;

    if (this->watchMode) {
        this->wd = inotify_add_watch(this->fd, this->inputDirPath.toLatin1(),
                                     IN_MODIFY | IN_CREATE | IN_DELETE);
    } else {
        inotify_rm_watch(this->fd, this->wd);
    }
}
