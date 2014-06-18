#ifndef POP3_H
#define POP3_H

#include <Python.h>
#include <QString>
#include <string>

using namespace std;

class POP3
{
public:
    POP3(PyObject *pClass = NULL, const string& mailAddr = "", const string& hostName = "", const string& user = "",
         const string& passwd = "", int isSSL = 0);
    ~POP3();

    void initPy();
    void setProperty(PyObject *pClass, const string& mailAddr, const string& hostName, const string& user,
                     const string& passwd, int isSSL);
    void getInstance();
    int login();
    void delMsg(int msgId);
    void popReset();
    void quit();
    int getMailCount();
    const QString& getUidl(int msgUidl);
    void downloadMsg();

protected:
    //PyObject *pModule;
    PyObject *pDict;
    PyObject *pClass;
    PyObject *pInstance;
    PyObject *pArg;
    PyObject *pRet;
    PyObject *pFunc;

    string mailAddr;
    string hostName;
    string user;
    string passwd;
    int     isSSL;
};

#endif // POP3_H
