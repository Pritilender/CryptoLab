#include "appgui.h"
#include "ui_appgui.h"
#include "QVector"
#include "QMessageBox"
#include "keyvalidator.h"
#include "QFileDialog"
#include "QFile"
#include "QTextStream"
#include "QDirIterator"
#include "cryptoworker.h"

#include "QDebug"

void appGui::loadFile(QLineEdit *lne)
{
    QString fname = this->loadDirFile(false);
    QFile file(fname);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream reader(&file);
        QString text = reader.readAll().trimmed();

        lne->setText("");
        lne->setText(text.trimmed());
        lne->setFocus();

        file.close();
    } else {
        QMessageBox msg;
        msg.setText("Fajl nije nadjen");
        msg.exec();
    }
}

void appGui::emitEncryption(bool enc)
{
    if (!this->ui->cbxWatch->isChecked()) {
        emit this->inDirLoad(this->ui->lneInputDir->text());
    }
    emit this->outDirLoad(this->ui->lneOutputDir->text());
    emit this->arrayLoad(this->ui->lneArray->text());
    emit this->mLoad(this->ui->lneM->text());
    emit this->nLoad(this->ui->lneN->text());
    emit this->startAlgo(enc);
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
    this->emitEncryption(true);
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
    this->emitEncryption(false);
}

void appGui::on_cbxWatch_clicked(bool checked)
{
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

void appGui::arrayFile(const QString &array)
{
    this->ui->lneArray->setText(array);
}

void appGui::watchFile(const bool watch)
{
    this->ui->cbxWatch->setChecked(watch);
}

void appGui::on_btnArrayLoad_clicked()
{
    this->loadFile(this->ui->lneArray);
}

void appGui::on_btnM_clicked()
{
    this->loadFile(this->ui->lneM);
}

void appGui::on_btnN_clicked()
{
    this->loadFile(this->ui->lneN);
}

void appGui::nFile(const QString &n)
{
    this->ui->lneN->setText(n);
}

void appGui::mFile(const QString &m)
{
    this->ui->lneM->setText(m);
}

void appGui::on_lineEdit_textChanged(const QString &arg1)
{
    this->ui->lblOut->setTextFormat(Qt::RichText);

    QByteArray bytes(arg1.toLatin1(), arg1.length());

    uint32_t pKey[8];
    QStringList arrSplit = this->ui->lneArray->text().split(',');

    for (int i = 0; i < 8; i++) {
        pKey[i] = arrSplit[i].toUInt();
    }

    QByteArray res = Knapsack::encryptByKnapsack(bytes, pKey);

    QString txt;

    for (int i = 0; i < res.length(); i += 4) {
        txt.append(QString::number(QString(res.mid(i, 4).toHex()).toInt(nullptr, 16)) + QChar(' '));
    }

    this->ui->lblOut->setText(txt);
}

void appGui::on_lineEdit_selectionChanged()
{
    qDebug() << this->ui->lineEdit->selectedText();

    QString lbl = this->ui->lblOut->text();
    lbl.remove(QRegExp("<\\/*b>"));

    QString res;
    int selStart = this->ui->lineEdit->selectionStart();
    int selEnd = selStart + this->ui->lineEdit->selectedText().length();

    for (int i = 0; i < lbl.length(); i++) {
        if (i == selStart * 4) {
            res.append("<b>");
        } else if (i == selEnd * 4) {
            res.append("</b>");
        }
        res.append(lbl.at(i));
    }

    this->ui->lblOut->setText(res);
}
