#!/usr/bin/env python

import time
import Matrix.GPIO as GPIO

def main():
    try:
        led = 7
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(led, GPIO.OUT)
        print "Light on"
        GPIO.output(led, GPIO.HIGH)
        time.sleep(1)
        print "Light off"
        GPIO.output(led, GPIO.LOW)
    except Exception, e:
        print e
    finally:
        GPIO.cleanup()

if __name__ == '__main__':
    main()