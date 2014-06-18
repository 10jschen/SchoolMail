#ifndef DOWNLOADATTACHDLG_H
#define DOWNLOADATTACHDLG_H

#include <QDialog>
#include <QString>
#include <QStringList>

namespace Ui {
    class DownloadAttachDlg;
}

class DownloadAttachDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadAttachDlg(QString path, QStringList attachList, QWidget *parent = 0);
    ~DownloadAttachDlg();

    void showAttachList();
    void copyFile(QString sourceFile, QString toFile);

private:
    Ui::DownloadAttachDlg *ui;

    QStringList attachList;
    QString     path;

private slots:
    void on_ui_downloadButton_clicked();
    void on_ui_closeButton_clicked();
};

#endif // DOWNLOADATTACHDLG_H
