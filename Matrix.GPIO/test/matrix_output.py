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
        GPIO.setup(pin, GPIO.OUT)
        print "Set pin%d HIGH" % pin
        GPIO.output(pin, GPIO.HIGH)
        time.sleep(1)
        print "Set pin%d LOW" % pin
        GPIO.output(pin, GPIO.LOW)
    except Exception, e:
        print e
    finally:
        GPIO.cleanup()

if __name__ == '__main__':
    main()