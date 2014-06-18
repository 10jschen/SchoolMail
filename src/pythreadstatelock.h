#ifndef PYTHREADSTATELOCK_H
#define PYTHREADSTATELOCK_H

#include <Python.h>

class PyThreadStateLock
{
public:
    PyThreadStateLock();
    ~PyThreadStateLock();

private:
    PyGILState_STATE state;;

};

#endif // PYTHREADSTATELOCK_H
