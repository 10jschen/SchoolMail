#include "schoolmail.h"
#include "ui_schoolmail.h"
#include "../src/showaddrbookdlg.h"
#include "../src/mythread.h"
#include "../src/pythreadstatelock.h"
#include "../src/showmessage.h"

#include <QStringList>
#include <QMessageBox>
#include <QFileDialog>
#include <QResizeEvent>
#include <QSize>
#include <QCheckBox>
#include <QString>
#include <QDebug>
#include <string>
#include <QMenu>
#include <QModelIndex>
#include <QAction>
#include <QDir>
#include <QFile>

using namespace std;


SchoolMail::SchoolMail(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SchoolMail)
{
    ui->setupUi(this);
    this->pModuleMime = NULL;
    this->pModulePop3 = NULL;
    this->pDictMime = NULL;
    this->pDictPop3 = NULL;
    this->pClassMime = NULL;
    this->pClassPop3 = NULL;
    connect(ui->ui_mailBoxList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(showSelectedMail(QTreeWidgetItem*,int)));
    connect(ui->ui_tabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(removeTabWindows(int)));
    connect(ui->ui_mailBoxList, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(on_showMouseRightButton(QPoint)));
    connect(ui->ui_messageList, SIGNAL(doubleClicked(QModelIndex)), this,
            SLOT(on_showMsg(QModelIndex)));
    connect(ui->ui_messageList, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(on_showMsgListRightButton(QPoint)));

    ui->ui_messageList->horizontalHeader()->setStretchLastSection(true);
    ui->ui_messageList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ui_messageList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ui_messageList->setSelectionMode(QAbstractItemView::SingleSelection);

    initPop3py();
    showMailBoxList();
}

SchoolMail::~SchoolMail()
{
    //::Py_Finalize();
    delete ui;
}

void
SchoolMail::initPop3py()
{
    if (!::Py_IsInitialized()) {
        qDebug() << "initialized";
        ::Py_Initialize();
    }
    ::PyRun_SimpleString("import sys; sys.path.append('.')");

    this->pModulePop3 = ::PyImport_ImportModule("pop3py");
    this->pModuleMime = ::PyImport_ImportModule("mime");

    if (this->pModulePop3 == NULL) {
        QMessageBox::information(NULL, "tip", "pModulePop3 is NULL!");
        return;
    }

    if (this->pModuleMime == NULL) {
        QMessageBox::information(NULL, "tip", "pModuleMime is NULL!");
        return;
    }

    this->pDictPop3 = ::PyModule_GetDict(this->pModulePop3);
    this->pDictMime = ::PyModule_GetDict(this->pModuleMime);
    if (this->pDictPop3 == NULL) {
        QMessageBox::information(this, "tip", "pDictPop3 is NULL!");
        return;
    }

    if (this->pDictMime == NULL) {
        QMessageBox::information(this, "tip", "pDictMime is NULL!");
        return;
    }

    this->pClassPop3 = ::PyDict_GetItemString(pDictPop3, "POP3Server");
    this->pClassMime = ::PyDict_GetItemString(pDictMime, "Mime");
    if (this->pClassPop3 == NULL) {
        QMessageBox::information(this, "tip", "pClassPop3 is NULL");
        return;
    }

    if (this->pClassMime == NULL) {
        QMessageBox::information(this, "tip", "pClassMime is NULL");
        return;
    }

    PyEval_InitThreads();
    PyEval_ReleaseThread(PyThreadState_Get());
}

//slots

void SchoolMail::on_showMsg(QModelIndex index)
{
    QString mailAddr = ui->ui_messageList->item(index.row(), 0)->text();
    QString subject  = ui->ui_messageList->item(index.row(), 1)->text();
    QString sender   = ui->ui_messageList->item(index.row(), 2)->text();
    QString date     = ui->ui_messageList->item(index.row(), 3)->text();

    this->addTabWindows(mailAddr, subject, sender, date);

}

qint64 string2datetime(QString s)
{
    QStringList li;
    QString month_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
                        "Aug", "Sep", "Oct", "Nov", "Dec"};
    int year, month, day;
    QTime t;
    //QString week_name[] = {"Mon,", "Tue,", "Wed,", "Thu,", "Fri,", "Sat,", "Sun,"};

    li = s.split(" ", QString::SkipEmptyParts);
    if(li.size() < 6)
        return 0;

    day = li.at(1).toInt();
    for(int i = 0; i < 12; ++i)
    {
        if(li.at(2) == month_name[i])
        {
            month = i + 1;
            break;
        }
    }
    year = li.at(3).toInt();
    t = QTime::fromString(li.at(4), "hh:mm:ss");

    int tz = li.at(5).toInt();
    qDebug() << tz;
    t = t.addSecs(3600 * (-tz / 100));
    return QDateTime(QDate(year, month, day), t, Qt::UTC).toMSecsSinceEpoch();
}

