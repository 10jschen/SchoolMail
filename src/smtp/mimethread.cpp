#include "mimethread.h"
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include "../../src/share/db.h"

MimeThread::MimeThread(MimeInfo mimeInfo, QList<QString> attachList, QList<QString> inlineList,
                        QObject *parent) :
    QThread(parent), mimeInfo(mimeInfo), attachList(attachList),
    inlineList(inlineList), flag(0)
{

}

void MimeThread::prepareInline()
{
    if (inlineList.count() == 0)
        return;
    for (int i = 0; i < inlineList.count(); ++i) {
        QFileInfo fileInfo(inlineList.at(i));

        mimeInfo.content.replace(inlineList.at(i), "cid:" + fileInfo.baseName());
    }
}

void MimeThread::getFlag()
{
    if (mimeInfo.content.isEmpty())
        return;
    if (mimeInfo.content.contains(QString("[/name/]"), Qt::CaseInsensitive))
        flag = 1;
}

void MimeThread::replaceName(QString &content, const QString &name)
{
    if (!content.isEmpty())
        content.replace(QString("[/name/]"), name);
}

void MimeThread::openDB()
{
    if (!db.openDB("./mail_db.db")) {
        qDebug() << "MimeThread can't open db First!";
    }
}

EmailAddress* MimeThread::stringToEmail(const QString &str)
{
    int p1 = str.indexOf("<");
    int p2 = str.indexOf(">");

    if (p1 == -1)
    {
        // no name, only email address
        return new EmailAddress(str);
    }
    else
    {
        return new EmailAddress(str.mid(p1 + 1, p2 - p1 - 1), str.left(p1));
    }
}

QString MimeThread::getCurrentTime()
{
    const int WEEK = 7;
    const int MONTH = 12;
    QDateTime time = QDateTime::currentDateTime();
    QString   str = time.toString("ddd, d MMM yyyy hh:mm:ss +0800");
    qDebug() << str;
    QStringList li = str.split(" ");
    QString month_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
                        "Aug", "Sep", "Oct", "Nov", "Dec"};
    QString month_ch[] = {"一月", "二月", "三月", "四月", "五月", "六月", "七月",
                        "八月", "九月", "十月", "十一月", "十二月"};
    QString week_name[] = {"Mon,", "Tue,", "Wed,", "Thu,", "Fri,", "Sat,", "Sun,"};
    QString week_ch[] = {"周一,", "周二,", "周三,", "周四,", "周五,", "周六,", "周日,"};

    for (int i = 0; i < WEEK; ++i) {
        if (li.at(0) == week_ch[i]) {
            li[0] = week_name[i];
            break;
        }
    }
    for (int i = 0; i < MONTH; ++i) {
        if (li.at(2) == month_ch[i]) {
            li[2] = month_name[i];
            break;
        }
    }

    return li.join(" ");
}

void MimeThread::saveMail(MimeMessage *mimeMessage)
{
    DB db;
    QDateTime time = QDateTime::currentDateTime();
    QString fileName = time.toString("yyyy-MM-dd-hh-mm-ss");
    QFile file("./schoolmail/mail/" + mimeInfo.mailAddr + "/" + fileName + ".eml");
    file.open(QIODevice::WriteOnly);
    QTextStream textStream(&file);

    textStream << mimeMessage->toString();
    textStream.flush();
    file.close();

    if (!db.openDB("./mail_db.db")) {
        qDebug() << "MimeThread db can't open!";
    }

    MessageInfo messageInfo;

    messageInfo.mailAddr = mimeInfo.mailAddr;
    messageInfo.subject  = mimeInfo.subject;
    messageInfo.sender = mimeInfo.mailAddr;
    messageInfo.msgUidl = fileName;
    messageInfo.isRead = 1;
    messageInfo.messageState = 1;
    messageInfo.date = fileName;

    if (!db.addMessageInfo(messageInfo)) {
        qDebug() << "mimeThread addMessageInfo error!";
    }
}

void MimeThread::sendComMail()
{
    SmtpClient smtp(mimeInfo.smtpServer, mimeInfo.smtpPort,
                    mimeInfo.ssl? SmtpClient::SslConnection : SmtpClient::TcpConnection);

    if (!smtp.connectToHost())
    {
        emit connectHostError();
        return;
    }

    bool auth = true;
    if (auth) {
        if (!smtp.login(mimeInfo.mailAddr, mimeInfo.passwd))
        {
            emit loginError();
            return;
        }
    }

    MimeMessage mimeMessage;
    QStringList recipientList = mimeInfo.to.split(";");
    MimeHtml mimeHtml;

    mimeMessage.setSender(stringToEmail(mimeInfo.mailAddr));
    mimeMessage.setSubject(mimeInfo.subject);
    for (int i = 0; i < recipientList.count(); ++i) {
        mimeMessage.addRecipient(stringToEmail(recipientList.at(i)));
    }

    if (!mimeInfo.cc.isEmpty()) {
        QStringList ccList = mimeInfo.cc.split(";");
        for (int i = 0; i < ccList.count(); ++i) {
            mimeMessage.addRecipient(stringToEmail(ccList.at(i)), MimeMessage::Cc);
        }
    }

    if (!mimeInfo.bcc.isEmpty()) {
        QStringList bccList = mimeInfo.bcc.split(";");
        for (int i = 0; i < bccList.count(); ++i) {
            mimeMessage.addRecipient(stringToEmail(bccList.at(i)), MimeMessage::Bcc);
        }
    }
    mimeHtml.setHtml(mimeInfo.content);
    mimeMessage.addPart(&mimeHtml);

    for (int i = 0; i < inlineList.count(); ++i) {
        MimeInlineFile *mimeInlineFile = new MimeInlineFile(new QFile(inlineList.at(i)));
        QFileInfo fileInfo(inlineList.at(i));

        mimeInlineFile->setContentId(fileInfo.baseName());
        mimeMessage.addPart(mimeInlineFile);
    }

    for (int i = 0; i < attachList.count(); ++i) {
        mimeMessage.addPart(new MimeAttachment(new QFile(attachList.at(i))));
    }

    if (!smtp.sendMail(mimeMessage))
    {
        emit sendMailError();
    }
    else
    {
        saveMail(&mimeMessage);
        emit sendMailSuccess();
    }
    smtp.quit();
}

