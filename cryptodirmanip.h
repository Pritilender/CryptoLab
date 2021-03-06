#ifndef CRYPTODIRMANIP_H
#define CRYPTODIRMANIP_H

#include "a51.h"

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
    QString key;
    QString input;
    bool watchMode = false;
    bool encryption = false;
    bool running = false;
    bool closeApp = false;
    bool simulation = false;
    bool nextStep = false;
    int oldLength = 0;
    QList<QFuture<void>> mapThread;
    QFuture<void> queueThread;
    QFuture<void> conc;
    QMutex mutex;
    QMutex fileMutex;

    QString configPath = "config.file";
    int runningThreads = 0;

    void inotifyThread();
    /**
     * @brief Queue manipulation function
     * @details This slot is used for adding, deleting and renaming paths
     * of files stored in the input dir. It checks what had provoked the signal.
     * @param path Path to be added to a queue
     */
    void queueManip();
    void fileToAlgo(const QString current);
    void fTAStepByStep(A51 &alg, const QString src);
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

    void passXStepped(const QString &xreg)
    {
        emit this->changeRegX(xreg);
    }
    void passYStepped(const QString &yreg)
    {
        emit this->changeRegY(yreg);
    }
    void passZStepped(const QString &zreg)
    {
        emit this->changeRegZ(zreg);
    }
    void simulationMode(const bool sim)
    {
        this->simulation = sim;

        if (sim) {
            this->loadKey(this->key);
        }
    }
    void stepNext(void)
    {
        this->nextStep = true;
    }

signals:
    void inDirFile(const QString &input);
    void outDirFile(const QString &output);
    void keyFile(const QString &key);
    void encryptionFile(const bool encry);
    void runningFile(const bool running);
    void watchFile(const bool watch);
    void changeRegX(const QString &regX);
    void changeRegY(const QString &regY);
    void changeRegZ(const QString &regZ);
    void changeSrc(const QString &src);
    void changeDst(const QString &dst);
};

#endif // CRYPTODIRMANIP_H
