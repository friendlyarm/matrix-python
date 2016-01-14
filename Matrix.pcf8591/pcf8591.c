#include "Python.h"
#include "common.h"
#include "libfahw-pcf8591.h"

#include <sys/types.h>
#include <dirent.h>

static int pcf8591Read(int channel, int *value)
{
    int existFlag = 0;
    DIR *d;
    struct dirent *de;
    char pcfFile[FILE_PATH_LENGTH];

    if (!(d = opendir(PCF8591_SYS_PATH))) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to opendir pcf8591 directory");
        return -1;
    }
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.')
            continue;

        sprintf(pcfFile, "%s%s/in%d_input", PCF8591_SYS_PATH, de->d_name, channel);
        if (access(pcfFile, F_OK) != -1) {
            existFlag = 1;
            break;
        }
    }
    closedir(d);

    if (existFlag == 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to access pcf8591 sys file");
        return -1;
    }
    if ((*value = readIntValueFromFile(pcfFile)) != -1) {
        return 0;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Invalid pcf8591 data");
        return -1;
    }
}

static PyObject *py_pcf8591_read(PyObject *self, PyObject *args)
{
    int data = -1;
    int channel = -1;
    PyObject *value = NULL;

    if (!PyArg_ParseTuple(args, "i", &channel))
        return NULL;
    if (channel < 0 || channel > 3) {
        PyErr_SetString(PyExc_ValueError, "Pcf8591 only has channel 0 1 2 3");
        return NULL;
    }
    if (pcf8591Read(channel, &data) == 0) {
        value = Py_BuildValue("i", data);
        return value;
    } else {
        return NULL;
    }
}

PyDoc_STRVAR(module_doc, "provides pcf8591 control function for Matrix accessory using Python");
PyMethodDef matrix_pcf8591_methods[] = {
        {"read_channel", py_pcf8591_read, METH_VARARGS, "Read pcf8591"},
        {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initpcf8591(void)
{
    PyObject *m;

    if ((m = Py_InitModule3("Matrix.pcf8591", matrix_pcf8591_methods, module_doc)) == NULL)
        return;
    return; 
}
