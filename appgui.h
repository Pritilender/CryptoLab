#ifndef APPGUI_H
#define APPGUI_H

#include <QGraphicsView>
#include <QWidget>
#include <cryptodirmanip.h>

#define REG_AX 20
#define REG_XY 0
#define REG_YY 40
#define REG_ZY 40

enum regs {X, Y, Z};

namespace Ui {
class appGui;
}

class appGui : public QWidget
{
    Q_OBJECT

private:
    bool canRun();
    QGraphicsScene scene;
    void drawRegister(/*QGraphicsScene& scene,*/ regs reg, const QString& bits);

public:
    explicit appGui(QWidget *parent = 0);
    ~appGui();

private slots:
    void on_btnEncrypt_clicked();
    void on_btnKeyLoad_clicked();
    void on_lneKey_textChanged(const QString &arg1);
    void on_btnInputDir_clicked();
    void on_btnOutputDir_clicked();
    void on_btnDecrypt_clicked();
    void on_cbxWatch_clicked(bool checked);
    void inDirFile(const QString &inDir);
    void outDirFile(const QString &output);
    void keyFile(const QString &key);
    void encryptionFile(const bool encry);
    void runningFile(const bool running);
    void watchFile(const bool watch);


signals:
    void inDirLoad(const QString);
    void outDirLoad(const QString);
    void keyLoad(const QString);
    void watchModeChanged(const bool);
    void startAlgo(const bool);

private:
    Ui::appGui *ui;
    QString loadDirFile(bool dir);
};

#endif // APPGUI_H