void MimeThread::sendDifMail()
{
    SmtpClient smtp(mimeInfo.smtpServer, mimeInfo.smtpPort,
                    mimeInfo.ssl? SmtpClient::SslConnection : SmtpClient::TcpConnection);

    if (!smtp.connectToHost())
    {
        emit connectHostError();
        return;
    }

    bool auth = true;
    if (auth) {
        if (!smtp.login(mimeInfo.mailAddr, mimeInfo.passwd))
        {
            emit loginError();
            return;
        }
    }

    openDB();
    if (!mimeInfo.to.isEmpty()) {
        QStringList toList = mimeInfo.to.split(";");
        for (int i = 0; i < toList.count(); ++i) {
            QString tmpContent(mimeInfo.content);
            QString userName;
            if (!db.getNameByAddr(toList.at(i), userName))
                userName = toList.at(i);
            MimeMessage mimeMessage;
            MimeHtml mimeHtml;

            mimeMessage.setSender(stringToEmail(mimeInfo.mailAddr));
            mimeMessage.setSubject(mimeInfo.subject);
            mimeMessage.addRecipient(stringToEmail(toList.at(i)));
            replaceName(tmpContent, userName);
            mimeHtml.setHtml(tmpContent);
            mimeMessage.addPart(&mimeHtml);

            for (int j = 0; j < inlineList.count(); ++j) {
                MimeInlineFile *mimeInlineFile = new MimeInlineFile(new QFile(inlineList.at(j)));
                QFileInfo fileInfo(inlineList.at(j));

                mimeInlineFile->setContentId(fileInfo.baseName());
                mimeMessage.addPart(mimeInlineFile);
            }

            for (int j = 0; j < attachList.count(); ++j) {
                mimeMessage.addPart(new MimeAttachment(new QFile(attachList.at(j))));
            }

            if (!smtp.sendMail(mimeMessage)) {
                qDebug() << "!!sendMail " + toList.at(i) + " error!!";
                if (i == toList.count() - 1)
                    emit sendMailError();
            } else {
                if (i == toList.count() - 1)
                    emit sendMailSuccess();
            }
        }
    }
    MimeMessage mimeMessage;
    QStringList recipientList = mimeInfo.to.split(";");
    MimeHtml mimeHtml;

    mimeMessage.setSender(stringToEmail(mimeInfo.mailAddr));
    mimeMessage.setSubject(mimeInfo.subject);
    for (int i = 0; i < recipientList.count(); ++i) {
        mimeMessage.addRecipient(stringToEmail(recipientList.at(i)));
    }

    if (!mimeInfo.cc.isEmpty()) {
        QStringList ccList = mimeInfo.cc.split(";");
        for (int i = 0; i < ccList.count(); ++i) {
            mimeMessage.addRecipient(stringToEmail(ccList.at(i)), MimeMessage::Cc);
        }
    }

    if (!mimeInfo.bcc.isEmpty()) {
        QStringList bccList = mimeInfo.bcc.split(";");
        for (int i = 0; i < bccList.count(); ++i) {
            mimeMessage.addRecipient(stringToEmail(bccList.at(i)), MimeMessage::Bcc);
        }
    }
    mimeHtml.setHtml(mimeInfo.content);
    mimeMessage.addPart(&mimeHtml);

    for (int i = 0; i < inlineList.count(); ++i) {
        MimeInlineFile *mimeInlineFile = new MimeInlineFile(new QFile(inlineList.at(i)));
        QFileInfo fileInfo(inlineList.at(i));

        mimeInlineFile->setContentId(fileInfo.baseName());
        mimeMessage.addPart(mimeInlineFile);
    }

    for (int i = 0; i < attachList.count(); ++i) {
        mimeMessage.addPart(new MimeAttachment(new QFile(attachList.at(i))));
    }

    saveMail(&mimeMessage);
    smtp.quit();
}

void MimeThread::run()
{
    prepareInline();
    getFlag();
    if (flag == 0)
        sendComMail();   /////
    else
        sendDifMail();
}
