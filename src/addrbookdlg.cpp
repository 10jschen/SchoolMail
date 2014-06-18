#include "addrbookdlg.h"
#include "ui_addrbookdlg.h"
#include <QMessageBox>

AddrBookDlg::AddrBookDlg(AddrBook *addrBook, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddrBookDlg)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    this->addrBook = addrBook;
}

AddrBookDlg::~AddrBookDlg()
{
    delete ui;
}

void AddrBookDlg::on_ui_addButton_clicked()
{
    if (ui->ui_mailAddr->text().isEmpty() || ui->ui_name->text().isEmpty()) {
        QMessageBox::information(this, "提示", "填写完整信息!");
        return;
    }

    addrBook->mailAddr = ui->ui_mailAddr->text().trimmed();
    addrBook->userName = ui->ui_name->text().trimmed();
    addrBook->num = ui->ui_num->text().trimmed();
    addrBook->phone = ui->ui_phone->text().trimmed();

    QDialog::accept();
}

void AddrBookDlg::on_ui_cancelButton_clicked()
{
    QDialog::reject();
}
