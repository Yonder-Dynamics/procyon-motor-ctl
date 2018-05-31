#!/usr/bin/env python
from serial import Serial

with Serial('/dev/cu.wchusbserial1410',19200,timeout=1) as arduino:
    print(arduino.readline())
    