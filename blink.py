#!/usr/bin/python
import os
import time  
import RPi.GPIO as GPIO

led = 13                 # Led on GPIO13 shows Python is running... ON = good !!! on PI header
button = 24              # Push Button GPIO24 on PI header
shutdown_pi = 16         # Shutdown PI from Arduino/Attiny85

GPIO.setmode(GPIO.BCM)  
GPIO.setup(led, GPIO.OUT)  
GPIO.setup(button, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.setup(shutdown_pi, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

def Shutdown(channel):    # callback for Push Button on PI header
  GPIO.output(led, True)  # twinkle Led ...
  time.sleep(0.2)
  GPIO.output(led, False)
  time.sleep(0.2)
  GPIO.output(led, True)
  time.sleep(0.2)
  GPIO.output(led, False)
  os.system("sudo shutdown -h now") # down we go 
# 
# Turning USB power on and off on the 1000C generates spikes on the shutdown_pi pin
# which cause the PI to shutdown randomly or prematurely. 
# Reading the pin again after the interrupt takes care of that ...
#

def Shutdown1(channel):       # callback from Attiny85/Arduino triggered by pin 16 HIGH
  time.sleep(0.2)             # a little debounce time 200 msec.
  if GPIO.input(shutdown_pi): # is it really HIGH ?
    GPIO.output(led, True)    # yeah !
    time.sleep(0.2)
    GPIO.output(led, False)
    time.sleep(0.2)
    GPIO.output(led, True)
    time.sleep(0.2)
    GPIO.output(led, False)
    os.system("sudo shutdown -h now")

GPIO.add_event_detect(24, GPIO.RISING, callback = Shutdown, bouncetime = 200)  # Header push button
GPIO.add_event_detect(16, GPIO.RISING, callback = Shutdown1, bouncetime = 200) # attiny_pin6->GPIO16

while True:                    # instead of doing nothing, find if Python is running
  found = False                # and if yes, turn ON the blue led on the header
  time.sleep(5)
  pids = [pid for pid in os.listdir('/proc') if pid.isdigit()]
  for pid in pids:
    try:
      cmd = open(os.path.join('/proc', pid, 'cmdline'), 'rb').read()
#      if "gimx" in cmd:       # well yes I took this code from the GIMX project...https://gimx.fr/wiki/index.php?title=RPi 
      if "python" in cmd:      # any program you like here 
        found = True
    except IOError: # proc has already terminated
      continue
  if found == True:
    GPIO.output(led, True)     # led ON
  else:
    GPIO.output(led, False)    # led OFF

