#include "Python.h"
#include "common.h"

int writeValueToFile(char* fileName, char* buff) {
    FILE *fp = fopen(fileName,"w");
    if (fp == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Unable to open file");
        return -1;
    } else {
        fwrite ( buff, strlen(buff), 1, fp );
    }
    fclose(fp);
    return 0;
}

int writeIntValueToFile(char* fileName, int value) {
    char buff[50];
    sprintf(buff, "%d", value);
    return writeValueToFile(fileName, buff);
}

int readValueFromFile(char* fileName, char* buff, int len) {
    int ret = -1;
    FILE *fp = fopen(fileName,"r");
    if (fp == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Unable to open file");
        return -1;
    } else {
        if (fread(buff, sizeof(char), len, fp)>0) {
            ret = 0;
        }
    }
    fclose(fp);
    return ret;
}

int readIntValueFromFile(char* fileName) {
    char buff[255];
    memset(buff, 0, sizeof(buff));
    int ret = readValueFromFile(fileName, buff, sizeof(buff)-1);
    if (ret == 0) {
        return atoi(buff);
    }
    return ret;
}