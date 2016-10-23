#include "appgui.h"
#include "ui_appgui.h"
#include "simplesubstitutioner.h"
#include "QVector"
#include "QMessageBox"
#include "keyvalidator.h"
#include "QFileDialog"
#include "QFile"
#include "QTextStream"

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

void appGui::on_btnEncrypt_clicked()
{
    QString key = ui->lneKey->text();
    QString text = ui->lneText->text();

    SimpleSubstitutioner substitutioner(key);
    QString res = substitutioner.substitute(text);

    ui->lblRes->setText(res);
}

void appGui::on_btnKeyLoad_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this, "Odaberite fajl", "~/", "*.txt");
    QFile keyFile(fname);

    if (keyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream keyReader(&keyFile);
        QString key = keyReader.readAll();

        ui->lneKey->setText("");
        ui->lneKey->setText(key.trimmed());
        ui->lneKey->setFocus();

        keyFile.close();
    } else {
        QMessageBox msg;
        msg.setText("Fajl nije nadjen");
        msg.exec();
    }
}

void appGui::on_lneKey_textChanged(const QString &arg1)
{
    KeyValidator kvalid;
    QString key = arg1;

    if (!kvalid.validate(key)) {
        QMessageBox msg;
        msg.setText("Dati ključ nije validan!");
        msg.exec();
        ui->btnEncrypt->setDisabled(true);
    } else {
        ui->btnEncrypt->setEnabled(true);
    }
}
