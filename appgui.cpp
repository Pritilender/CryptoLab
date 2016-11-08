#include "appgui.h"
#include "ui_appgui.h"
#include "QVector"
#include "QMessageBox"
#include "keyvalidator.h"
#include "QFileDialog"
#include "QFile"
#include "QTextStream"
#include "QDirIterator"

#include "QDebug"

bool appGui::canRun()
{
    return !this->ui->lneKey->text().isEmpty() &&
           !this->ui->lneInputDir->text().isEmpty() &&
           !this->ui->lneOutputDir->text().isEmpty();
}

appGui::appGui(CryptoQueue *q = 0, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::appGui)
{
    ui->setupUi(this);

    this->q = q;
}

appGui::~appGui()
{
    delete ui;
}

void appGui::on_btnEncrypt_clicked()
{
    if (!this->ui->cbxWatch->isChecked()) {
        emit this->inDirLoad(this->ui->lneInputDir->text());
    }
    emit this->keyLoad(this->ui->lneKey->text());
    emit this->iVLoad(this->ui->lneIV->text());
    emit this->outDirLoad(this->ui->lneOutputDir->text());
    emit this->startAlgo(true);
}

void appGui::on_btnKeyLoad_clicked()
{
    QString fname = this->loadDirFile(false);
    QFile keyFile(fname);

    if (keyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream keyReader(&keyFile);
        QString key = keyReader.readAll().trimmed();

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

void appGui::on_btnInputDir_clicked()
{
    const QString inputDir = this->loadDirFile(true);

    ui->lneInputDir->setText(inputDir);
}


void appGui::on_btnOutputDir_clicked()
{
    const QString outputDir = this->loadDirFile(true);

    ui->lneOutputDir->setText(outputDir);
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
    emit this->inDirLoad(this->ui->lneInputDir->text());
    emit this->keyLoad(this->ui->lneKey->text());
    emit this->outDirLoad(this->ui->lneOutputDir->text());
    emit this->startAlgo(false);
}

void appGui::on_cbxWatch_clicked(bool checked)
{
    this->ui->cbxBMP->setEnabled(!checked);
    if (checked) {
        emit this->inDirLoad(this->ui->lneInputDir->text());
    }
    emit watchModeChanged(checked);
}

void appGui::inDirFile(const QString &inDir)
{
    this->ui->lneInputDir->setText(inDir);
}

void appGui::outDirFile(const QString &output)
{
    this->ui->lneOutputDir->setText(output);
}

void appGui::keyFile(const QString &key)
{
    this->ui->lneKey->setText(key);
}

void appGui::watchFile(const bool watch)
{
    this->ui->cbxWatch->setChecked(watch);
}

void appGui::on_btnIVLoad_clicked()
{
    QString fname = this->loadDirFile(false);
    QFile iVFile(fname);

    if (iVFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream iVReader(&iVFile);
        QString iv = iVReader.readAll().trimmed();

        ui->lneIV->setText("");
        ui->lneIV->setText(iv.trimmed());
        ui->lneIV->setFocus();

        iVFile.close();
    } else {
        QMessageBox msg;
        msg.setText("Fajl nije nadjen");
        msg.exec();
    }
}

void appGui::on_cbxBMP_clicked(bool checked)
{
    this->ui->cbxWatch->setEnabled(!checked);
    emit this->BMPModeChanged(checked);
}

void appGui::on_btnNext_clicked()
{
    emit this->nextClicked();
}

void appGui::setBeforeBMP(QString bB)
{
    this->beforeBMP.load(bB);
    this->beforeScene.addPixmap(this->beforeBMP);

    this->ui->gvBefore->setScene(&this->beforeScene);
}

void appGui::setAfterBMP(QString aB)
{
    this->afterBMP.load(aB);
    this->afterScene.addPixmap(this->afterBMP);

    this->ui->gvAfter->setScene(&this->afterScene);
}
