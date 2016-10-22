#include "appgui.h"
#include "ui_appgui.h"
#include "simplesubstitutioner.h"
#include "QVector"
#include "QMessageBox"

appGui::appGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::appGui)
{
    ui->setupUi(this);
}

appGui::~appGui()
{
    delete ui;
}

void appGui::on_pushButton_clicked()
{
    QString key = "ijklmnopqrstuvwxyzabcdefgh";
    SimpleSubstitutioner substitutioner(key);
    QString res = substitutioner.substitute("abcd");
    QMessageBox msg;
    msg.setText(res);
    msg.exec();
}
