#!/usr/bin/env python

import os
import sys
import time

def read_position():
    sys_dir = '/sys/bus/i2c/drivers/adxl34x'
    found = False
    try:
        for item in os.listdir(sys_dir):
            position = '%s/%s/position' % (sys_dir, item)
            if os.path.exists(position):
                found = True
                break;
        if not found:
            print 'error: adxl34x position not found' % channel
            return -1
    except OSError, e:
        print e
        return -1

    try:
        fd = open(position, 'r')
    except IOError, e:
        print 'error: fail to open %s' % position
        return -1
    else:
        value = fd.read()
        fd.close()
        return value        

def main():
    i = 10
    while i > 0:
        value = read_position()
        if value:
            print 'Get position: ', value,
        else:
            print 'Fail to get position'
            break
        time.sleep(1)
        i = i - 1
    
if __name__ == '__main__':
    main()