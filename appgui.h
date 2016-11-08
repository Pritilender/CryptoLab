#ifndef APPGUI_H
#define APPGUI_H

#include "cryptoqueue.h"

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsTextItem>

#define REG_BLOCK 14
#define REG_SPACE 20
#define FONT_REGULATOR 3
#define REG_START 20
#define REG_LEFT 30

enum regs {SRC, X, Y, Z, DST};

namespace Ui {
class appGui;
}

class appGui : public QWidget
{
    Q_OBJECT

private:
    bool canRun();

public:
    explicit appGui(CryptoQueue* q, QWidget *parent = 0);
    ~appGui();
    CryptoQueue* q;

private slots:
    void on_btnEncrypt_clicked();
    void on_btnKeyLoad_clicked();
    void on_btnInputDir_clicked();
    void on_btnOutputDir_clicked();
    void on_btnDecrypt_clicked();
    void on_cbxWatch_clicked(bool checked);
    void inDirFile(const QString &inDir);
    void outDirFile(const QString &output);
    void keyFile(const QString &key);
    void watchFile(const bool watch);
    void on_btnIVLoad_clicked();

signals:
    void inDirLoad(const QString);
    void outDirLoad(const QString);
    void keyLoad(const QString);
    void watchModeChanged(const bool);
    void startAlgo(const bool);
    void simulationChanged(const bool);
    void nextStep(void);
    void iVLoad(const QString);

private:
    Ui::appGui *ui;
    QString loadDirFile(bool dir);
};

#endif // APPGUI_H
