#!/usr/bin/env python

import sys
import time
import math
import Matrix.I2C as I2C

WADDR = 0x3C
RADDR = 0x3D 
HMC5883_ADDRESS = 0x1E
def hmc5883_read(fd):
    try:
        I2C.write_byte(fd, RADDR, 6)                    # read command
        x1 = I2C.read_bytefrom(fd, 0x03, 6)
        x2 = I2C.read_bytefrom(fd, 0x04, 6)
        x = x1 << 8 | x2                                # Combine MSB and LSB of X Data output register
     
        z1 = I2C.read_bytefrom(fd, 0x05, 6)
        z2 = I2C.read_bytefrom(fd, 0x06, 6)
        z = z1 << 8 | z2                                # Combine MSB and LSB of Z Data output register  
     
        y1 = I2C.read_bytefrom(fd, 0x07, 6)
        y2 = I2C.read_bytefrom(fd, 0x08, 6)
    except IOError, e:
        print e
        return None
    y = y1 << 8 | y2                                    # Combine MSB and LSB of Y Data output register
    if x > 0x8000:
        x = -(0xFFFF - x + 1)
    if z > 0x8000:
        z = -(0xFFFF - z + 1)
    if y > 0x8000:
        y = -(0xFFFF - y + 1)
 
    angle = math.atan2(y, x) * (180 / 3.14159265) + 180
    if angle >= 0 and angle <= 360:
        return int(angle)
    else:
        return None

def hmc5883_init(index):
    fd = -1
    try:
        fd = I2C.open_channel(index)
    except Exception, e:
        print e
        return None
    try:
        I2C.set_slave(fd, HMC5883_ADDRESS)
        I2C.write_byte(fd, WADDR, 6)
        I2C.write_byteto(fd, 0x00, 0x70, 6)
        I2C.write_byte(fd, WADDR, 6)
        I2C.write_byteto(fd, 0x02, 0x00, 6)
        I2C.write_byte(fd, RADDR, 6)
    except Exception, e:
        print e
    time.sleep(0.1)
    return fd

def hmc5883_deinit(fd):
    I2C.close_channel(fd)

def main():    
    fd = hmc5883_init(0)
    if not fd:
        print "error: fail to init hmc5883"
        return
    i = 10
    while i > 0:
        angle = hmc5883_read(fd)
        if angle:
            print "The angle is:%d" % angle
        else:
            print "invalid angle"
        time.sleep(1)
        i = i - 1
    hmc5883_deinit(fd)

if __name__ == '__main__':
    main()
