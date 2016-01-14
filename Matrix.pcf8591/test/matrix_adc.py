#!/usr/bin/env python

import sys
import time
import Matrix.pcf8591 as pcf8591

def main():
    i = 10
    channel = 0
    while i > 0:
        try:
            value = pcf8591.read_channel(channel)
            print "Channel%d value=%d" % (channel, value)
        except Exception, e:
            print e
            return
        time.sleep(1)
        i = i -1

if __name__ == '__main__':
    main()