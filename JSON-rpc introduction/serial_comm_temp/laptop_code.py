# python script to retrieve sensors readings from arduino

import serial
import time

# Connect to Arduino Nano 33 BLE Sense
port = '/dev/ttyACM0'  # Update this with the correct port
ser = serial.Serial(port, 115200)

# Allow the serial connection to initialize
time.sleep(2)

# Initialize the system
ser.write(b'{"method": "init", "params": {} }\n')
time.sleep(1)

# Read temperature
ser.write(b'{"method": "readTemperature", "params": {} }\n')
time.sleep(1)
response = ser.readline()
print("Temperature Response:", response.decode())

# Read humidity (optional)
ser.write(b'{"method": "readHumidity", "params": {} }\n')
time.sleep(1)
response = ser.readline()
print("Humidity Response:", response.decode())

# Close the connection
ser.close()
