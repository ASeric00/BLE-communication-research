# sending json rpc over serial communication

import sys
import serial
import time
 
port = '/dev/cu.usbmodem21201'
ser = serial.Serial(port, 115200)

# give the serial connection 2 seconds to settle
time.sleep(2)

ser.write('{"method": "init", "params": {} }')

time.sleep(1)

ser.write('{"method": "toggleLED", "params": {"status": true} }')
time.sleep(5)
ser.write('{"method": "toggleLED", "params": {"status": false} }')

# wait 2 seconds before closing the serial connection
time.sleep(2)
ser.close()