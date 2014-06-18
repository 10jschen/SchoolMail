#include "mythread.h"
#include "../src/pythreadstatelock.h"
#include <QDebug>
#include <QMutexLocker>


MyThread::MyThread(int flag, QObject *parent) :
    QThread(parent)
{
    this->flag = flag;
}

void MyThread::setPop3Server(PyObject *pClass, const Pop3ServerInfo &pop3ServerInfo)
{
    this->pClass = pClass;
    this->pop3ServerInfo.mailAddr = pop3ServerInfo.mailAddr;
    this->pop3ServerInfo.passwd = pop3ServerInfo.passwd;
    this->pop3ServerInfo.pop3Server = pop3ServerInfo.pop3Server;
    this->pop3ServerInfo.pop3serverPort = pop3ServerInfo.pop3serverPort;
    this->pop3ServerInfo.pop3ServerState = pop3ServerInfo.pop3ServerState;
}

void MyThread::downloadMsg()
{
    POP3 *pop3Server = new POP3();
    qDebug() << pop3ServerInfo.mailAddr << pop3ServerInfo.passwd << pop3ServerInfo.pop3Server
             << pop3ServerInfo.pop3ServerState;

    pop3Server->setProperty(pClass, pop3ServerInfo.mailAddr.toStdString().c_str(), pop3ServerInfo.pop3Server.toStdString().c_str(),
                           pop3ServerInfo.mailAddr.toStdString().c_str(), pop3ServerInfo.passwd.toStdString().c_str(),
                           pop3ServerInfo.pop3ServerState);

    pop3Server->getInstance();

    int flag = pop3Server->login();
    qDebug() << "+++++++++ flag = " << flag << "+++++++++" << pop3ServerInfo.mailAddr;
    if (!flag) {
        qDebug() << "------------------login---------------------";
        return;
    }

    pop3Server->downloadMsg();
    qDebug() << "downloadMsg()" << pop3ServerInfo.pop3Server;
    delete pop3Server;
}

void MyThread::run()
{
    PyThreadStateLock PyThreadLock;

    if (flag == 0) {
        downloadMsg();
    }

    this->quit();
}
