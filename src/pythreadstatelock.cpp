#include "pythreadstatelock.h"

PyThreadStateLock::PyThreadStateLock()
{
    state = PyGILState_Ensure();
}

PyThreadStateLock::~PyThreadStateLock()
{
    PyGILState_Release(state);
}
