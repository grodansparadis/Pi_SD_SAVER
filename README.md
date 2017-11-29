# Pi_SD_SAVER  
  
### Description:  
    
The Raspberry PI will sometimes corrupt it's SD card when there is a power failure. The main purpose of the PI_SD_SAVER is to  prevent this by combining:  

* the Adafruit 1000C power booster which allows a smooth transition to a backup Lion battery, with an 

* atmel(microchip) Attiny85 which monitors the battery voltage level.  

### How it works:  
  
  
When this level gets too low, because the battery is either normally discharging or not properly charging, the Attiny85 will ask the PI to perform a safe system shutdown and once done the Attiny85 will tell the 1000C to cut the power to the PI. If an external power USB voltage is present at that time, the Attinu85 will wait for the battery to be replaced but if this power USB voltage is not present, the Attiny85 itself will be shut down. The battery drainage in that state is about 25 uAmp, it would then take about 2.3 years to totally discharge an half full 1000 mah Lion battery... in other words, time enough to do something about it ...  

At first glance, if using a "meager" Attiny85 to correct a "mighty" Raspberry PI's flaw doesn't look like an overkill, then, stand corrected because frankly it is ! This little critter can do a lot more, like most of the things the Arduino does... same genes ! So we might as well do something else with it... consequently PI_SD_SAVER also provides a single push button which allows convenient and safe poweroffs and powerdowns of the PI.   
  
  
Inspired by :<a href="https://github.com/NeonHorizon/lipopi"> lipopi</a> and  <a href="https://github.com/craic/pi_power"> pi_power</a>   
  
many thanks !  

* * *  
  
### Requirements:  
  
* A Raspberry Pi (any model - see notes about the Raspbery Pi 3) and an Adafruit PowerBoost 1000C Charger  
* an Attiny85  
* an arduino IDE with the Attiny85 library installed, see: <a href="https://www.google.ca/url?sa=t&rct=j&q=&esrc=s&source=web&cd=9&cad=rja&uact=8&ved=0ahUKEwja95L0zOTXAhUK74MKHWu8CDUQFghhMAg&url=https%3A%2F%2Fgithub.com%2Fdamellis%2Fattiny&usg=AOvVaw1qKS9_GT57_0IKVSfc5Ivs"> Github-Damellis/attiny</a>  
* an arduino UNO or equivalent as a programmer, see: <a href="http://highlowtech.org/?p=1706"> High-Low tech<a/>  
  


  

