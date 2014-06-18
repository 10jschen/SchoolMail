#ifndef MIMETHREAD_H
#define MIMETHREAD_H

#include <QThread>
#include <QString>
#include <QDebug>
#include <QList>

#include "../../src/smtp/SmtpMime"
#include "../../src/share/utils.h"
#include "../../src/share/db.h"

class MimeThread : public QThread
{
    Q_OBJECT
public:
    explicit MimeThread(MimeInfo mimeInfo, QList<QString> attachList, QList<QString> inlineList,
                         QObject *parent = 0);

    void run();
    void prepareInline();
    EmailAddress *stringToEmail(const QString &str);
    void sendComMail();
    void sendDifMail();
    void getFlag();
    void replaceName(QString &content, const QString &name);
    void openDB();
    void saveMail(MimeMessage *mimeMessage);
    QString getCurrentTime();

private:
    MimeInfo mimeInfo;
    QList<QString> attachList;
    QList<QString> inlineList;
    MimeMessage mimeMessage;
    int flag;
    DB  db;

signals:
    void sendMailError();
    void sendMailSuccess();
    void connectHostError();
    void loginError();

public slots:

};

#endif // MIMETHREAD_H
