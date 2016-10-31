#ifndef CRYPTODIRMANIP_H
#define CRYPTODIRMANIP_H

#include <QString>
#include <cryptoalgorithm.h>
#include <QList>
#include <QFileInfo>
#include <QObject>
#include <QtConcurrent>
#include <QFuture>
#include <QMutex>
#include <QDateTime>

/**
 * @brief The CryptoDirManip class
 * It expects that eveything is valid.
 */
class CryptoDirManip: public QObject
{
    Q_OBJECT
private:
    QString inputDir;
    QString outputDir;
    CryptoAlgorithm *algoRunner;
    bool watchMode = false;
    bool encryption = false;
    bool running = false;
    bool closeApp = false;
    int oldLength = 0;
    QList<QFuture<void>> mapThread;
    QFuture<void> queueThread;
    QFuture<void> conc;
    QMutex mutex;
    QMutex fileMutex;
    //inotify
    int fd;
    int wd;
    QString configPath = "config.file";

    QList<QString> fileNames;

    void inotifyThread();
    /**
     * @brief Queue manipulation function
     * @details This slot is used for adding, deleting and renaming paths
     * of files stored in the input dir. It checks what had provoked the signal.
     * @param path Path to be added to a queue
     */
    void queueManip();
    void fileToAlgo(const QString current);
    void writeConfig();
    void readConfig();

public:
    CryptoDirManip();
    void loadConfigFile();
    virtual ~CryptoDirManip();

public slots:
    /**
     * @brief Load input directory
     * @param input Path to a given directory
     */
    void loadInputDir(const QString &input);
    /**
     * @brief Load output directory
     * @param input Path to a given directory
     */
    void loadOutputDir(const QString &output);
    /**
     * @brief Load key to be used in the algorithm
     * @param key Key in string form
     */
    void loadKey(const QString &key);
    /**
     * @brief Set if the class should be watching input directory
     * @param mode True for watch, false for no-watch
     */
    void setWatchMode(const bool mode);
    /**
     * @brief Do the magic!
     * @param encryption Set algorithm to do the encryption
     */
    void run(const bool encryption);
    //stop();
signals:
    void inDirFile(const QString &input);
    void outDirFile(const QString &output);
    void keyFile(const QString &key);
    void encryptionFile(const bool encry);
    void runningFile(const bool running);
    void watchFile(const bool watch);
};

#endif // CRYPTODIRMANIP_H
