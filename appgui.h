#ifndef APPGUI_H
#define APPGUI_H

#include <QGraphicsView>
#include <QWidget>
#include <cryptodirmanip.h>
#include <QGraphicsTextItem>

#define REG_BLOCK 14
#define REG_SPACE 20
#define FONT_REGULATOR 3
#define REG_START 20
#define REG_LEFT 30

enum regs {SRC, X, Y, Z, DEST};

namespace Ui {
class appGui;
}

class appGui : public QWidget
{
    Q_OBJECT

private:
    bool canRun();
    QGraphicsScene scene;
    void drawRegister(regs reg, const QString &bits, const QString &regName);
    QList<QList<QGraphicsTextItem*>> registerMatrix;
    inline double calculateX(int index)
    {
        return index * REG_BLOCK + REG_LEFT;
    }

    inline double calulateYFont(regs reg)
    {
        return (reg * (REG_BLOCK + REG_SPACE) - FONT_REGULATOR) + REG_START;
    }

    inline double calculateYReg(regs reg)
    {
        return reg * (REG_BLOCK + REG_SPACE) + REG_START;
    }

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
