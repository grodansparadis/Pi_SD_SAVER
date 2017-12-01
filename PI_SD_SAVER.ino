/*


 Tuesday November 28, 2017

 

PI_SD_SAVER

 implements a state machine which will prevent the Raspberry PI SD card corruption when a power failure occurs.

 This version is for the Attiny85.

 This whole project only intends to protect the SD card from corruption because of power failure, nothing else. If you think about it, you will realize that 
 the only important parameter is the battery voltage, we don't care about the USB voltage (Vusb) beeing present or not.
 We simply watchout for the battery depletion, because if it is in good shape it will endure a power fail and give us enough time to shutdown the PI safely.
 I didn't test with super capacitors, but it should work fine.
 
 There are 2 separated states machines, one for the PI/1000C combo and a smaller one for the push button.

 Please be kind with my English, I usually speak french, but I guess am as bad writing French ... !
 On the other hand, you are allowed to laugh constructively at my code, I'm a bit rusty, I used to code in assembler, which gives you
 an idea of how old I am. So C++ is fairly new to me. 

*/


#include <avr/io.h>                       // allows register commands
//#include <SoftwareSerial.h>             // for debugging
//SoftwareSerial Serial(0,3);  //RX, TX

// system state machine states
                                          // The battery should be connected on the 1000C and plugging the USB power module will start everything, 
#define CHARGING       4                  // charging the battery, the PI is not running
#define START_PI       6                  // ok time to start it, the battery has reached a sufficient level
#define WAIT_START_PI  3                  // wait for the PI to start
#define PI_RUNNING     1                  // the PI is running
#define STOP_PI        5                  // can be caused by a push button or a terminal shutdown 
#define PI_OFF         2                  // the PI has stopped and is waiting to be restarted
#define WAIT_STOP_PI   7                  // wait for it to stop
#define KILL_ALL       8                  // something bad happened stop everything
                                          // in all blocking states the battery level is continously checked
// push button state machine states

#define BUTTON_UP          20             // no comments here ....
#define BUTTON_MAYBE_DOWN  21
#define DEBOUNCE           22
#define BUTTON_DOWN        23


// constants won't change. They're used here to
// set pin numbers:

const int ledPin1 =  3;           // status led which blinks a number of time equal to the system state, every two seconds on pin 3 (chip pin 2)
const int pi_off = 1;             // tells the PI to do a safe system shutdown with pin 1 (chip pin 6)
const int enb_pow = 2;            // enable the 1000C to deliver power to the PI with pin 2 (chip pin 7)
const int pi_on = 0;              // sense if the PI is running, monitoring the TX line of the PI with pin 0 (chip pin 5)

// Battery levels: the Attiny85 is powered either by the USB or the 1000C through diodes, whose voltage drop is ~ 0.7 volts
//                  in my case I get 4.46 volts for the USB and 4.42 volts for the 1000C after the diodes. 
//                  The analog reference voltage is then one of those two but the difference will not be significant.
//                  A single Lion battery is fully charges at 4.2 volts and discharged at 3.2 volts.
//                  I use 3.8 volts to shutdown, this leaves the battery slightly more than half full.
//                  

const int vbat_low = 217;         // (3.8 volts/4.46 or 4.42)*255 = 217.26 or 219.23
const int delta = 11;             // (0.2 volts/4.46 or 4.42)*255 = 11.43 or 11.53 
const int vbat_max_low = 184;     // (3.2 volts/4.46 ou 4.42)*255 = 182.95 or 184.61

// variables will change:
int buttonState = 20;             // State machine for the push button states, start with 20...
int sm = 4 ;                      // System state machine states... start with 4...
int transition = 0;               // transition up-down-up of the push boutton
int cnt = 0;                      // delays counter ...
int bcnt = 0;                     // push button debounce counter
int tgl = 0;                      // led toggle counter 
unsigned long lastmillis = millis(); // for debuging...
int vusb = 0;                     // USB voltage from 1000C ... not used 
int vbat = 0;                     // Lion battery voltage from 1000C
int pi_running = 0;               // = 1 when the PI is running


void setup() {
  
  pinMode(ledPin1, OUTPUT);       // initialize the LED pin as an output
  pinMode(pi_off, OUTPUT);        // Tell PI to shutdown... also turns the voltage setting blue led on
  pinMode(enb_pow, OUTPUT);       // enable power on 1000C
  pinMode(pi_on, INPUT);          // receive TXD du PI, 1= running
  
  // initialize controls
  
  digitalWrite(pi_off, LOW);
  digitalWrite(enb_pow, LOW);
  
  // and the counters...
   
  cnt = 0;
  tgl = 0;
}

