#ifndef MIME_H
#define MIME_H

#include <Python.h>
#include <QString>
#include <string>

using namespace std;

class Mime
{
public:
    Mime(PyObject *pClass, const string &mailAddr, const string &msgUidl);
    ~Mime();

    void getInstance();
    void decodeMime();

private:
    PyObject *pClass;
    PyObject *pInstance;

    string mailAddr;
    string msgUidl;
};

#endif // MIME_H
