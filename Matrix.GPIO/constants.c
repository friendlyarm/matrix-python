#include "Python.h"
#include "libfahw-gpio.h"
#include "constants.h"

void define_constants(PyObject *module)
{
	high = Py_BuildValue("i", HIGH);
	PyModule_AddObject(module, "HIGH", high);

	low = Py_BuildValue("i", LOW);
	PyModule_AddObject(module, "LOW", low);

	output = Py_BuildValue("i", OUTPUT);
	PyModule_AddObject(module, "OUT", output);

	input = Py_BuildValue("i", INPUT);
	PyModule_AddObject(module, "IN", input);

	board = Py_BuildValue("i", BOARD);
	PyModule_AddObject(module, "BOARD", board);

	kernel = Py_BuildValue("i", KERNEL);
	PyModule_AddObject(module, "KERNEL", kernel);

}