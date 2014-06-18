#ifndef ADDMAILBOXDLG_H
#define ADDMAILBOXDLG_H

#include <QDialog>
#include "../src/share/utils.h"

namespace Ui {
    class AddMailBoxDlg;
}

class AddMailBoxDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddMailBoxDlg(ServerInfo *serverInfo, QWidget *parent = 0);
    ~AddMailBoxDlg();

private:
    Ui::AddMailBoxDlg *ui;

    ServerInfo *serverInfo;

private slots:
    void on_ui_cancelButton_clicked();
    void on_ui_addButton_clicked();
};

#endif // ADDMAILBOXDLG_H