bool operator<(const MessageInfo &x, const MessageInfo &y)
{
    qint64 tx, ty;

    tx = string2datetime(x.date);
    ty = string2datetime(y.date);
    return tx > ty;
}

void
SchoolMail::showSelectedMail(QTreeWidgetItem *item, int col)
{
    QString mailAddr;
    int msg_state = 0;
    QList<MessageInfo> messageInfoList;
    DB db;

    qDebug() << item->text(0);
    if (!(item->text(0) == QString("收件箱") || item->text(0) == QString("送件箱")
        || item->text(0) == QString("草稿箱") || item->text(0) == QString("垃圾箱")))
        return;

    ui->ui_messageList->setRowCount(0);
    mailAddr = item->parent()->text(0);
    if (item->text(0) == QString("收件箱")) {
        msg_state = 0;
    } else if (item->text(0) == QString("送件箱")) {
        msg_state = 1;
    } else if (item->text(0) == QString("草稿箱")) {
        msg_state = 2;
    } else if (item->text(0) == QString("垃圾箱")) {
        msg_state = 3;
    }

    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "tip", "can't open mail_db.db");
        return;
    }

    if (!db.getMessageInfo(mailAddr, messageInfoList)) {
        qDebug() << "getMessageInfo error!";
        return;
    }

    qDebug() << "=================";
    qDebug() << mailAddr;
    qDebug() << msg_state;
    qDebug() << messageInfoList.count();
    qDebug() << "=================";

    qSort(messageInfoList);
    int count = 0;
    for (int i = 0; i < messageInfoList.count(); ++i) {
        if (messageInfoList.at(i).messageState == msg_state) {
            QTableWidgetItem *reciver = new QTableWidgetItem(messageInfoList.at(i).mailAddr);
            QTableWidgetItem *subject = new QTableWidgetItem(messageInfoList.at(i).subject);
            QTableWidgetItem *sender = new QTableWidgetItem(messageInfoList.at(i).sender);
            QTableWidgetItem *date = new QTableWidgetItem(messageInfoList.at(i).date);

            ui->ui_messageList->insertRow(count);

            ui->ui_messageList->setItem(count, 0, reciver);
            ui->ui_messageList->setItem(count, 1, subject);
            ui->ui_messageList->setItem(count, 2, sender);
            ui->ui_messageList->setItem(count, 3, date);
            count ++;

            qDebug() << messageInfoList.at(i).messageState;
            qDebug() << "==================";
        }
    }
}

void SchoolMail::on_showMouseRightButton(QPoint point)
{
    QMenu *qMenu = new QMenu(ui->ui_mailBoxList);
    QModelIndex indexSelect = ui->ui_mailBoxList->indexAt(point);

    QString str;
    str = indexSelect.data().toString();
    if(str.contains('@')) {
        this->selectMailAddr = str;

        QAction *getMsgAction = new QAction("获取邮件",this);
        QAction *delMailBoxAction = new QAction("删除邮箱",this);
        qMenu->addAction(getMsgAction);
        qMenu->addAction(delMailBoxAction);

        connect(getMsgAction, SIGNAL(triggered()), this, SLOT(on_getMsg_Action()));
        connect(delMailBoxAction, SIGNAL(triggered()), this, SLOT(on_delMailBox_Action()));
        qMenu->exec(QCursor::pos());
    }
}

void SchoolMail::on_showMsgListRightButton(QPoint point)
{
    QMenu *menu = new QMenu(ui->ui_messageList);
    QModelIndex indexSelect = ui->ui_messageList->indexAt(point);
    QString str = indexSelect.data().toString();
    QTableWidgetItem *item = ui->ui_messageList->itemAt(point);

    if (item == NULL)
        return;

    qDebug() << str;
    QAction *delMsgAction = new QAction("删除信息", this);
    menu->addAction(delMsgAction);
    connect(delMsgAction, SIGNAL(triggered()), this, SLOT(on_delMsg()));
    menu->exec(QCursor::pos());
}

