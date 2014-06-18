#include "showaddrbookdlg.h"
#include "ui_showaddrbookdlg.h"
#include "../src/addrbookdlg.h"
#include <QMessageBox>
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QDebug>

ShowAddrBookDlg::ShowAddrBookDlg( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowAddrBookDlg)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    ui->ui_addrList->horizontalHeader()->setStretchLastSection(true);
    ui->ui_addrList->setEditTriggers (QAbstractItemView::NoEditTriggers);
    showAddrBookList();
}

ShowAddrBookDlg::~ShowAddrBookDlg()
{
    delete ui;
}

void ShowAddrBookDlg::showAddrBookList()
{
    QList<AddrBook> addrBookList;
    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "提示", "无法打开数据库！");
        return;
    }

    if (!db.getAddrBookList(addrBookList)) {
        QMessageBox::information(this, "提示", "数据库出错!");
        return;
    }
    qDebug() << "++++++++++++++++";
    qDebug() << addrBookList.count();
    qDebug() << "++++++++++++++++";
    ui->ui_addrList->setRowCount(addrBookList.count());
    for (int i = 0; i < addrBookList.count(); ++i) {
        QTableWidgetItem *mailAddr = new QTableWidgetItem(addrBookList.at(i).mailAddr);
        QTableWidgetItem *userName = new QTableWidgetItem(addrBookList.at(i).userName);
        QTableWidgetItem *phone = new QTableWidgetItem(addrBookList.at(i).phone);
        QTableWidgetItem *num = new QTableWidgetItem(addrBookList.at(i).num);
        QCheckBox *checkBox = new QCheckBox();

        ui->ui_addrList->setCellWidget(i, 0, checkBox);
        ui->ui_addrList->setItem(i, 1, mailAddr);
        ui->ui_addrList->setItem(i, 2, userName);
        ui->ui_addrList->setItem(i, 3, phone);
        ui->ui_addrList->setItem(i, 4, num);
    }
}

void ShowAddrBookDlg::on_ui_addButton_clicked()
{
    AddrBook addrBook;

    AddrBookDlg *addrBookDlg = new AddrBookDlg(&addrBook, NULL);

    if (addrBookDlg->exec() == QDialog::Rejected) {
        qDebug() << "addrBookDlg rejected";
        return;
    }

    DB db;
    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "提示", "无法打开数据库!");
        qDebug() << "on_addMailBoxAction_triggered无法打开数据库!";
        return;
    }
    if (!db.addAddrBook(addrBook.mailAddr, addrBook.userName, addrBook.num, addrBook.phone)) {
        QMessageBox::information(this, "提示", "插入数据库出错！");
        return;
    }

    ui->ui_addrList->insertRow(ui->ui_addrList->rowCount());
    QTableWidgetItem *mailAddr = new QTableWidgetItem(addrBook.mailAddr);
    QTableWidgetItem *userName = new QTableWidgetItem(addrBook.userName);
    QTableWidgetItem *phone = new QTableWidgetItem(addrBook.phone);
    QTableWidgetItem *num = new QTableWidgetItem(addrBook.num);
    QCheckBox *checkBox = new QCheckBox();

    ui->ui_addrList->setCellWidget(ui->ui_addrList->rowCount() - 1, 0, checkBox);
    ui->ui_addrList->setItem(ui->ui_addrList->rowCount() - 1, 1, mailAddr);
    ui->ui_addrList->setItem(ui->ui_addrList->rowCount() - 1, 2, userName);
    ui->ui_addrList->setItem(ui->ui_addrList->rowCount() - 1, 3, phone);
    ui->ui_addrList->setItem(ui->ui_addrList->rowCount() - 1, 4, num);
}

void ShowAddrBookDlg::on_ui_shutdownButton_clicked()
{
    this->close();
}

void ShowAddrBookDlg::on_ui_delButton_clicked()
{
    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "提示", "无法打开数据库！");
        return;
    }

    for (int i = ui->ui_addrList->rowCount() - 1; i >= 0; --i) {
        QCheckBox *checkBox = (QCheckBox *)ui->ui_addrList->cellWidget(i, 0);
        if (checkBox->checkState() == Qt::Checked) {
            qDebug() << ui->ui_addrList->item(i, 1)->text();
            db.removeAddrBook(ui->ui_addrList->item(i, 1)->text());
            ui->ui_addrList->removeRow(i);
        }
    }
}
