#include "showmessage.h"
#include "ui_showmessage.h"
#include "../src/share/utils.h"
#include "../src/share/db.h"
#include "../src/pythreadstatelock.h"
#include "../src/share/mime.h"
#include "../src/downloadattachdlg.h"
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

ShowMessage::ShowMessage(PyObject *pClass, QString mailAddr, QString subject,
                         QString sender, QString date, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowMessage)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);

    this->pClass = pClass;
    this->mailAddr = mailAddr;
    this->subject  = subject;
    this->sender   = sender;
    this->date     = date;
    this->attachList.clear();

    listMessage();
}

ShowMessage::~ShowMessage()
{
    delDir();
    delete ui;
}

void ShowMessage::delDir()
{
    QDir existDir;

    if (!existDir.exists("./schoolmail/tmp/" + this->msgUidl))
        return;

    QDir dir("./schoolmail/tmp/" + this->msgUidl);

    dir.setFilter(QDir::Files);

    for (int i = 0; i< dir.count(); i++)
        dir.remove(dir[i]);
    //dir.rmdir("./schoolmail/tmp/" + this->msgUidl);
    QDir().rmdir(dir.path());
}

void ShowMessage::buildDir()
{
    QDir dir;

    if (!dir.exists("./schoolmail/tmp/" + this->msgUidl))
        dir.mkdir("./schoolmail/tmp/" + this->msgUidl);
}

void ShowMessage::decodeMsg()
{
    PyThreadStateLock pyThreadStateLock;

    Mime *mime = new Mime(pClass, mailAddr.toStdString(), msgUidl.toStdString());
    mime->getInstance();
    mime->decodeMime();

    delete mime;
}

void ShowMessage::getAttachList()
{
    QFileInfo fileInfo("./schoolmail/tmp/" + msgUidl + "/attach.txt");

    if (!fileInfo.exists()) {
        qDebug() << "Doesn't have attach";
        return;
    }

    QFile file("./schoolmail/tmp/" + msgUidl + "/attach.txt");
    QString attachString;
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    in.setCodec("GBK");
    attachString = in.readAll();
    this->attachList.append(attachString.split(";"));
}

void ShowMessage::getTo()
{
    QFileInfo fileInfo("./schoolmail/tmp/" + msgUidl + "/to.txt");

    if (!fileInfo.exists()) {
        qDebug() << "Doesn't have to.txt";
        return;
    }

    QFile file("./schoolmail/tmp/" + msgUidl + "/to.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    in.setCodec("GBK");
    this->recipientTo = in.readAll();
}

void ShowMessage::getCc()
{
    QFileInfo fileInfo("./schoolmail/tmp/" + msgUidl + "/cc.txt");

    if (!fileInfo.exists()) {
        qDebug() << "Doesn't have cc.txt";
        return;
    }

    QFile file("./schoolmail/tmp/" + msgUidl + "/cc.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    in.setCodec("GBK");
    this->recipientCc = in.readAll();
}

void ShowMessage::listMessage()
{
    DB db;

    if (!db.openDB("./mail_db.db")) {
        qDebug() << "openDB打不开数据库!";
        QMessageBox::information(this, "提示", "数据库打开错误！");
        return;
    }

    qDebug() << "^^^^^^^^^^^^^";
    qDebug() << mailAddr << subject << date;
    qDebug() << "^^^^^^^^^^^^^";
    if (!db.getMsgUidl(mailAddr, subject, date, msgUidl)) {
        qDebug() << "listMessage db error!";
        return;
    }

    buildDir();
    decodeMsg();
    getAttachList();
    getTo();
    getCc();

    QFileInfo fileInfo("./schoolmail/tmp/" + msgUidl + "/html.txt");

    if (!fileInfo.exists()) {
        qDebug() << "Didn't exist html file";
        return;
    }

    QFile file("./schoolmail/tmp/" + msgUidl + "/html.txt");
    QString content;
    QByteArray data;
    file.open(QIODevice::ReadOnly);
//    QTextStream in(&file);
//    in.setCodec("UTF-8");
//    in.setAutoDetectUnicode(true);

//    content = in.readAll();
    data = file.readAll();
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    content = codec->toUnicode(data.constData(), data.size(), &state);
    if (state.invalidChars > 0) {
        qDebug() << "Not a valid UTF-8 sequence.";
        content = QTextCodec::codecForName("System")->toUnicode(data);
    }
    ui->ui_content->setHtml(content);
    ui->ui_sender->setText(sender);
    ui->ui_subject->setText(subject);
    ui->ui_reciver->setText(recipientTo);
    ui->ui_cc->setText(recipientCc);
}

void ShowMessage::on_ui_attachButton_clicked()
{
    QString path = "./schoolmail/tmp/" + msgUidl + "/";
    DownloadAttachDlg *downloadAttachDlg = new DownloadAttachDlg(path, attachList);

    downloadAttachDlg->show();
}
