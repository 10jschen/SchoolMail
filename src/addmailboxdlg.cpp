#include "addmailboxdlg.h"
#include "ui_addmailboxdlg.h"
#include <QMessageBox>

AddMailBoxDlg::AddMailBoxDlg(ServerInfo *serverInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMailBoxDlg)
{
    ui->setupUi(this);

    setAttribute (Qt::WA_DeleteOnClose);
    this->serverInfo = serverInfo;
}

AddMailBoxDlg::~AddMailBoxDlg()
{
    delete ui;
}

void AddMailBoxDlg::on_ui_addButton_clicked()
{
    if (ui->ui_pop3Server->text().isEmpty() || ui->ui_smtpServer->text().isEmpty()
        || ui->ui_user->text().isEmpty() || ui->ui_passwd->text().isEmpty()) {
        QMessageBox::information(this, "提示", "请将信息填写完整!");
        return;
    }

    if (ui->ui_connectComboBox->currentIndex() == 0)
        serverInfo->serverState = 0;
    else
        serverInfo->serverState = 2;

    if (ui->ui_pop3Port->currentIndex() == 0)
        serverInfo->pop3ServerPort = 110;
    else
        serverInfo->pop3ServerPort = 995;

    if (ui->ui_smtpPort->currentIndex() == 0)
        serverInfo->smtpServerPort = 25;
    else
        serverInfo->smtpServerPort = 465;

    serverInfo->pop3Server = ui->ui_pop3Server->text().trimmed();
    serverInfo->smtpServer = ui->ui_smtpServer->text().trimmed();
    serverInfo->user = ui->ui_user->text().trimmed();
    serverInfo->passwd = ui->ui_passwd->text().trimmed();

    QDialog::accept();
}

void AddMailBoxDlg::on_ui_cancelButton_clicked()
{
    QDialog::reject();
}