void loop() {                      // the loop executes every 2 msec.
  pi_running = digitalRead(pi_on); // read the pi_on pin ... yeah yeah I could have used digitalRead(pi_on) directly, neater !
  vbat = Readanal(2);              // read the battery voltage on pin 2 (chip pin 3) with function Readanal(analog_pin_#), use vbat = analogRead(pin) on other arduino
                                   // 
                                   // The system state machine starts here...
//============================================================================================================================
//                                   
  switch (sm) {
    case CHARGING:                 // we start with sm = 4, 
      tgl++;                       // toggle the status led 4 times, the led will turn on/off at a 200 msec. rate,
      if(tgl == 100) {             // then stay off for 2 seconds, and start over. The whole loop execute every 2 msec... so 100 is 200 msec.
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 300) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 400) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 500) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 600) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 700) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 800) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl >= 1800) {            // delay about 2 seconds 
        tgl = 0;
      }
      if (vbat >= vbat_low + delta && pi_running == false) {  // the battery has to be 4.0 volts (3.8 + 0.2) and the PI not running
        sm = START_PI;                                        // before we start the PI, so we keep charging the battery
        cnt = 0;
        tgl = 0;
      }
     break;
    case START_PI:                    // *** sm = 6, we don't need to blink this it happens too fast
      digitalWrite(enb_pow, HIGH);    // enable power on 1000C
      transition = 0;                 // just in case.... will be set if the push button has been pressed and released
      cnt = 0;
      tgl = 0;
      sm = WAIT_START_PI;             // wait for the PI to start
     break;
    case WAIT_START_PI:               // *** sm = 3
      tgl++;                          // tgl 3 times...
      if(tgl == 100) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 300) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 400) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 500) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 600) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl >= 1600) {               // pause 2 seconds
        tgl = 0;
      }
      if(vbat <= vbat_low && vbat >= vbat_max_low) {  // make sure the battery is still ok
        cnt = 0;                      // this might happen if we are running on battery only, Vusb = 0 
        tgl = 0;
        sm = KILL_ALL;                // the PI never started and the battery is now too low
      }                               // if the PI does not start we wait indefinitely here... unless the battery gets depleted 
      if(pi_running) {                // the PI TX data line will become high when the PI starts and will stay high
        cnt++;                        // ok the PI has started
        if(cnt >= 7000) {             // we wait about 14 seconds more...
          cnt = 0;
          tgl = 0;
          sm = PI_RUNNING;            // before declaring Yeah ! the PI is running
        }
      }
     break;
    case PI_RUNNING:                  // *** sm = 1
      tgl++;                          // tgl led once every 2 sec. so one blink means it's good the PI is running
      if(tgl == 100) {                // this is why this state is 1, less distracting blinking
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl >= 1200) {               // 2 seconds pause
      tgl = 0;  
      }
      if(transition == 1) {           // The push button was pressed... someone wants to stop the PI
        cnt = 0;
        tgl = 0;
        sm = STOP_PI;
        }
      if(pi_running == 0) {           // the PI was killed by a terminal shutdown or by the optional header pushbutton...
        cnt++;                        // SEE NEXT LINES FOR REBOOTs
        if(cnt >= 5000) {             // wait 10 secs .... just to be sure it is really down and
          cnt = 0;                    // also allows us to stay in the same state PI_RUNNING if there was a terminal reboot of the PI 
          tgl = 0;                    // the PI reboot causes TX to go low for 1 or 2 seconds, so we won't see it and stay in PI_RUNNING
          transition = 0;             // if the reboot fails TX might still be high but the PI hangs, then disconnect the PI_SD_SAVER and debug the PI on it's own.
          sm = PI_OFF;                // well it's already off, turn the power down
        }  
      } else {
          cnt = 0;                    // if there was a reboot cnt might have reach 500 or 1000, 1 or 2 seconds
        }
      if(vbat <= vbat_low && vbat >= vbat_max_low) {  // always check our battery
        cnt = 0;
        tgl = 0;
        sm = STOP_PI;                 // make a safe PI shutdown, followed by a powerdown. 
      }
     break;
    case STOP_PI:                     // *** sm = 5
      tgl++;                          // tgl 5 times
      if(tgl == 100) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 300) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 400) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 500) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 600) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 700) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 800) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 900) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 1000) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl >= 2000) {                               // pause 2 seconds 
        tgl = 0;
      }                                               // Something went wrong if we get here...
      if(vbat <= vbat_low && vbat >= vbat_max_low) {  // always check the battery in case the PI never stops
        cnt = 0;                                      // the PI never did respond to the off command and the battery is getting low.
        tgl = 0;                                      // we'll take a chance of corrupting the SD card 
        sm = KILL_ALL;                                // by shutting down the power anyway... no choice here !
      }
      digitalWrite(pi_off, HIGH);       // tell the PI to stop, this will also turn on the voltage setting led
      if(pi_running == 0) {             // has the PI has stopped
        transition = 0;                 // we clear/consume this button transition just in case
        cnt++;
        if(cnt >= 5000) {               // wait another 10 sec.
          cnt = 0;
          tgl = 0;
          sm = PI_OFF;                  // the PI is off
          digitalWrite(pi_off, LOW);    // set it back to low duh !
        }
      }
     break;
    case PI_OFF:                        // *** sm = 2
      tgl++;                            //  tgl twice.. so toggling twice is good, we are in the stopped mode. 
      if(tgl == 100) {                  //  a push button press will restart the PI.
        digitalWrite(ledPin1, HIGH);    //  only 2 blinks here not to get too excited !
      }
      if(tgl == 200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 300) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 400) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl >= 1400) {
        tgl = 0;
      }
      digitalWrite(enb_pow, LOW);      // disable power to 1000C, if Vusb is zero here, everything will shut down, including the Attiny85 (suicide)
                                       // the whole system will restart when Vusb comes back on.
                                       //
      if(transition == 1) {            // we get out of this state if the push button get pressed
        transition = 0;                // consume this button transition
        cnt = 0;
        tgl = 0;
        sm = CHARGING;                 // will make sure the battery is properly charged and the PI is down.
      }
      if(vbat <= vbat_low && vbat >= vbat_max_low) {  // if the battery gets low here it's bad...
        sm = KILL_ALL;                                // we shut everything down.
        cnt = 0;
        tgl = 0;
      }
     break;
    case KILL_ALL:                   // *** sm = 7
       tgl++;                        // tgl 7 times ... we'll never see that but for the sake of consistancy ... :-) !
      if(tgl == 100) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 300) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 400) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 500) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 600) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 700) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 800) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 900) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 1000) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 1100) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 1200) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl == 1300) {
        digitalWrite(ledPin1, HIGH);
      }
      if(tgl == 1400) {
        digitalWrite(ledPin1, LOW);
      }
      if(tgl >= 2400) {
        tgl = 0;
      }
      digitalWrite(enb_pow, LOW);
      sm = KILL_ALL;
      cnt = 0;                       // force of habit :-)
      tgl = 0;
     break;
  }
