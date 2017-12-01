# Pi_SD_SAVER  
  
### Description:  
    
The Raspberry PI will sometimes corrupt it's SD card when there is a power failure. The main purpose of the PI_SD_SAVER is to  prevent this by combining:  

* the Adafruit 1000C power booster which allows a smooth transition to a backup Lion battery 

* an atmel(microchip) Attiny85 which monitors the battery voltage level.  

### How it works:  
  
  
The most and only important parameter we have to maintain in order to achieve our goal, is the battery readiness to provide power instantaneously should the external power from a USB power module disappear. Accordingly, we only watch the battery.  
  
When the battery voltage level gets too low, because the battery is either normally discharging or not properly charging, the Attiny85 will ask the PI to perform a safe system shutdown and once done the Attiny85 will tell the 1000C to cut the power to the PI. If the external USB power is present at that time, the Attiny85 will enter a state where it will wait for the battery to be replaced but if this USB power is not present, the Attiny85 itself will be shut down. The battery drain in that state is about 25 uAmp, it would then take about 2.3 years to totally discharge an half full 1000 mah Lion battery... in other words, time enough to do something about it ...  

It might not look like an overkill to use a "meager" Attiny85 to correct a "mighty" Raspberry PI's flaw, but it is ! This little critter can do a lot more, like most of the things the Arduino does... same genes ! So we might as well do something else with it... consequently PI_SD_SAVER also provides a single push button which allows convenient and safe power-off(s) and power-on(s) of the PI.   
  
  
Inspired by :<a href="https://github.com/NeonHorizon/lipopi"> lipopi</a> and  <a href="https://github.com/craic/pi_power"> pi_power</a>   
  
many thanks Guys !  

* * *  
  
### Requirements:  
  
* a Raspberry Pi (any model - see notes about the Raspbery Pi 3)  
* an Adafruit PowerBoost <a href="https://learn.adafruit.com/adafruit-powerboost-1000c-load-share-usb-charge-boost/overview"> 1000C</a>  Charger
* a <a href="https://www.sparkfun.com/products/13813">Lion battery<a/>, I'm using a 1000 mAh from Costco.
* an <a href="http://www.microchip.com/wwwproducts/en/ATtiny85">Attiny85</a>  
* the <a href="https://www.arduino.cc/en/Main/Software">arduino IDE<a/> with the <a href="http://highlowtech.org/?p=1695">Attiny85 library</a> installed, or see: <a href="https://github.com/damellis/attiny"> Github-Damellis/attiny</a>  
* an arduino UNO or equivalent as a <a href="http://highlowtech.org/?p=1706">programmer</a>, Note: the arduino UNO as an ISP is by far the simplest  
* a small breadboard, some resistors, diodes and leds, I'm blue ! The circuit is not critical and will work fine on a breadboard  
* a small perf. single side <a href="https://makezine.com/2015/10/15/how-and-when-to-use-protoboard/">proto-board</a>, if you feel like soldering, get an 8 pins socket for the chip then. I use the friendlier <a href="https://www.digikey.com/product-detail/en/chip-quik-inc/SBB1605-1/SBB1605-1-ND/5978253"> three holes pads perf. board with bus traces</a>  
* having fun !  

  
 * * *
  
  ##### Note about Raspberry Pi 3:  
  
  in his readme <a href="https://github.com/NeonHorizon/lipopi"> lipopi</a> mentions that the PI3 uses the TX differently, I'm using Raspian Stretch and if in raspi-config you enable uart and disable console, the PI3 will use TX correctly for our project.  
    
### Software 
#### for the PI with Raspbian Stretch  
  
  1. run `sudo raspi-config` select `Interfacing Options`then `Serial` set `login shell to No` and `serial port to Yes`
  2. copy <a href="https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/blink.py">blink.py</a> in your PI home directory
  3. in the terminal type `crontab -e` and choose you favorite editor and add this line `@reboot python /home/pi/blink.py &` at the end of the file. Make sure that <a href="https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/blink.py">blink.py</a> is executable `ls -al blink.py` and if not `chmod 0755 blink.py` 
    
    
  #### for the Attiny85  
   
  1. copy <a href="https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/PI_SD_SAVER.ino">PI_SD_SAVER.ino</a> to your Arduino IDE and upload it to the Attiny85 using your favorite programmer.  
    
 ***  
   
### Hardware setup  
  
  1. use this <a href="https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/Pi_power_6_bb.png">breadboard</a> example
  2. I found out that these <a href="https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/button%26led_bb.png">addons</a> were very helpfull to see what the PI is doing. Mount them on the PI header. This push button only allows a shutdown of the PI. The blue led tells you Python is running.
  3. You can install a 100 kΩ resistor between the Attiny85 reset pin 1 to Vcc
  4. The best startup procedure goes like this:
     * connect your battery to the 1000C
     * connect the USB power cable to the PI
     * plug in the USB wall adaptor power module
     If everything goes according to the plan, no poufs no smokes, the status led should make 3 blinks every 2 seconds while waiting for the PI to start, the 1000C blue power led turns on, the PI starts, the header blue led will eventually turn on telling you Python is running and finally the status led will make one blink every 2 seconds... you deserve a beer !  
     
     Cheer !
     
       
    

  ![Image of breadboard](https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/Pi_power_6_bb.png)  
  ![Image of breadboard](https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/Pi_power_6_schem.png) 
  ![Image of breadboard](https://github.com/jeanrocco/Pi_SD_SAVER/blob/master/button%26led_bb.png) 

