#!/usr/bin/env python

import os
import sys
import time

def read_channel(channel):
    sys_dir = '/sys/bus/i2c/drivers/pcf8591'
    found = False
    try:
        for item in os.listdir(sys_dir):
            inx_input = '%s/%s/in%d_input' % (sys_dir, item, channel)
            if os.path.exists(inx_input):
                found = True
                break;
        if not found:
            print 'error: pcf8591 channel%d not found' % channel
            return -1
    except OSError, e:
        print e
        return -1

    try:
        fd = open(inx_input, 'r')
    except IOError, e:
        print 'error: fail to open %s' % inx_input
        return -1
    else:
        value = fd.read()
        fd.close()
        return int(value)        

def main():
    argc = len(sys.argv)
    if argc == 2:
        channel = int(sys.argv[1])
    else:
        channel = 0
    i = 10
    while i > 0:
        value = read_channel(channel)
        if value >= 0:
            print 'Channel%d value=%d' % (channel, value)
        else:
            print 'Fail to read Channel%d' % channel
            break
        time.sleep(1)
        i = i - 1
    
if __name__ == '__main__':
    main()