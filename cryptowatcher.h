#ifndef CRYPTOWATCHER_H
#define CRYPTOWATCHER_H

#include <QThread>
#include <sys/inotify.h>
#include <sys/types.h>
#include <poll.h>
#include <unistd.h>

class CryptoWatcher: public QThread
{
    Q_OBJECT

private:
    QString inputDirPath;
    QString outputDirPath;

    bool watchMode = false;
    bool appRunning = true;

    int fd;
    int wd;

    /**
     * @brief Deduce what kind of event inotify has spot
     * @param event
     * @return 0 - directory, 1 - new file, 2 - modified, 3 - removed
     */
    int deduceEvent(struct inotify_event *event);

    /**
     * @brief Dispatch signal according to the recieved event
     * @param event
     */
    void dispatchSignal(inotify_event *event);

    /**
     * @brief Read given folder and return all files from it
     * @param folderName Name of the folder
     * @return Absolut path of files in the folder
     */
    QList<QString> getAllFromFolder(const QString &folderName);

public:
    CryptoWatcher(QObject *parent = 0);
    ~CryptoWatcher();
    void run() Q_DECL_OVERRIDE;
    QString getInputDir();
    QString getOutputDir();

public slots:
    void watchModeChange(bool watchMode);
    void setInputDir(const QString &inDir);
    void setOutputDir(const QString &outDir);

signals:
    void newFile(const QString &);
    void modifiedFile(const QString &);
    void removedFile(const QString &);
};

#endif // CRYPTOWATCHER_H
