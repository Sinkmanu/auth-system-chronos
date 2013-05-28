#!/usr/bin/env python
import serial
import time

class watch():
        def __init__(self, dev = "/dev/ttyACM0", deb = 50):
                self.dev = dev
                self.deb = deb
		self.conn = serial.Serial(self.dev, 115200, timeout = 0)
	def start_simpliciti(self):
                self.write("\xFF\x07\x03")
        def close(self):
        #Close the serial port
               self.conn.close()
	def stop_simpliciti(self):
		self.write("\xFF\x09\x03")
	def stop_auth(self):
		self.write("\xFF\x76\x03")
	def flushInput(self):
		self.conn.flushInput()
	def flushOutput(self):
		self.conn.flushOutput()
        def write(self,msg):
                self.conn.write(msg)
		time.sleep(0.015)
        def read(self,len):
		return self.conn.read(len)
        def debounce(self):
                counter=0
                while True:
                        print ("%d: %s" % (counter, repr(s.read(12))))
                        counter += 1
	def send_command(self, data):
    		#Write to serial port
    		self.conn.write(data)
    		#Add a 15ms delay after every command sent
    		time.sleep(0.015)
	
