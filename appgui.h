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
  void on_pushButton_clicked();

  void on_btnEncrypt_clicked();

private:
    Ui::appGui *ui;
};

#endif // APPGUI_H
