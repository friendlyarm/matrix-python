#!/usr/bin/env python

import sys
import time
import Matrix.GPIO as GPIO

def main():
    argc = len(sys.argv)
    if argc != 2:
        print "usage:%s pin" % sys.argv[0]
        return
    pin = int(sys.argv[1])
    try:
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(pin, GPIO.IN)
        print "Get pin%d value %d" % (pin, GPIO.input(pin))
    except Exception, e:
        print e
    finally:
        GPIO.cleanup()

if __name__ == '__main__':
    main()