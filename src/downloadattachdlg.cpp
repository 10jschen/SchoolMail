#include "downloadattachdlg.h"
#include "ui_downloadattachdlg.h"
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QFile>
#include <QDebug>

DownloadAttachDlg::DownloadAttachDlg(QString path, QStringList attachList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadAttachDlg)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    ui->ui_attachList->horizontalHeader()->setStretchLastSection(true);
    ui->ui_attachList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->path = path;
    this->attachList = attachList;
    showAttachList();
}

DownloadAttachDlg::~DownloadAttachDlg()
{
    delete ui;
}

void DownloadAttachDlg::showAttachList()
{
    qDebug() << "attachList size = " << attachList.size();
    ui->ui_attachList->setRowCount(attachList.size());

    for (int i = 0; i < attachList.size(); ++i) {
        QTableWidgetItem *fileName = new QTableWidgetItem(attachList.at(i));
        QCheckBox *checkBox = new QCheckBox();

        ui->ui_attachList->setCellWidget(i, 0, checkBox);
        ui->ui_attachList->setItem(i, 1, fileName);
    }
}

void DownloadAttachDlg::on_ui_closeButton_clicked()
{
    this->close();
}

void DownloadAttachDlg::copyFile(QString sourceFile, QString toFile)
{
    if (sourceFile == toFile)
        return;
    if (!QFile::exists(sourceFile))
        return;
    QFile::copy(sourceFile, toFile);
}

void DownloadAttachDlg::on_ui_downloadButton_clicked()
{
    if (ui->ui_attachList->rowCount() == 0)
        return;

    QString dir = QFileDialog::getExistingDirectory(this, tr("保存目录"),
                                                    "C:/Users/Administrator/Desktop",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);


    for (int i = 0; i < ui->ui_attachList->rowCount(); ++i) {
        QCheckBox *checkBox = (QCheckBox *)ui->ui_attachList->cellWidget(i, 0);
        if (checkBox->checkState() == Qt::Checked) {
            qDebug() << ui->ui_attachList->item(i, 1)->text();
            copyFile(path + attachList.at(i), dir + "/" + ui->ui_attachList->item(i, 1)->text());
        }
    }
}
