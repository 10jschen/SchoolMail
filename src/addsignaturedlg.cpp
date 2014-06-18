#include "addsignaturedlg.h"
#include "ui_addsignaturedlg.h"
#include <QMessageBox>
#include <QDebug>

AddSignatureDlg::AddSignatureDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSignatureDlg)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    QObject::connect(ui->ui_hasName, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(showSignature(int)));
    openDB();
    addSignatureItems();
}

AddSignatureDlg::~AddSignatureDlg()
{
    delete ui;
}

void
AddSignatureDlg::openDB()
{
    if (!db.openDB("./mail_db.db")) {
        qDebug() << "openDB打不开数据库!";
        QMessageBox::information(this, "提示", "数据库打开错误！");
        return;
    }
}

void AddSignatureDlg::on_ui_closeButton_clicked()
{
    this->close();
}

void AddSignatureDlg::on_ui_addButton_clicked()
{
    if (ui->ui_newName->text().trimmed().isEmpty()
        || ui->ui_context->toPlainText().isEmpty()) {
        QMessageBox::information(this, "提示", "请填写完整信息!");
        return;
    }

    QString signatureName = ui->ui_newName->text();
    QString context = ui->ui_context->toHtml();

    qDebug() << signatureName;
    qDebug() << context;

    if (!db.addSignature(signatureName, context)) {
        QMessageBox::information(this, "提示", "添加签名失败!");
        return;
    }
    ui->ui_hasName->addItem(signatureName);
}

void
AddSignatureDlg::addSignatureItems()
{
    QList<Signature> signatureList;

    if (!db.getSignatures(signatureList)) {
        return;
    }

    int count = signatureList.count();

    for (int i = 0; i < count; i++) {
        ui->ui_hasName->addItem(signatureList.at(i).title);
    }
}

void
AddSignatureDlg::showSignature(int index)
{
    if (index == 0)
        return;

    QString title = ui->ui_hasName->currentText();
    Signature signature;

    if (!db.getSignatureByTitle(signature, title)) {
        return;
    }

    ui->ui_newName->setText(signature.title);
    ui->ui_context->setHtml(signature.context);
}
