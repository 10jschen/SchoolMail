#ifndef UTILS_H
#define UTILS_H

#include <QString>
//#include <QMutex>

//QMutex mutex;
/*
enum ENCODE_MODE {
    _7Bit,
    _8Bit,
    Base64,
    QuotedPrintable
};

enum PART_TYPE {
    Mixed,         //0
    Digest,        //1
    Alternative,   //2
    Related,       //3
    Report,        //4
    Signed,        //5
    Encrypted      //6
};

enum RECEIVER_TYPE {
    To,
    Cc,
    Bcc
};

enum CONNECTION_TYPE {
    TCP_CONNECTTION,
    TLS_CONNECTION,
    SSL_CONNECTION
};

enum SMTP_ERROR {
    SERVER_ERROR,
    CLIENT_ERROR,
    RESPONSETIMEOUT_ERROR,
    CONNECTIONTIMEOUT_ERROR,
    AUTH_ERROR
};

const QString PART_TYPE_NAME[] = {
    "multipart/mixed",
    "multipart/digest",
    "multipart/alternative",
    "multipart/related",
    "multipart/report",
    "multipart/signed",
    "multipart/encrypted"
};

class TimeoutException {

};
*/
struct SmtpServerInfo {
    QString smtpServer;
    quint16 smtpServerPort;
    int smtpServerState;     // 0--common, 2--STARTTLS, 1--SSL
};

struct Pop3ServerInfo {
    QString mailAddr;
    QString passwd;
    QString pop3Server;
    quint16 pop3serverPort;
    int pop3ServerState;     // 0--common, 2--STARTTLS, 1--SSL
};

struct MessageInfo {
    QString mailAddr;
    QString subject;
    QString date;
    QString sender;
    QString msgUidl;
    int isRead;
    int messageState;       // 0--收件箱，1--发件箱， 2--草稿箱，3--垃圾箱
};

struct MimeInfo {
    QString subject;
    QString content;
    QString to;
    QString cc;
    QString bcc;
    QString mailAddr;
    QString passwd;
    QString smtpServer;
    int     smtpPort;
    int     ssl;
};

struct ServerInfo {
    QString pop3Server;
    quint16 pop3ServerPort;
    QString smtpServer;
    quint16 smtpServerPort;
    QString user;
    QString passwd;
    int     serverState;
};

struct AddrBook {
    QString mailAddr;
    QString userName;
    QString num;
    QString phone;
};

struct Signature {
    QString title;
    QString context;
};

#endif // UTILS_H
