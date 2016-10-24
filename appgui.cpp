#include "appgui.h"
#include "ui_appgui.h"
#include "simplesubstitutioner.h"
#include "QVector"
#include "QMessageBox"
#include "keyvalidator.h"
#include "QFileDialog"
#include "QFile"
#include "QTextStream"
#include "QDirIterator"

#include "QDebug"

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
    ///TODO
    bool autowatch = ui->cbxWatch->isChecked();

    if (autowatch) {
        //autowatcher
    } else {
        //just read dirs and encrypt files
        //This should read only txt or only crypto, depending on encrypt or decrypt button pressed
        QDirIterator inputDir(ui->lneInputDir->text(), QStringList() << "*.txt", QDir::Files);
        const QString outputDir = ui->lneOutputDir->text();

        qDebug() << inputDir.path() << " i out: " << outputDir;

        SimpleSubstitutioner substitutioner(ui->lneKey->text());

        while (inputDir.hasNext()) {
            QFile inFile(inputDir.next());
            //This should switch extension depending on encryption and decryption
            QFile outFile(outputDir + "/" + inputDir.fileInfo().baseName() + ".crypto");
            if (inFile.open(QIODevice::ReadOnly | QIODevice::Text) && outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream inReader(&inFile);
                QTextStream fileOut(&outFile);
                QString inputTxt;

                while (!inReader.atEnd()) {
                    inputTxt += inReader.readLine();
                }

                inputTxt.remove(QChar(' '), Qt::CaseInsensitive);
                inputTxt.remove(QChar('\n'), Qt::CaseInsensitive);

                qDebug() << inputTxt;

                inputTxt = inputTxt.toLower();

                fileOut << substitutioner.substitute(inputTxt, false);

                qDebug() << "kraj za " << inputDir.fileInfo().baseName();
            }
        }
    }
}

void appGui::on_btnKeyLoad_clicked()
{
    QString fname = this->loadDirFile(false);
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

void appGui::on_btnInputDir_clicked()
{
    const QString inputDir = this->loadDirFile(true);

    ui->lneInputDir->setText(inputDir);
}


void appGui::on_btnOutputDir_clicked()
{
    const QString inputDir = this->loadDirFile(true);

    ui->lneOutputDir->setText(inputDir);
}

QString appGui::loadDirFile(bool dir)
{
    QString dirFileName;

    if (dir) {
        dirFileName = QFileDialog::getExistingDirectory(this, "Odaberite folder", "~/");
    } else {
        dirFileName = QFileDialog::getOpenFileName(this, "Odaberite fajl", "~/", "*.txt");
    }

    return dirFileName;
}

void appGui::on_btnDecrypt_clicked()
{

}