// the system state machine ends here
//=================================================================================================================
//
// push button state machine begins here:
//
//******************************************************************************************************* 
                                             // Second state machine, a small one for the push button.                         
  switch (buttonState) {                     // It is less confusing this way !
    case BUTTON_UP:                          // *** ButtonState = 20 we start with BUTTON_UP state 20
      if(vbat == 0) {                        // I'm using the analog input vbat to read the push button, Vbat = 0 should never happen with a healty battery...
        bcnt = 0;                            // unless you ground the analog input with a push button :-) 
        buttonState = BUTTON_MAYBE_DOWN;
    }
     break;
    case BUTTON_MAYBE_DOWN:                  // *** buttonState = 21
      bcnt++;                                // performs a debouncing
      if(bcnt >= 25) {                       // wait, 50 msec should do it...
        bcnt = 0;
        if(vbat == 0) {                      // vbat is still zero... debounced, we got a valid button down
          buttonState = BUTTON_DOWN;         // this also means we have to hold the button down for more than 50 msec. ... easy !
        } else {
          buttonState = BUTTON_UP;           // we had a fluke the button is still up.
          }
      }
     break;
    case BUTTON_DOWN:                        // *** buttonState = 23  the button is down, wait for it to be released...
      if(vbat > 0) {                         // button was released... no debouncing since we open a contact
        transition = 1;                      // tell the other state machine we had a push button transition, up-down-up
        buttonState = BUTTON_UP;             // and the button is up
      }
     break; 
    }
//
// push button state machine end here
//*********************************************************************************************************    
    delay(1);                                // delay of 1 msec and the Readanal() function takes the other 1 msec for a total loop time of 2 msec.  well close enough !    
  }                                          // go back to main loop
//
//
//  Fast function to read the analog voltage on a Attiny85
//
int Readanal(byte n){          // a bit of magic for the Attiny85, this function is faster than analogRead(pin) 
                               // I borrowed this code directly from https://www.marcelpost.com/wiki/index.php/ATtiny85_ADC
ADMUX = 
            (1 << ADLAR) |     // left shift result, I'm using a sample resolution of 255 instead of 1023 for the analog precision
            (0 << REFS2) |     // the granularity will be 5 volts/255 = ~ 0.02 volts/step and it's easier to process using Marcel's own words
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC2 for input (PB4), MUX bit 3
            (0 << MUX2)  |     // use ADC2 for input (PB4), MUX bit 2
            (n & 3);           // n = 0(ADC0)|1(ADC1)|2(ADC2)|3(ADC3) for the Attiny85, mask n with 3 so we don't disturb other bits, a precaution.
            
ADCSRA = 
            (1 << ADEN)  |     // Enable ADC 
            (1 << ADPS2) |     // set prescaler to 64, bit 2 
            (1 << ADPS1) |     // set prescaler to 64, bit 1 
            (0 << ADPS0);      // set prescaler to 64, bit 0  

 // take 8 x 8-bit samples and calculate the average, the readings are fairly constant, and we could take only one sample
 // but it takes roughly 1 msec. for 8 samples which is fine to work with and it's easy to divide by 8. 
 
 int count = 0;
 int AD = 0; 
 int sample_loop;

 for (sample_loop=8; sample_loop > 0 ; sample_loop --)
 {

   ADCSRA |= (1 << ADSC);         // start ADC measurement
   while (ADCSRA & (1 << ADSC) ); // wait till conversion completes 
   AD = int(ADCH);
   count = count + AD;
 }
 return count>>3;                 // returns count divided by 8 .

}

