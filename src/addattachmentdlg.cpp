#include "addattachmentdlg.h"
#include "ui_addattachmentdlg.h"
#include <QCheckBox>
#include <QFileDialog>
#include <QStringList>
#include <QDebug>

AddAttachmentDlg::AddAttachmentDlg(QList<QString> *attachList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAttachmentDlg)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    ui->ui_attachList->horizontalHeader()->setStretchLastSection(true);
    ui->ui_attachList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->attachList = attachList;
    showAttachList();
}

AddAttachmentDlg::~AddAttachmentDlg()
{
    delete ui;
}

void AddAttachmentDlg::showAttachList()
{
    ui->ui_attachList->setRowCount(attachList->count());

    for (int i = 0; i < attachList->count(); ++i) {
        QTableWidgetItem *fileName = new QTableWidgetItem(attachList->at(i));
        QCheckBox *checkBox = new QCheckBox();

        ui->ui_attachList->setCellWidget(i, 0, checkBox);
        ui->ui_attachList->setItem(i, 1, fileName);
    }
}

void AddAttachmentDlg::on_ui_addButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);


    if (dialog.exec()) {
        QStringList fileList = dialog.selectedFiles();

        for (int i = 0; i < fileList.count(); ++i) {
            ui->ui_attachList->insertRow(ui->ui_attachList->rowCount());

            QTableWidgetItem *fileName = new QTableWidgetItem(fileList.at(i));
            QCheckBox *checkBox = new QCheckBox();

            ui->ui_attachList->setCellWidget(ui->ui_attachList->rowCount() - 1, 0, checkBox);
            ui->ui_attachList->setItem(ui->ui_attachList->rowCount() - 1, 1, fileName);
            attachList->append(fileList.at(i));
        }
    }
}

void AddAttachmentDlg::on_ui_delButton_clicked()
{
    for (int i = ui->ui_attachList->rowCount() - 1; i >= 0; --i) {

        QCheckBox *checkBox = (QCheckBox *)ui->ui_attachList->cellWidget(i, 0);
        if (checkBox->checkState() == Qt::Checked) {
            qDebug() << ui->ui_attachList->item(i, 1)->text();
            attachList->removeOne(ui->ui_attachList->item(i, 1)->text());
            ui->ui_attachList->removeRow(i);
        }
    }
}

void AddAttachmentDlg::on_ui_closeButton_clicked()
{
    this->close();
}
