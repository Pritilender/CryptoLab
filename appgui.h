#ifndef APPGUI_H
#define APPGUI_H

#include "cryptoqueue.h"

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsTextItem>
#include <QLineEdit>

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
    void loadFile(QLineEdit* lne);
    void emitEncryption(bool enc);

public:
    explicit appGui(CryptoQueue *q, QWidget *parent = 0);
    ~appGui();
    CryptoQueue *q;
    QPixmap beforeBMP, afterBMP;
    QGraphicsScene beforeScene, afterScene;

private slots:
    void on_btnEncrypt_clicked();
    void on_btnInputDir_clicked();
    void on_btnOutputDir_clicked();
    void on_btnDecrypt_clicked();
    void on_cbxWatch_clicked(bool checked);
    void inDirFile(const QString &inDir);
    void outDirFile(const QString &output);
    void arrayFile(const QString &array);
    void watchFile(const bool watch);
    void on_btnArrayLoad_clicked();
    void on_btnM_clicked();
    void on_btnN_clicked();
    void nFile(const QString &n);
    void mFile(const QString &m);

public slots:

signals:
    void inDirLoad(const QString);
    void outDirLoad(const QString);
    void arrayLoad(const QString);
    void mLoad(const QString);
    void nLoad(const QString);
    void watchModeChanged(const bool);
    void startAlgo(const bool);

private:
    Ui::appGui *ui;
    QString loadDirFile(bool dir);
};

#endif // APPGUI_H
