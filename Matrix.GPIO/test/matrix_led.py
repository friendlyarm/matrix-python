#!/usr/bin/env python

import time
import Matrix.GPIO as GPIO

led = 7
GPIO.setmode(GPIO.BOARD)
GPIO.setup(led, GPIO.OUT)
print("Light on")
GPIO.output(led, GPIO.HIGH)
time.sleep(1)
print("Light off")
GPIO.output(led, GPIO.LOW)
GPIO.cleanup()