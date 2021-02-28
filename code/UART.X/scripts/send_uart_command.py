import serial
import time 

ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=1)  # open serial port

print(ser.name)         # check which port was really used

data = [0x02, 0x13, 0x03]
while True:
    ser.write(bytearray(data))     # write a string
    time.sleep(0.5)
ser.close()             # close port