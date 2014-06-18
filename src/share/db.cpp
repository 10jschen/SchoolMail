#include "db.h"
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

DB::DB()
{
}

DB::~DB()
{

}

void
DB::prepare()
{
    QSqlQuery q;

    q.exec("pragma foreign_keys = ON");
}

bool
DB::openDB(/*const */QString dbName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(dbName);
    bool ok = db.open();
    return ok;
}

void
DB::close()
{
    //db->close();
}

//邮件服务器信息设置

bool
DB::addMailServerInfo(const QString &mailAddr, const QString &passwd,
                      const SmtpServerInfo &smtpServerInfo, const Pop3ServerInfo &pop3ServerInfo)
{
    prepare();

    QSqlQuery q;

    q.prepare("insert into server_info (mail_addr, passwd, smtpserver, smtpserver_port, "
              "pop3server, pop3server_port, smtpserver_state, pop3server_state) "
              "values (?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(mailAddr);
    q.addBindValue(passwd);
    q.addBindValue(smtpServerInfo.smtpServer);
    q.addBindValue(smtpServerInfo.smtpServerPort);
    q.addBindValue(pop3ServerInfo.pop3Server);
    q.addBindValue(pop3ServerInfo.pop3serverPort);
    q.addBindValue(smtpServerInfo.smtpServerState);
    q.addBindValue(pop3ServerInfo.pop3ServerState);

    return q.exec();
}

bool
DB::getMailServerInfoList(QList<ServerInfo> &serverInfoList)
{
    prepare();

    QSqlQuery q;

    q.prepare("select mail_addr, passwd, smtpserver, smtpserver_port, pop3server, "
              "pop3server_port, smtpserver_state from server_info");

    if (!q.exec()) {
        return false;
    }

    while(q.next()) {
        ServerInfo serverInfo;

        serverInfo.user = q.value(0).toString();
        serverInfo.passwd = q.value(1).toString();
        serverInfo.smtpServer = q.value(2).toString();
        serverInfo.smtpServerPort = q.value(3).toUInt();
        serverInfo.pop3Server = q.value(4).toString();
        serverInfo.pop3ServerPort = q.value(5).toUInt();
        serverInfo.serverState = q.value(6).toInt();

        serverInfoList.append(serverInfo);
    }

    return true;
}

bool
DB::modifyPasswd(const QString &mailAddr, const QString &passwd)
{
    prepare();

    QSqlQuery q;

    q.prepare("update server_info set passwd = ? "
              "where mail_addr = ?");
    q.addBindValue(passwd);
    q.addBindValue(mailAddr);

    return q.exec();
}

bool
DB::modifySmtpServerInfo(const QString &mailAddr, const SmtpServerInfo &smtpServerInfo)
{
    prepare();

    QSqlQuery q;

    q.prepare("update server_info set smtpserver = ?, smtpserver_port = ?, smtpserver_state = ?, "
              "where mail_addr = ?");
    q.addBindValue(smtpServerInfo.smtpServer);
    q.addBindValue(smtpServerInfo.smtpServerPort);
    q.addBindValue(smtpServerInfo.smtpServerState);
    q.addBindValue(mailAddr);

    return q.exec();
}

bool
DB::modifyPop3ServerInfo(const QString &mailAddr, const Pop3ServerInfo &pop3ServerInfo)
{
    prepare();

    QSqlQuery q;

    q.prepare("update server_info set pop3server = ?, pop3server_port = ?, pop3server_state = ?"
              "where mail_addr = ?");
    q.addBindValue(pop3ServerInfo.pop3Server);
    q.addBindValue(pop3ServerInfo.pop3serverPort);
    q.addBindValue(pop3ServerInfo.pop3ServerState);
    q.addBindValue(mailAddr);

    return q.exec();
}

bool
DB::getPasswd(const QString &mailAddr, QString &passwd)
{
    prepare();

    QSqlQuery q;

    q.prepare("select passwd from server_info "
              "where mail_addr = ?");
    q.addBindValue(mailAddr);

    if (!q.exec())
        return false;
    if (!q.next())
        return false;

    passwd = q.value(0).toString();

    return true;
}

bool
DB::getSmtpServerInfo(const QString &mailAddr, SmtpServerInfo &smtpServerInfo)
{
    prepare();

    QSqlQuery q;

    q.prepare("select smtpserver, smtpserver_port, smtpserver_state from server_info "
              "where mail_addr = ?");
    q.addBindValue(mailAddr);

    if(!q.exec())
        return false;
    if(!q.next())
        return false;

    smtpServerInfo.smtpServer      = q.value(0).toString();
    smtpServerInfo.smtpServerPort  = q.value(1).toInt();
    smtpServerInfo.smtpServerState = q.value(2).toInt();

    return true;
}

bool
DB::getPop3ServerInfo(const QString &mailAddr, Pop3ServerInfo &pop3ServerInfo)
{
    prepare();

    QSqlQuery q;

    q.prepare("select passwd, pop3server, pop3server_port, pop3server_state from server_info "
              "where mail_addr = ?");
    q.addBindValue(mailAddr);

    if (!q.exec())
        return false;
    if (!q.next())
        return false;

    pop3ServerInfo.mailAddr = mailAddr;
    pop3ServerInfo.passwd = q.value(0).toString();
    pop3ServerInfo.pop3Server = q.value(1).toString();
    pop3ServerInfo.pop3serverPort = q.value(2).toInt();
    pop3ServerInfo.pop3ServerState = q.value(3).toInt();

    return true;
}

bool
DB::getPop3ServerList(QList<Pop3ServerInfo> &pop3ServerList)
{
    prepare();

    QSqlQuery q;
    Pop3ServerInfo pop3ServerInfo;

    q.prepare("select mail_addr, passwd, pop3server, pop3server_port, pop3server_state from server_info");

    if (!q.exec())
        return false;

    while(q.next()) {
        pop3ServerInfo.mailAddr = q.value(0).toString();
        pop3ServerInfo.passwd = q.value(1).toString();
        pop3ServerInfo.pop3Server = q.value(2).toString();
        pop3ServerInfo.pop3serverPort = q.value(3).toInt();
        pop3ServerInfo.pop3ServerState = q.value(4).toInt();

        pop3ServerList.append(pop3ServerInfo);
    }

    return true;
}

bool
DB::removeMailInfo(const QString &mailAddr)
{
    prepare();

    QSqlQuery q;

    q.prepare("delete from server_info where mail_addr = ?");
    q.addBindValue(mailAddr);

    if (!q.exec())
        return false;

    q.prepare("delete from msg_info where mail_addr = ?");
    q.addBindValue(mailAddr);

    if (!q.exec())
        return false;

    return true;
}

//邮件信息设置

bool
DB::addMessageInfo(const MessageInfo &messageInfo)
{
    prepare();

    QSqlQuery q;

    q.prepare("insert into msg_info (mail_addr, subject, sender, date, is_read, msg_state, msg_uidl) "
              "values (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(messageInfo.mailAddr);
    q.addBindValue(messageInfo.subject);
    q.addBindValue(messageInfo.sender);
    q.addBindValue(messageInfo.date);
    q.addBindValue(messageInfo.isRead);
    q.addBindValue(messageInfo.messageState);
    q.addBindValue(messageInfo.msgUidl);

    return q.exec();
}

bool
DB::modifyMessageState(const QString &mailAddr, const QString &subject,
                       const QString &date, int state)
{
    prepare();

    QSqlQuery q;

    q.prepare("update msg_info set msg_state = ? "
              "where mail_addr = ? and subject = ? and date = ?");
    q.addBindValue(state);
    q.addBindValue(mailAddr);
    q.addBindValue(subject);
    q.addBindValue(date);

    return q.exec();
}

bool
DB::modifyMessageIsRead(const QString &mailAddr, const QString &subject, int isRead)
{
    prepare();

    QSqlQuery q;

    q.prepare("update msg_info set is_read = ?"
              "where mail_addr = ? and subject = ?");
    q.addBindValue(isRead);
    q.addBindValue(mailAddr);
    q.addBindValue(subject);

    return q.exec();
}

bool
DB::getMessageInfo(const QString &mailAddr, QList<MessageInfo> &messageInfoList)
{
    prepare();

    QSqlQuery q;

    q.prepare("select mail_addr, subject, sender, date, is_read, msg_state, msg_uidl from msg_info "
              "where mail_addr = ?");
    q.addBindValue(mailAddr);

    if(!q.exec())
        return false;

    while(q.next())
    {
        MessageInfo messageInfo;

        messageInfo.mailAddr = q.value(0).toString();
        messageInfo.subject = q.value(1).toString();
        messageInfo.sender = q.value(2).toString();
        messageInfo.date = q.value(3).toString();
        messageInfo.isRead = q.value(4).toInt();
        messageInfo.messageState = q.value(5).toInt();
        messageInfo.msgUidl = q.value(6).toString();

        messageInfoList.append(messageInfo);
    }

    return true;
}

bool
DB::getMessageState(const QString &mailAddr, const QString &subject,
                    const QString &date, int &state)
{
    prepare();

    QSqlQuery q;

    q.prepare("select msg_state from msg_info "
              "where mail_addr = ? and subject = ? and date = ?");
    q.addBindValue(mailAddr);
    q.addBindValue(subject);
    q.addBindValue(date);

    if (!q.exec())
        return false;
    if (!q.next())
        return false;

    state = q.value(0).toInt();

    return true;
}

bool
DB::getMessageIsRead(const QString &mailAddr, const QString &subject, int &isRead)
{
    prepare();

    QSqlQuery q;

    q.prepare("select is_read from msg_info"
              "where mail_addr = ? and subject = ?");
    q.addBindValue(mailAddr);
    q.addBindValue(subject);

    if (!q.exec())
        return false;
    if (!q.next())
        return false;

    isRead = q.value(0).toInt();

    return true;
}

bool
DB::getMsgUidl(const QString &mailAddr, const QString &subject,
               const QString &date, QString &msgUidl)
{
    prepare();

    QSqlQuery q;

    q.prepare("select msg_uidl from msg_info "
              "where mail_addr = ? and subject = ? and date = ?");
    q.addBindValue(mailAddr);
    q.addBindValue(subject);
    q.addBindValue(date);

    if (!q.exec())
        return false;
    if (!q.next()) {
        qDebug() << "@@@@@@@@@@@@@@@";
        qDebug() << mailAddr << subject << date;
        qDebug() << "@@@@@@@@@@@@@@@";
        return false;
    }

    msgUidl = q.value(0).toString();

    return true;
}

bool
DB::removeMessageInfo(const QString &mailAddr, const QList<QString> &subjectList)
{
    prepare();

    QSqlQuery q;

    for (int i = 0; i < subjectList.length(); i++) {
        q.prepare("delete from msg_info where mail_addr = ? and subject = ?");
        q.addBindValue(mailAddr);
        q.addBindValue(subjectList.at(i));
        if (!q.exec())
            return false;
    }

    return true;
}

bool
DB::removeMsg(const QString &mailAddr, const QString &subject, const QString &date)
{
    prepare();

    QSqlQuery q;
    q.prepare("delete from msg_info where mail_addr = ? and subject = ? and date = ?");
    q.addBindValue(mailAddr);
    q.addBindValue(subject);
    q.addBindValue(date);

    return q.exec();
}

bool
DB::removeMessageInfoByMailAddr(const QString &mailAddr)
{
    prepare();

    QSqlQuery q;

    q.prepare("delete from msg_info where mail_addr = ?");
    q.addBindValue(mailAddr);

    if (!q.exec())
        return false;

    return true;
}

//地址簿设置

bool
DB::addAddrBook(const QString &mailAddr, const QString &userName,
                const QString &stuNum, const QString &phoneNum)
{
    prepare();

    QSqlQuery q;

    q.prepare("insert into addr_book (mail_addr, user_name, stu_num, phone_num)"
              "values (?, ?, ?, ?)");
    q.addBindValue(mailAddr);
    q.addBindValue(userName);
    q.addBindValue(stuNum);
    q.addBindValue(phoneNum);

    return q.exec();
}

bool
DB::getAddrBookList(QList<AddrBook> &addrBookList)
{
    prepare();

    QSqlQuery q;

    q.prepare("select mail_addr, user_name, stu_num, phone_num from addr_book");

    if (!q.exec())
        return false;

    while (q.next()) {
        AddrBook addrBook;

        addrBook.mailAddr = q.value(0).toString();
        addrBook.userName = q.value(1).toString();
        addrBook.num = q.value(2).toString();
        addrBook.phone = q.value(3).toString();

        addrBookList.append(addrBook);
    }

    return true;
}

bool
DB::getNameByAddr(const QString &mailAddr, QString &userName)
{
    prepare();

    QSqlQuery q;
    q.prepare("select user_name from addr_book where mail_addr = ?");
    q.addBindValue(mailAddr);

    if (!q.exec())
        return false;
    if (!q.next())
        return false;

    userName = q.value(0).toString();
    return true;
}

bool
DB::removeAddrBook(const QString &mailAddr)
{
    prepare();

    QSqlQuery q;

    q.prepare("delete from addr_book where mail_addr = ?");
    q.addBindValue(mailAddr);

    return q.exec();
}

//个性签名设置

bool
DB::addSignature(const QString &idName, const QString &context)
{
    prepare();

    QSqlQuery q;

    q.prepare("insert into signature (signature_name, context) "
              "values (?, ?)");
    q.addBindValue(idName);
    q.addBindValue(context);

    return q.exec();
}

bool
DB::removeSignature(const QString &idName)
{
    prepare();

    QSqlQuery q;

    q.prepare("delete from signature where signature_name = ?");
    q.addBindValue(idName);

    return q.exec();
}

bool
DB::getSignatures(QList<Signature> &signature)
{
    prepare();

    QSqlQuery q;

    q.prepare("select signature_name, context from signature");

    if (!q.exec())
        return false;

    while(q.next()) {
        Signature signatureItem;

        signatureItem.title = q.value(0).toString();
        signatureItem.context = q.value(1).toString();
        signature.append(signatureItem);
    }

    return true;
}

bool
DB::getSignatureByTitle(Signature &signature, const QString &title)
{
    prepare();

    QSqlQuery q;

    q.prepare("select context from signature where signature_name = ?");
    q.addBindValue(title);

    if (!q.exec())
        return false;

    if (!q.next())
        return false;

    signature.context = q.value(0).toString();
    signature.title = title;

    return true;
}
