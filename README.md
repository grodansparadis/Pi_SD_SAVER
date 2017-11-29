# Pi_SD_SAVER
The Raspberry PI will sometimes corrupt it's SD card when there is a power failure. The main purpose of the PI_SD_SAVER is to  prevent this by combining:  

* the Adafruit 1000C power booster which allows a smooth transition to a backup Lion battery, with an 

* atmel(microchip) Attiny85 which monitors the battery voltage level.  

When this level gets too low, because the battery is either normally discharging or not properly charging, the Attiny85 will ask the PI to perform a safe system shutdown and once done the Attiny85 will tell the 1000C to cut the power to the PI. If an external power USB voltage is present at that time, the Attinu85 will wait for the battery to be replaced but if this power USB voltage is not present, the Attiny85 itself will be shut down. The battery drainage in that state is about 25 uAmp, it would then take about 2.3 years to totally discharge an half full 1000 mah Lion battery... in other words, time enough to do something about it ...  

Against all appearances, if using a "meager" Attiny85 to correct a "mighty" Raspberry PI's flaw doesn't look like an overkill, then frankly it is ! This little critter can do a lot more, like most things the Arduino does. So we might as well do something else with it... consequently PI_SD_SAVER also provides a single push button which allows to make convenient and safe poweroffs and powerdowns of the PI.   
  
  
Inspired by :<a href="https://github.com/NeonHorizon/lipopi"> lipopi</a> and  <a href="https://github.com/craic/pi_power"> pi_power</a>   
  
many thanks !

