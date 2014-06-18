#ifndef SENDMESSAGE_H
#define SENDMESSAGE_H

#include <QWidget>
#include <QList>
#include <QString>

#include "../src/share/db.h"
#include "../src/share/utils.h"
#include "../src/smtp/SmtpMime"

#include <QFont>


namespace Ui {
class SendMessage;
}

class SendMessage : public QWidget
{
    Q_OBJECT
    
public:
    explicit SendMessage(QWidget *parent = 0);
    ~SendMessage();

    EmailAddress* stringToEmail(const QString& address);
    void init();
    void initServerInfoList();
    void initAddrBookList();
    void initSignature();
    void sendCommonMail();
    void sendDifMail();
    
private:
    Ui::SendMessage *ui;

    DB db;
    QList<ServerInfo> serverInfoList;
    QList<AddrBook>   addrBookList;
    QList<QString>    attachList;
    QList<QString>    inlineFileList;
    QList<Signature>  signatureList;

private slots:
    void on_ui_addNameButton_clicked();
    void on_ui_addPicButton_clicked();
    void on_ui_attachButton_clicked();
    void on_ui_italicButton_clicked();
    void on_ui_BoldButton_clicked();
    void on_ui_rightButton_clicked();
    void on_ui_centerButton_clicked();
    void on_ui_leftButton_clicked();
    void on_ui_sendButton_clicked();
    void setTextFont(const QFont &font);
    void setTextSize(const QString &sizeStr);
    void setSignature(int index);

    void showConnectHostError();
    void showLoginError();
    void showSendMailError();
    void showSendMailSuccess();
};

#endif // SENDMESSAGE_H
