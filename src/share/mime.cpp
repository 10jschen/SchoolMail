#include "mime.h"
#include <QDebug>

Mime::Mime(PyObject *pClass, const string &mailAddr, const string &msgUidl)
{
    this->pClass = pClass;
    this->mailAddr = mailAddr;
    this->msgUidl = msgUidl;
}

Mime::~Mime()
{
    Py_DECREF(this->pInstance);
}

void Mime::getInstance()
{
    if (pClass == NULL)
        return;

    PyObject *arg;
    try {
        arg = Py_BuildValue("(s, s)", mailAddr.c_str(), msgUidl.c_str());
        pInstance = PyInstance_New(pClass, arg, NULL);
    } catch(...) {
        qDebug() << "111111111#################111111111";
    }
}

void Mime::decodeMime()
{
    if (pInstance == NULL) {
        qDebug() << "pInstance is NULL";
        return;
    }

    char instance[] = "get_body";
    //PyObject_CallMethod(pInstance, "get_body", NULL, NULL);
    PyObject_CallMethod(pInstance, instance, NULL, NULL);
}
