#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <Python.h>
#include "../src/pop3/pop3.h"
#include "../src/share/utils.h"
#include <QMutex>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(int flag, QObject *parent = 0);

    void run();
    void downloadMsg();
    void setPop3Server(PyObject *pClass, const Pop3ServerInfo &pop3ServerInfo);

private:
    PyObject *pClass;
    int      flag;      // 0--Pop3, 1--mime
    Pop3ServerInfo pop3ServerInfo;
    //QMutex   mutex;

signals:

public slots:

};

#endif // MYTHREAD_H
