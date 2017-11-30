# Pi_SD_SAVER  
  
### Description:  
    
The Raspberry PI will sometimes corrupt it's SD card when there is a power failure. The main purpose of the PI_SD_SAVER is to  prevent this by combining:  

* the Adafruit 1000C power booster which allows a smooth transition to a backup Lion battery 

* an atmel(microchip) Attiny85 which monitors the battery voltage level.  

### How it works:  
  
  
When the battery voltage level gets too low, because the battery is either normally discharging or not properly charging, the Attiny85 will ask the PI to perform a safe system shutdown and once done the Attiny85 will tell the 1000C to cut the power to the PI. If the external USB power is present at that time, the Attiny85 will enter a state where it will wait for the battery to be replaced but if this USB power is not present, the Attiny85 itself will be shut down. The battery drain in that state is about 25 uAmp, it would then take about 2.3 years to totally discharge an half full 1000 mah Lion battery... in other words, time enough to do something about it ...  

It might not look like an overkill to use a "meager" Attiny85 to correct a "mighty" Raspberry PI's flaw, but it is ! This little critter can do a lot more, like most of the things the Arduino does... same genes ! So we might as well do something else with it... consequently PI_SD_SAVER also provides a single push button which allows convenient and safe power-offs and powerd-ups of the PI.   
  
  
Inspired by :<a href="https://github.com/NeonHorizon/lipopi"> lipopi</a> and  <a href="https://github.com/craic/pi_power"> pi_power</a>   
  
many thanks !  

* * *  
  
### Requirements:  
  
* a Raspberry Pi (any model - see notes about the Raspbery Pi 3)  
* an Adafruit PowerBoost 1000C Charger  
* a Lion battery, I'm using a 1000 mAh from Costco.
* an Attiny85  
* the arduino IDE with the Attiny85 library installed, see: <a href="http://highlowtech.org/?p=1695"> High-Low tech</a> or <a href="https://github.com/damellis/attiny"> Github-Damellis/attiny</a>  
* an arduino UNO or equivalent as a programmer, see: <a href="http://highlowtech.org/?p=1706"> High-Low tech<a/> Note: the arduino UNO as an ISP is by far the simplest  
* a small protoboard, some resistors, diodes and leds, I'm blue ! The circuit is not critical and will work fine on a protoboard  
* a small perf. single side board, if you feel like soldering, get an 8 pins socket for the chip then. I use the friendlier three holes pads perf. board with bus traces, see: <a href="https://www.digikey.com/product-detail/en/chip-quik-inc/SBB1605-1/SBB1605-1-ND/5978253"> digikey</a>  
* having fun ! 
 
  


  

