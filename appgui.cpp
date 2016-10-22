#include "appgui.h"
#include "ui_appgui.h"
#include "simplesubstitutioner.h"
#include "QVector"
#include "QMessageBox"
#include "keyvalidator.h"

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
    QString key = "ijklmnopqristuvwxyzabcdefgh";
    //SimpleSubstitutioner substitutioner(key);
    //QString res = substitutioner.substitute("abcd");
    KeyValidator kvalid;
    bool res = kvalid.validate(key);
    QMessageBox msg;
    msg.setText(res ? "true" : "false");
    msg.exec();
}