void SchoolMail::on_delMsg()
{
    int index = ui->ui_messageList->currentRow();

    QString mailAddr = ui->ui_messageList->item(index, 0)->text();
    QString subject  = ui->ui_messageList->item(index, 1)->text();
    QString date     = ui->ui_messageList->item(index, 3)->text();
    int msg_state = 0;
    DB db;

    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "tip", "Can't open mail_db.db!");
        return;
    }

    if (!db.getMessageState(mailAddr, subject, date, msg_state)) {
        QMessageBox::information(this, "tip", "db fault!");
        return;
    }

    qDebug() << "del state before :" << msg_state << "del state end";

    if (msg_state == 0 || msg_state == 1 || msg_state == 2) {
        if (!db.modifyMessageState(mailAddr, subject, date, 3)) {
            QMessageBox::information(this, "tip", "msg_state update error!");
            return;
        }
    } else if (msg_state == 3) {
        QString msg_uidl;

        if (!db.getMsgUidl(mailAddr, subject, date, msg_uidl)) {
            //QMessageBox::information(this, "tip", "get uidl error!");
            qDebug() << "Get uidl error!";
            return;
        }

        if (!db.removeMsg(mailAddr, subject, date)) {
            QMessageBox::information(this, "tip", "msg delete error!");
            return;
        }
        qDebug() << "mailAddr = " << mailAddr << msg_uidl;
        this->deleteFile(QString("./schoolmail/mail/" + mailAddr + "/" + msg_uidl + ".eml"));
    }
    ui->ui_messageList->removeRow(index);
}

void SchoolMail::on_getMsg_Action()
{
    DB db;
    Pop3ServerInfo pop3ServerInfo;

    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "tip", "can't open mail_db.db");
        return;
    }

    if (!db.getPop3ServerInfo(this->selectMailAddr, pop3ServerInfo)) {
        QMessageBox::information(this, "tip", "db fault!");
        return;
    }

    downloadMsg(pop3ServerInfo);
}

bool SchoolMail::deleteFile(QString fileName)
{
    if (fileName.size() < 1)
        return true;
    QFile file(fileName);
    if (!file.exists())
        return true;
    return file.remove();
}

void SchoolMail::deleteDir(QString dirName)
{
    QDir directory(dirName);

    if (!directory.exists()) {
        return;
    }

    QStringList dirs = directory.entryList(QDir::Dirs | QDir::Hidden);
    QStringList files = directory.entryList(QDir::Files | QDir::Hidden);
    QList<QString>::iterator f = files.begin();
    QList<QString>::iterator d = dirs.begin();
    bool error = false;

    for (; f != files.end(); ++f) {
        if (!deleteFile(directory.path() + '/' + (*f))) {
            error = true;
        }
    }

    if(!QDir().rmdir(directory.path())) {
        error = true;
    }
}

void SchoolMail::on_delMailBox_Action()
{
    QTreeWidgetItem *item = ui->ui_mailBoxList->currentItem();

    if (item == NULL)
        return;

    QString mailAddr = item->text(0);
    int index = ui->ui_mailBoxList->indexOfTopLevelItem(item);
    item->takeChildren();
    ui->ui_mailBoxList->takeTopLevelItem(index);

    delMailBox(this->selectMailAddr);
    deleteDir(QString("./schoolmail/mail/" + mailAddr));
}

void
SchoolMail::removeTabWindows(int index)
{
    if (index == 0)
        return;

    ui->ui_tabWidget->removeTab(index);
}

////////////////

void
SchoolMail::resizeEvent(QResizeEvent *e)
{
    int with = e->size().width();

    ui->ui_mailBoxList->setMaximumWidth(with / 5);
}

void
SchoolMail::addMailBox(const QString &mailAddress)
{
    QTreeWidgetItem *mail = new QTreeWidgetItem(ui->ui_mailBoxList, QStringList(mailAddress));
    QTreeWidgetItem *receiveMailBox = new QTreeWidgetItem(mail, QStringList(trUtf8("收件箱")));
    QTreeWidgetItem *sendMailBox    = new QTreeWidgetItem(mail, QStringList(trUtf8("送件箱")));
    //QTreeWidgetItem *draftMailBox   = new QTreeWidgetItem(mail, QStringList(trUtf8("草稿箱")));
    QTreeWidgetItem *trashMailBox   = new QTreeWidgetItem(mail, QStringList(trUtf8("垃圾箱")));

    mail->addChild(receiveMailBox);
    mail->addChild(sendMailBox);
    //mail->addChild(draftMailBox);
    mail->addChild(trashMailBox);
}

