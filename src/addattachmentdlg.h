#ifndef ADDATTACHMENTDLG_H
#define ADDATTACHMENTDLG_H

#include <QDialog>
#include <QList>
#include <QString>

namespace Ui {
    class AddAttachmentDlg;
}

class AddAttachmentDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddAttachmentDlg(QList<QString> *attachList, QWidget *parent = 0);
    ~AddAttachmentDlg();

    void showAttachList();

private:
    Ui::AddAttachmentDlg *ui;

    QList<QString> *attachList;

private slots:
    void on_ui_closeButton_clicked();
    void on_ui_delButton_clicked();
    void on_ui_addButton_clicked();
};

#endif // ADDATTACHMENTDLG_H
