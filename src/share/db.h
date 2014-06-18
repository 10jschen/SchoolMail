#ifndef DB_H
#define DB_H

#include <QString>
#include <QList>
#include <QtSql>
#include "utils.h"

class DB
{
public:
    DB();
    ~DB();

    bool openDB(/*const */QString dbName);
    void close();

    bool addMailServerInfo(const QString& mailAddr, const QString& passwd,
                           const SmtpServerInfo& smtpServerInfo, const Pop3ServerInfo& pop3ServerInfo);
    bool getMailServerInfoList(QList<ServerInfo> &serverInfoList);
    bool modifyPasswd(const QString& mailAddr, const QString& passwd);
    bool modifySmtpServerInfo(const QString& mailAddr, const SmtpServerInfo& smtpServerInfo);
    bool modifyPop3ServerInfo(const QString& mailAddr, const Pop3ServerInfo& pop3ServerInfo);
    bool getPasswd(const QString& mailAddr, QString& passwd);
    bool getSmtpServerInfo(const QString& mailAddr, SmtpServerInfo& smtpServerInfo);
    bool getPop3ServerInfo(const QString& mailAddr, Pop3ServerInfo& pop3ServerInfo);
    bool getPop3ServerList(QList<Pop3ServerInfo>& pop3ServerList);
    bool removeMailInfo(const QString& mailAddr);

    bool addMessageInfo(const MessageInfo& messageInfo);
    bool modifyMessageState(const QString& mailAddr, const QString& subject,
                            const QString &date, int state);
    bool modifyMessageIsRead(const QString& mailAddr, const QString& subject, int isRead);
    bool getMessageInfo(const QString& mailAddr, QList<MessageInfo>& messageInfoList);
    bool getMessageState(const QString& mailAddr, const QString& subject,
                         const QString &date, int &state);
    bool getMessageIsRead(const QString& mailAddr, const QString& subject, int &isRead);
    bool getMsgUidl(const QString& mailAddr, const QString& subject,
                    const QString &date, QString& msgUidl);
    bool removeMessageInfo(const QString& mailAddr, const QList<QString>& subjectList);
    bool removeMessageInfoByMailAddr(const QString &mailAddr);
    bool removeMsg(const QString &mailAddr, const QString &subject,
                   const QString &date);

    bool addAddrBook(const QString& mailAddr, const QString& userName,
                     const QString& stuNum, const QString& phoneNum);
    bool getAddrBookList(QList<AddrBook> &addrBookList);
    bool removeAddrBook(const QString& mailAddr);
    bool getNameByAddr(const QString& mailAddr, QString &userName);

    bool addSignature(const QString& idName, const QString& context);
    bool removeSignature(const QString& idName);
    bool getSignatures(QList<Signature> &signature);
    bool getSignatureByTitle(Signature &signature, const QString& tile);

protected:
    void prepare();

private:
    //QSqlDatabase *db;
};

#endif // DB_H

