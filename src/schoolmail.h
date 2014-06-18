#ifndef SCHOOLMAIL_H
#define SCHOOLMAIL_H

#include <QMainWindow>
#include <QString>
#include <QByteArray>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QStringList>

#include "addmailboxdlg.h"
#include "addrbookdlg.h"
#include "addsignaturedlg.h"
#include "sendmessage.h"

#include "../src/share/utils.h"
#include "../src/share/db.h"
#include "../src/smtp/SmtpMime"
#include "../src/pop3/pop3.h"
#include "showmessage.h"
#include <QtConcurrentRun>
#include <QPoint>
#include <Python.h>

namespace Ui {
class SchoolMail;
}

class SchoolMail : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SchoolMail(QWidget *parent = 0);
    ~SchoolMail();
    
protected:

    void resizeEvent(QResizeEvent *);               //调整窗口大小
    void addMailBox(const QString& mailAddress);    //向邮箱列表添加邮箱
    void delMailBox(QString mailAddr);              //删除邮箱列表中选定的邮箱
    void addRowToMessageList(const QString& mailAddress); //向邮件列表添加邮件
    void addTabWindows(QString mailAddr, QString subject, QString sender, QString date);       //添加邮件查看窗口
    void showMailBoxList();                         //显示所有邮箱列表
    void downloadMsg(Pop3ServerInfo pop3ServerInfo); //下载邮件
    void initPop3py();

    void deleteDir(QString dirName);
    bool deleteFile(QString fileName);

private slots:
    void on_closeAction_triggered();
    void on_softwareAction_triggered();
    void on_qt_aboutAction_triggered();
    void on_newMessageAction_triggered();
    void on_addSignature_triggered();
    void on_addToAddrBookAction_triggered();
    void on_addMailBoxAction_triggered();
    void on_action_triggered();
    void showSelectedMail(QTreeWidgetItem *item, int col);
    void removeTabWindows(int index);
    void on_delMailBox_Action();
    void on_getMsg_Action();
    void on_showMouseRightButton(QPoint point);
    void on_showMsg(QModelIndex index);
    void on_showMsgListRightButton(QPoint point);
    void on_delMsg();

signals:
    void tabCloseRequested(int index);

private:
    Ui::SchoolMail *ui;

    QString selectMailAddr;

    PyObject *pModulePop3;
    PyObject *pDictPop3;
    PyObject *pClassPop3;

    PyObject *pModuleMime;
    PyObject *pDictMime;
    PyObject *pClassMime;
};

#endif // SCHOOLMAIL_H
