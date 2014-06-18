#include "pop3.h"
#include <QMessageBox>
#include <QDebug>

POP3::POP3(PyObject *pClass, const string& mailAddr, const string& hostName, const string& user,
           const string& passwd, int isSSL)
{
    //initPy();
    this->pClass = pClass;
    this->mailAddr = mailAddr;
    this->hostName = hostName;
    this->user = user;
    this->passwd = passwd;
    this->isSSL = isSSL;
}

POP3::~POP3()
{
    Py_DECREF(this->pInstance);
}

void
POP3::initPy()
{
    if (!::Py_IsInitialized()) {
        qDebug() << "initialized";
        Py_Initialize();
    }
    char classstr[] = "POP3Server";
    pClass = PyDict_GetItemString(pDict, classstr);
}

void
POP3::setProperty(PyObject *pClass, const string &mailAddr, const string &hostName, const string &user,
                  const string &passwd, int isSSL)
{
    this->pClass = pClass;
    this->mailAddr = mailAddr;
    this->hostName = hostName;
    this->user = user;
    this->passwd = passwd;
    this->isSSL = isSSL;
}

void
POP3::getInstance()
{
    PyObject *arg;

    arg = Py_BuildValue("(s, s, s, s, i)", mailAddr.c_str(), hostName.c_str(),
                        user.c_str(), passwd.c_str(), isSSL);
    pInstance = PyInstance_New(pClass, arg, NULL);
}

int
POP3::login()
{
    PyObject *ret;
    int retValue;
    char instance[] = "login";

    ret = PyObject_CallMethod(pInstance, instance, NULL, NULL);
    PyArg_Parse(ret, "i", &retValue);

    return retValue;
}

void
POP3::delMsg(int msgId)
{
    char instance[] = "del_msg";
    char arg[] = "(i)";
    PyObject_CallMethod(pInstance, instance, arg, msgId);
}

void
POP3::popReset()
{
    char instance[] = "mail_reset";
    PyObject_CallMethod(pInstance, instance, NULL, NULL);
}

void
POP3::quit()
{
    char instance[] = "quit";
    PyObject_CallMethod(pInstance, instance, NULL, NULL);
}

int
POP3::getMailCount()
{
    PyObject *ret;
    int      mailCount;
    char instance[] = "get_mail_count";

    ret = PyObject_CallMethod(pInstance, instance, NULL, NULL);
    PyArg_Parse(ret, "i", &mailCount);

    return mailCount;
}

const QString&
POP3::getUidl(int msgUidl)
{
    PyObject *ret;
    char     *uidl = NULL;
    char instance[] = "get_uidl";
    char arg[] = "(i)";

    //ret = PyObject_CallMethod(pInstance, "get_uidl", "(i)", msgUidl);
    ret = PyObject_CallMethod(pInstance, instance, arg, msgUidl);
    PyArg_Parse(ret, "s", uidl);

    return QString(uidl);
}

void
POP3::downloadMsg()
{
    char instance[] = "download_msg";
    //PyObject_CallMethod(pInstance, "download_msg", NULL, NULL);
    PyObject_CallMethod(pInstance, instance, NULL, NULL);
}