void
SchoolMail::delMailBox(QString mailAddr)
{
    DB db;

    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "tip", "can't open mail_db.db");
        return;
    }

    if (!db.removeMailInfo(mailAddr)) {
        QMessageBox::information(this, "tip1", "删除成功！");
        return;
    }

    if (!db.removeMessageInfoByMailAddr(mailAddr)) {
        QMessageBox::information(this, "tip2", "删除成功！");
        return;
    }
}

void
SchoolMail::showMailBoxList()
{
    DB db;
    QList<Pop3ServerInfo> pop3ServerList;
    int mailBoxCount;

    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "tip", "can't open mail_db.db");
        return;
    }


    if (!db.getPop3ServerList(pop3ServerList))
        return;

    mailBoxCount = pop3ServerList.count();

    for(int i = 0; i < mailBoxCount; i++) {

        addMailBox(pop3ServerList.at(i).mailAddr);
        qDebug() << pop3ServerList.at(i).mailAddr;
    }
}

void
SchoolMail::addRowToMessageList(const QString &mailAddress)
{

}

void
SchoolMail::addTabWindows(QString mailAddr, QString subject, QString sender, QString date)
{
    ShowMessage *showMessage = new ShowMessage(pClassMime, mailAddr, subject, sender, date, this);

    ui->ui_tabWidget->addTab(showMessage, trUtf8(subject.toAscii()));
}

void
SchoolMail::on_action_triggered()
{
    QMessageBox::information(this, "提示", "发送邮件！");
}


void
SchoolMail::downloadMsg(Pop3ServerInfo pop3ServerInfo)
{
    MyThread *myThread = new MyThread(0);
    myThread->setPop3Server(this->pClassPop3, pop3ServerInfo);
    myThread->start();
}

void
SchoolMail::on_addMailBoxAction_triggered()
{
    ServerInfo serverInfo;

    AddMailBoxDlg *addMailBoxDlg = new AddMailBoxDlg(&serverInfo, NULL);

    if (addMailBoxDlg->exec() == QDialog::Rejected) {
        qDebug() << "addMailBoxDlg Rejected";
        return;
    }

    DB db;
    QString mailAddr;
    QString passwd;
    SmtpServerInfo smtpServerInfo;
    Pop3ServerInfo pop3ServerInfo;
    QDir    dir;

    mailAddr = serverInfo.user;
    passwd = serverInfo.passwd;
    smtpServerInfo.smtpServer = serverInfo.smtpServer;
    smtpServerInfo.smtpServerPort = serverInfo.smtpServerPort;
    smtpServerInfo.smtpServerState = serverInfo.serverState;
    pop3ServerInfo.pop3Server = serverInfo.pop3Server;
    pop3ServerInfo.pop3serverPort = serverInfo.pop3ServerPort;
    pop3ServerInfo.pop3ServerState = serverInfo.serverState;
    pop3ServerInfo.mailAddr = serverInfo.user;
    pop3ServerInfo.passwd = serverInfo.passwd;

    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "提示", "无法打开数据库!");
        qDebug() << "on_addMailBoxAction_triggered无法打开数据库!";
        return;
    }
    if (!db.addMailServerInfo(mailAddr, passwd, smtpServerInfo, pop3ServerInfo)) {
        qDebug() << "添加邮箱失败";
        return;
    }

    addMailBox(serverInfo.user);
    if (!dir.exists("./schoolmail/mail/" + serverInfo.user))
        dir.mkdir("./schoolmail/mail/" + serverInfo.user);
    downloadMsg(pop3ServerInfo);
}

void SchoolMail::on_addToAddrBookAction_triggered()
{
    ShowAddrBookDlg *showAddrBookDlg = new ShowAddrBookDlg(NULL);

    showAddrBookDlg->show();
}

void SchoolMail::on_addSignature_triggered()
{
    AddSignatureDlg *addSignatureDlg = new AddSignatureDlg(NULL);

    addSignatureDlg->show();
}

void SchoolMail::on_newMessageAction_triggered()
{
    SendMessage *sendMessage = new SendMessage();

    sendMessage->showMaximized();
}


void SchoolMail::on_qt_aboutAction_triggered()
{
    QMessageBox::aboutQt(NULL, "关于Qt");
}

void SchoolMail::on_softwareAction_triggered()
{
    QMessageBox msgBox;

    msgBox.setWindowTitle(QString("关于软件"));
    msgBox.setText(QString("校园邮件客户端v0.01"));
    msgBox.exec();
}

void SchoolMail::on_closeAction_triggered()
{
    this->close();
}
