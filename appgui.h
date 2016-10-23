#ifndef APPGUI_H
#define APPGUI_H

#include <QWidget>

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

private:
    Ui::appGui *ui;
    QString loadDirFile(bool dir);
};

#endif // APPGUI_H
