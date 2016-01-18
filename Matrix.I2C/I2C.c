#include "Python.h"

#include <linux/i2c-dev.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

static PyObject *py_open_channel(PyObject *self, PyObject *args) 
{
    int fd = -1;
    char i2c_dev[32] = {0};
    int index;
    
    if (!PyArg_ParseTuple(args, "i", &index))
        return NULL;
    if (index < 0) {
        PyErr_SetString(PyExc_ValueError, "I2C channel must be int type and >=0");
        return NULL;
    }
    sprintf(i2c_dev, "/dev/i2c-%d", index);
    fd = open(i2c_dev, O_RDWR);
    if (fd < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to open I2C channel");
        return NULL;
    }
    return Py_BuildValue("i", fd);
}

static PyObject *py_set_slave(PyObject *self, PyObject *args) 
{
    int fd = -1;
    int slave = -1;
    
    if (!PyArg_ParseTuple(args, "ii", &fd, &slave))
        return NULL;
    if (slave < 0 || slave > 0x7f) {
        PyErr_SetString(PyExc_ValueError, "I2C slave address must be 7 bit");
        return NULL;
    }
    if (ioctl(fd, I2C_SLAVE, slave) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to ioctl I2C_SLAVE");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *py_set_timeout(PyObject *self, PyObject *args) 
{
    int fd = -1;
    int timeout = -1;
    
    if (!PyArg_ParseTuple(args, "ii", &fd, &timeout))
        return NULL;
    if (ioctl(fd, I2C_TIMEOUT, timeout) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail TO ioctl I2C_TIMEOUT");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *py_set_retries(PyObject *self, PyObject *args) 
{
    int fd = -1;
    int retries = -1;
    
    if (!PyArg_ParseTuple(args, "ii", &fd, &retries))
        return NULL;
    if (ioctl(fd, I2C_RETRIES, retries) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail ioctl I2C_RETRIES");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *py_read_byte(PyObject *self, PyObject *args) 
{
    int data = -1;
    int wait_ms = -1;
    int fd = -1;
    
    if (!PyArg_ParseTuple(args, "ii", &fd, &wait_ms))
        return NULL;
    ioctl(fd, BLKFLSBUF);
    usleep(1000 * wait_ms);
    if ((data = i2c_smbus_read_byte(fd)) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to i2c_smbus_read_byte");
        return NULL;
    }
    return Py_BuildValue("i", data);
}

static PyObject *py_write_byte(PyObject *self, PyObject *args) 
{
    int fd = -1;
    int data = -1;
    int wait_ms = -1;
    if (!PyArg_ParseTuple(args, "iii", &fd, &data, &wait_ms))
        return NULL;
    if (data < 0 || data > 0xff) {
        PyErr_SetString(PyExc_ValueError, "Data must be 8 bit");
        return NULL;
    }
    if (i2c_smbus_write_byte(fd, (unsigned char)data) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to i2c_smbus_write_byte");
        return NULL;
    }
    usleep(1000 * wait_ms);
    Py_RETURN_NONE;
}

static PyObject *py_read_bytefrom(PyObject *self, PyObject *args) 
{
    int fd = -1;
    int addr = -1;
    int data = -1;
    int wait_ms = -1;
    if (!PyArg_ParseTuple(args, "iii", &fd, &addr, &wait_ms))
         return NULL;
    if (addr < 0 || addr > 0xff) {
        PyErr_SetString(PyExc_ValueError, "Address must be 8 bit");
        return NULL;
    }
    ioctl(fd, BLKFLSBUF);
    if (i2c_smbus_write_byte(fd, (unsigned char)addr) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to i2c_smbus_write_byte");
        return NULL;
    }
    usleep(1000 * wait_ms);
    data = i2c_smbus_read_byte(fd);
    return Py_BuildValue("i", data);
    Py_RETURN_NONE;
}

static PyObject *py_write_byteto(PyObject *self, PyObject *args) 
{
    int addr = -1;
    int data = -1;
    int wait_ms = -1;
    int fd = -1;
    
    if (!PyArg_ParseTuple(args, "iiii", &fd, &addr, &data, &wait_ms))
         return NULL;
    if (data < 0 || data > 0xff || addr < 0 || addr > 0xff) {
        PyErr_SetString(PyExc_ValueError, "Address and data must be 8 bit");
        return NULL;
    }
    if (i2c_smbus_write_byte_data(fd, (unsigned char)addr, (unsigned char)data) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to i2c_smbus_write_byte_data");
        return NULL;
    }
    usleep(1000 * wait_ms);
    Py_RETURN_NONE;
}

static PyObject *py_close_channel(PyObject *self, PyObject *args) 
{
    int fd = -1;
    if (!PyArg_ParseTuple(args, "i", &fd))
         return NULL;
    close(fd);
    Py_RETURN_NONE;
}
PyDoc_STRVAR(module_doc, "provides I2C control function for Matrix accessory using Python");
PyMethodDef matrix_I2C_methods[] = {
        {"open_channel", py_open_channel, METH_VARARGS, "open i2c channel"},
        {"set_slave", py_set_slave, METH_VARARGS, "set i2c device slave address"},
        {"set_timeout", py_set_timeout, METH_VARARGS, "set i2c timeout"},
        {"set_retries", py_set_retries, METH_VARARGS, "set i2c retries"},
        {"read_byte", py_read_byte, METH_VARARGS, "i2c read byte"},
        {"write_byte", py_write_byte, METH_VARARGS, "i2c write byte"},
        {"read_bytefrom", py_read_bytefrom, METH_VARARGS, "i2c read byte from address"},
        {"write_byteto", py_write_byteto, METH_VARARGS, "i2c write byte to address"},
        {"close_channel", py_close_channel, METH_VARARGS, "close i2c channel"},
        {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initI2C(void)
{
    PyObject *m;

    if ((m = Py_InitModule3("Matrix.I2C", matrix_I2C_methods, module_doc)) == NULL)
        return;
    return; 
}
