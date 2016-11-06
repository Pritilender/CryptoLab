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

    this->scene.setSceneRect(0, 0, 390, 270);

    for (int reg = 0; reg <= DST; reg++) {
        QList<QGraphicsTextItem *> list;
        int blocks;
        switch (reg) {
        case SRC:
        case DST:
            blocks = 23;
            break;
        case X:
            blocks = 19;
            break;
        case Y:
            blocks = 22;
            break;
        case Z:
            blocks = 23;
            break;
        }
        for (int i = 0; i < blocks; i++) {
            list.append(this->scene.addText("0"));
            list[i]->setVisible(false);
        }
        this->registerMatrix.append(list);
    }
}

void appGui::drawRegister(regs reg, const QString &bits, const QString &regName)
{
    this->scene.addText(regName)->setPos(0, this->calulateYFont(reg));
    QPen pen(QColor(255, 0, 0));
    QPen *rectPen = nullptr;

    for (int i = 0; i < bits.length(); i++) {
        this->registerMatrix[reg][i]->setPlainText(bits.at(i));
        this->registerMatrix[reg][i]->setPos(this->calculateX(i), this->calulateYFont(reg));
        this->registerMatrix[reg][i]->setVisible(true);

        if ((i == 8 && reg == X) || (i == 10 && (reg == Y || reg == Z)) || (i == 0 && reg == SRC)
                || (i == 0 && reg == DST)) {
            rectPen = &pen;
        } else {
            rectPen = new QPen();
        }

        this->scene.addRect(this->calculateX(i), this->calculateYReg(reg), REG_BLOCK, REG_BLOCK, *rectPen);
    }

    this->ui->graphicsView->setScene(&this->scene);
    this->ui->graphicsView->show();
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
    emit this->outDirLoad(this->ui->lneOutputDir->text());
    emit this->startAlgo(true);
}

void appGui::on_btnKeyLoad_clicked()
{
    ///TODO check if file exists and do the validation!
    QString fname = this->loadDirFile(false);
    QFile keyFile(fname);

    if (keyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream keyReader(&keyFile);
        QString key = keyReader.readAll().trimmed();

        ui->lneKey->setText("");
        ui->lneKey->setText(key.trimmed());
        ui->lneKey->setFocus();

        keyFile.close();

        //emit this->keyLoad(key);
    } else {
        QMessageBox msg;
        msg.setText("Fajl nije nadjen");
        msg.exec();
    }
}

void appGui::on_lneKey_textChanged(const QString &arg1)
{
    ///TODO
//    KeyValidator kvalid;
//    QString key = arg1;

//    if (!kvalid.validate(key)) {
//        QMessageBox msg;
//        msg.setText("Dati ključ nije validan!");
//        msg.exec();
//        ui->btnEncrypt->setEnabled(this->canRun());
//        ui->btnDecrypt->setEnabled(this->canRun());
//    } else {
//        ui->btnEncrypt->setEnabled(false);
//        ui->btnDecrypt->setEnabled(false);
//    }
}

void appGui::on_btnInputDir_clicked()
{
    ///TODO check if the directory exists
    const QString inputDir = this->loadDirFile(true);

    ui->lneInputDir->setText(inputDir);

    //ui->btnEncrypt->setEnabled(this->canRun());
    //ui->btnDecrypt->setEnabled(this->canRun());

    //emit inDirLoad(inputDir);
}


void appGui::on_btnOutputDir_clicked()
{
    ///TODO check if the directory exists
    const QString outputDir = this->loadDirFile(true);

    ui->lneOutputDir->setText(outputDir);

    //ui->btnEncrypt->setEnabled(this->canRun());
    //ui->btnDecrypt->setEnabled(this->canRun());

    //emit outDirLoad(outputDir);
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
    if (checked) {
        emit this->inDirLoad(this->ui->lneInputDir->text());
    }
    emit watchModeChanged(checked);
}

void appGui::inDirFile(const QString &inDir)
{
    this->ui->lneInputDir->setText(inDir);

    //ui->btnEncrypt->setEnabled(this->canRun());
    //ui->btnDecrypt->setEnabled(this->canRun());
}

void appGui::outDirFile(const QString &output)
{
    this->ui->lneOutputDir->setText(output);

    //ui->btnEncrypt->setEnabled(this->canRun());
    //ui->btnDecrypt->setEnabled(this->canRun());
}

void appGui::keyFile(const QString &key)
{
    this->ui->lneKey->setText(key);

    //ui->btnEncrypt->setEnabled(this->canRun());
    //ui->btnDecrypt->setEnabled(this->canRun());
}

void appGui::encryptionFile(const bool encry)
{
    this->ui->lblStatus->setText(encry ? "Enkripcija" : "Dekripcija");
}

void appGui::runningFile(const bool running)
{
    this->ui->lblRunning->setText(running ? "u toku..." : "kraj.");
}

void appGui::watchFile(const bool watch)
{
    this->ui->cbxWatch->setChecked(watch);
}

void appGui::on_btnNext_clicked()
{
    emit this->nextStep();
}

void appGui::on_cbxSimulation_clicked(bool checked)
{
    //this->ui->btnNext->setEnabled(checked);
    emit this->simulationChanged(checked);
}

void appGui::drawRegisterX(const QString &regx)
{
    this->drawRegister(X, regx, "X");
}

void appGui::drawRegisterY(const QString &regy)
{
    this->drawRegister(Y, regy, "Y");
}

void appGui::drawRegisterZ(const QString &regz)
{
    this->drawRegister(Z, regz, "Z");
}

void appGui::drawSrc(const QString &src)
{
    this->drawRegister(SRC, src, "src");
}

void appGui::drawDst(const QString &dst)
{
    this->drawRegister(DST, dst, "dst");
}

void appGui::on_pushButton_clicked()
{
    QMessageBox::information(this, "Queue", QString::number(this->q->length()));
}
