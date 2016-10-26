#ifndef APPGUI_H
#define APPGUI_H

#include <QWidget>
#include <cryptodirmanip.h>

namespace Ui {
class appGui;
}

class appGui : public QWidget
{
    Q_OBJECT

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
