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

appGui::appGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::appGui)
{
    ui->setupUi(this);

    this->scene.setSceneRect(0, 0, 390, 270);

    for (int reg = 0; reg <= DEST; reg++) {
        QList<QGraphicsTextItem*> list;
        int blocks;
        switch (reg) {
        case SRC:
        case DEST:
            blocks = 25;
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

    this->drawRegister(SRC, "101010100111100001010101", "src");
    this->drawRegister(X, "1011110000101010110", "x");

    this->drawRegister(DEST, "1011110000110111010110", "dest");
}

void appGui::drawRegister(regs reg, const QString &bits, const QString &regName)
{
    this->scene.addText(regName)->setPos(0, this->calulateYFont(reg));
    for (int i = 0; i < bits.length(); i++) {
        this->registerMatrix[reg][i]->setPlainText(bits.at(i));
        this->registerMatrix[reg][i]->setPos(this->calculateX(i), this->calulateYFont(reg));
        this->registerMatrix[reg][i]->setVisible(true);
        this->scene.addRect(this->calculateX(i), this->calculateYReg(reg), REG_BLOCK, REG_BLOCK);
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

        emit this->keyLoad(key);
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

    this->drawRegister(Y, "1011110000101010101010", "y");
    this->drawRegister(Z, "10111100001010101100110", "z");
    ///TODO check if the directory exists
    const QString inputDir = this->loadDirFile(true);

    ui->lneInputDir->setText(inputDir);

    ui->btnEncrypt->setEnabled(this->canRun());
    ui->btnDecrypt->setEnabled(this->canRun());

    emit inDirLoad(inputDir);
}


void appGui::on_btnOutputDir_clicked()
{
    ///TODO check if the directory exists
    const QString outputDir = this->loadDirFile(true);

    ui->lneOutputDir->setText(outputDir);

    ui->btnEncrypt->setEnabled(this->canRun());
    ui->btnDecrypt->setEnabled(this->canRun());

    emit outDirLoad(outputDir);
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
    emit startAlgo(false);
}

void appGui::on_cbxWatch_clicked(bool checked)
{
    emit watchModeChanged(checked);
}

void appGui::inDirFile(const QString &inDir)
{
    this->ui->lneInputDir->setText(inDir);

    ui->btnEncrypt->setEnabled(this->canRun());
    ui->btnDecrypt->setEnabled(this->canRun());
}

void appGui::outDirFile(const QString &output)
{
    this->ui->lneOutputDir->setText(output);

    ui->btnEncrypt->setEnabled(this->canRun());
    ui->btnDecrypt->setEnabled(this->canRun());
}

void appGui::keyFile(const QString &key)
{
    this->ui->lneKey->setText(key);

    ui->btnEncrypt->setEnabled(this->canRun());
    ui->btnDecrypt->setEnabled(this->canRun());
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
