#include "Python.h"
#include "common.h"
#include "constants.h"
#include "libfahw-gpio.h"

static int gpio_mode = MODE_UNKNOWN;
static int pinGPIO[GPIO_MAX_NUM+1] = {-1, -1, -1, 99, -1, 98, -1,  60, 117, -1, 113,
        61, 58, 62, -1, 63, 78,  -1,  59, 95, -1,
        96, 97, 93, 94, -1, 77, 103, 102, 72, -1,
        73, 92, 74, -1, 76, 71,  75, 162, -1, 163,
};
static int is_exported[GPIO_MAX_NUM+1];

static int chantoGPIO(int channel)
{
    int i;
    int gpio;

    if (gpio_mode == BOARD) {
        if (channel < GPIO_MIN_NUM || channel > GPIO_MAX_NUM)
            return -1;
        gpio = pinGPIO[channel];
    } else if (gpio_mode == KERNEL) {
        gpio = channel;
        for (i=GPIO_MIN_NUM; i<=GPIO_MAX_NUM; i++) {
            if (gpio == pinGPIO[i])
                break;
        }
        if (i > GPIO_MAX_NUM || gpio == -1) {
            return -1;
        }
    } else {
        return -1;
    }
    return gpio;
}

static int exportGPIOPin(int gpio) 
{
    return writeIntValueToFile("/sys/class/gpio/export", gpio);
}

static int unexportGPIOPin(int gpio) 
{
    return writeIntValueToFile("/sys/class/gpio/unexport", gpio);
}

static int getGPIOValue(int gpio) 
{
    char fileName[255] = {0};
    sprintf(fileName, "/sys/class/gpio/gpio%d/%s", gpio, "value");        
    return readIntValueFromFile(fileName);
}

static int setGPIOValue(int gpio, int value) 
{
    char fileName[255] = {0};
    sprintf(fileName, "/sys/class/gpio/gpio%d/%s", gpio, "value"); 
    return writeIntValueToFile(fileName, value);
}

static int setGPIODirection(int gpio, int direction) 
{
    char directionStr[10];
    char fileName[255] = {0};

    sprintf(fileName, "/sys/class/gpio/gpio%d/%s", gpio, "direction");     
    if (direction == INPUT) {
        strcpy(directionStr, "in");
    } else if (direction == OUTPUT) {
        strcpy(directionStr, "out");
    } else {
        PyErr_SetString(PyExc_ValueError, "Direction must be INPUT or OUTPUT");
        return -1;
    }
    return writeValueToFile(fileName, directionStr);
}

#if 0
static int getGPIODirection(int gpio) 
{
    char buf[255] = {0};
    int direction;
    int ret;
    char fileName[255] = {0};

    sprintf(fileName, "/sys/class/gpio/gpio%d/%s", gpio, "direction");    
    ret = readValueFromFile(fileName, buf, sizeof(buf)-1);
    if (ret >= 0) {
        if (strncasecmp(buf, "out", 3)==0) {
            direction = OUTPUT;
        } else if (strncasecmp(buf, "in", 2)==0) {
            direction = INPUT;
        } else {
            PyErr_SetString(PyExc_ValueError, "Direction must be INPUT or OUTPUT");
            return -1;
        }
        return direction;
    }
    return ret;
}
#endif

static PyObject *py_gpio_setmode(PyObject *self, PyObject *args)
{
    int new_mode;

    if (!PyArg_ParseTuple(args, "i", &new_mode))
        return NULL;
    gpio_mode = new_mode;
    if (gpio_mode != KERNEL && gpio_mode != BOARD) {
        PyErr_SetString(PyExc_ValueError, "Pin mode must be KERNEL or BOARD");
        return NULL;
    }
    Py_RETURN_NONE;    
}

static PyObject *py_gpio_getmode(PyObject *self, PyObject *args)
{
    PyObject *mode;
    mode = Py_BuildValue("i", gpio_mode);
    return mode;
}

static PyObject *py_gpio_setup(PyObject *self, PyObject *args)
{
    int channel = -1;
    int gpio = -1;
    int direction = -1;
    int i;

    if (!PyArg_ParseTuple(args, "ii", &channel, &direction)) {
        return NULL; 
    }
    if (direction != INPUT && direction != OUTPUT) {
        PyErr_SetString(PyExc_ValueError, "Pin direction must be IN or OUT");
        return NULL;
    }
    if ((gpio = chantoGPIO(channel)) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid Pin, it may be 5V 3.3V GND or occupied by kernel");
        return NULL;
    }
    if (exportGPIOPin(gpio) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to export gpio");
        return NULL;
    }
    for (i=GPIO_MIN_NUM; i<=GPIO_MAX_NUM; i++) {
        if (gpio == pinGPIO[i]) {
            is_exported[i] = 1;
            break;
        }
    }
    if (setGPIODirection(gpio, direction) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to set gpio direction");
        return NULL;
    }
    Py_RETURN_NONE; 
}

static PyObject *py_gpio_cleanup(PyObject *self, PyObject *args)
{
    int i;
    for (i=GPIO_MIN_NUM; i<=GPIO_MAX_NUM; i++) {
        if (is_exported[i] == 1) {
            unexportGPIOPin(pinGPIO[i]);
            is_exported[i] = 0;
        }
    }
    Py_RETURN_NONE;
}
static PyObject *py_gpio_output(PyObject *self, PyObject *args)
{
    int channel = -1;
    int gpio = -1;
    int value = -1;

    if (!PyArg_ParseTuple(args, "ii", &channel, &value))
        return NULL; 
    if ((gpio = chantoGPIO(channel)) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid Pin, it may be 5V 3.3V GND or occupied by kernel");
        return NULL;
    }
    if (setGPIOValue(gpio, value) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Fail to set gpio value");
        return NULL;
    }

    Py_RETURN_NONE; 
}

static PyObject *py_gpio_input(PyObject *self, PyObject *args)
{
    int channel = -1;
    int gpio = -1;
    PyObject *value = NULL;

    if (!PyArg_ParseTuple(args, "i", &channel))
        return NULL;
    if ((gpio = chantoGPIO(channel)) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid Pin, it may be 5V 3.3V GND or occupied by kernel");
        return NULL;
    }
    switch (getGPIOValue(gpio)) {
    case 0:
        value = Py_BuildValue("i", LOW);
        break;
    case 1:
        value = Py_BuildValue("i", HIGH);
        break;
    default:
        break;
    }
    return value;
}

PyDoc_STRVAR(module_doc, "provides GPIO function for Matrix accessory using Python");
static PyMethodDef matrix_gpio_methods[] = {
        {"setmode", py_gpio_setmode, METH_VARARGS, "Set pin index mode as sys index or board index"},
        {"getmode", py_gpio_getmode, METH_VARARGS, "Get pin index mode"},
        {"setup",   py_gpio_setup, METH_VARARGS, "Export gpio and set gpio direction as ouput or input"},
        {"cleanup", py_gpio_cleanup, METH_VARARGS, "Unexport gpio"},
        {"output",  py_gpio_output, METH_VARARGS, "Set gpio value as high or low"},
        {"input",   py_gpio_input, METH_VARARGS, "Get gpio value"},
        {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initGPIO(void)
{
    PyObject *m;

    if ((m = Py_InitModule3("Matrix.GPIO", matrix_gpio_methods, module_doc)) == NULL)
        return;
    define_constants(m);
    memset(is_exported, 0, sizeof(is_exported)/sizeof(is_exported[0]));
    return; 
}
