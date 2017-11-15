#include <avr/io.h>   // register commands
//#include <SoftwareSerial.h>
//SoftwareSerial Serial(0,3);  //RX, TX
// system states
#define CHARGING       0
#define START_PI       1
#define WAIT_START_PI  2
#define PI_RUNNING     3
#define PI_OFF         4
#define STOP_PI        5
#define WAIT_STOP_PI   6
#define KILL_ALL       7
#define BUTTON_UP          20
#define BUTTON_MAYBE_DOWN  21
#define DEBOUNCE           22
#define BUTTON_DOWN        23


// constants won't change. They're used here to
// set pin numbers:
//const int buttonPin = 12;     // the number of the pushbutton pin
//const int ledPin0 =  2;        // the number of the LED pin
const int ledPin1 =  3;
//const int ledPin2 =  4;
//const int ledPin3 =  5;
const int pi_off = 1;
const int enb_pow = 2;
const int pi_on = 0;
const int vbat_low = 175;       //700;  // 700/1023*5 = 3.4 volts
const int delta = 15;           //62;   // 62/1023*5 = 0.3 volts
const int vbat_max_low = 164;   //655;  // 3.2 volts

// variables will change:
int buttonState = 20;         // buttonState for button state machine
int sm = 0;                  // etat de la state machine...
//int button = 20;             // etat du boutton
int transition = 0;          // transition down-up du boutton
int cnt = 100;               // compteur de debounce...
int bcnt = 0;                // compteur du boutton
unsigned long lastmillis = millis();
int vusb = 0;
int vbat = 0;
int pi_running = 0;
//int pi_off = 0;
//int enb_pow = 0;


void setup() {
  // initialize the LED pin as an output:
  //pinMode(ledPin0, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  //pinMode(ledPin2, OUTPUT);
  //pinMode(ledPin3, OUTPUT);
 
  pinMode(pi_off, OUTPUT);       // Tell PI to shutdown...
  pinMode(enb_pow, OUTPUT);      // enable power to PI usb
  pinMode(pi_on, INPUT);    // TXD du PI, 1= running
  
  // initialize the pushbutton pin as an input:
  // pinMode(buttonPin, INPUT);
  
  // les LED off...
  /*digitalWrite(ledPin0, HIGH);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW); 
  */
  // initialise controls
  digitalWrite(pi_off, LOW);
  digitalWrite(enb_pow, LOW);
  
  // initialise serial communication:
  //Serial.begin(38400);
}

void loop() {
  // read the state of the pushbutton value:
  // buttonState = digitalRead(buttonPin);
  pi_running = digitalRead(pi_on);
  //vusb = analogRead(A0);
  vbat = Readanal(2);
  
  //Serial.print("sm=  ");
  //Serial.print(sm);
  //Serial.print(" pi_running=  ");
  //Serial.print(pi_running); 
  //Serial.print(" vusb= ");
  //Serial.print(vusb);
  //Serial.print(" vbat= ");
  //Serial.print(vbat);
  //Serial.print(" cnt= ");
  //Serial.print(cnt);
  //Serial.print(" buttonState ");
  //Serial.print(buttonState);
  //Serial.print(" trans= ");
  //Serial.print(transition);
  //Serial.print(" bcnt= ");
  //Serial.print(bcnt);
  //Serial.print("  ");
  
  switch (sm) {
    case CHARGING:         // *** sm = 0
      if (vbat >= vbat_low + delta && pi_running == false) {     // vbat > 3.7 volts
        sm = START_PI;
        cnt = 0;
      }
     break;
    case START_PI:         // *** sm = 1
      digitalWrite(enb_pow, HIGH);
      transition = 0;                 // just in case....
      cnt = 0;
      sm = WAIT_START_PI;
     break;
    case WAIT_START_PI:    // *** sm = 2
     // cnt++;
      if(pi_running) {
        cnt++;
        if(cnt >= 7000) {
          cnt = 0;
          sm = PI_RUNNING;
        }
      }
    /*  if(cnt >= 4000) {                 // wait for 4 secs.
    //    cnt = 0;
    //    sm = CHARGING;
    */  
     break;
    case PI_RUNNING:       // *** sm = 3
      cnt++;
      if(cnt == 500) {
        digitalWrite(ledPin1, HIGH);
      }
      if(cnt >= 1000) {
        cnt = 0;
        digitalWrite(ledPin1, LOW);
      }
      if(transition == 1) {              // wants to stop PI
        cnt = 0;
        sm = STOP_PI;
        }
      if(vbat <= vbat_low && vbat >= vbat_max_low) {
        cnt = 0;
        sm = STOP_PI;  
      }
     break;
    case STOP_PI:          // *** sm = 4
      digitalWrite(pi_off, HIGH);       // tell PI to stop
      if(pi_running == 0) {             // has PI stopped ?
        transition = 0;                 // we use this button transition
        cnt++;
        if(cnt >= 5000) {                 // wait another 10 sec.
          cnt = 0;
          sm = PI_OFF;
          digitalWrite(pi_off, LOW);
        }
      }
     break;
    case PI_OFF:           // *** sm = 5
      cnt++;
      if(cnt == 1500) {
        digitalWrite(ledPin1, HIGH);
      }
      if(cnt >= 3000) {
        cnt = 0;
        digitalWrite(ledPin1, LOW);
      }
      digitalWrite(enb_pow, LOW);      // disable power to 1000C
      if(transition == 1) {
        transition = 0;                 // consume this button transition
        sm = CHARGING;
      }
      if(vbat <= vbat_low && vbat >= vbat_max_low) {
        sm = KILL_ALL;
      }
     break;
    case KILL_ALL:         // *** sm = 7
      cnt++;
      if(cnt == 50) {
        digitalWrite(ledPin1, HIGH);
      }
      if(cnt >= 100) {
        cnt = 0;
        digitalWrite(ledPin1, LOW);
      }
      digitalWrite(enb_pow, LOW);
      sm = KILL_ALL;
     break;
  } 
  
  switch (buttonState) {
    case BUTTON_UP:         // *** ButtonState = 20
      if(vbat==0) {
        bcnt = 0;
        buttonState = BUTTON_MAYBE_DOWN;
    }
     break;
    case BUTTON_MAYBE_DOWN: // *** buttonState = 21
      bcnt++;
      if(bcnt >= 25) {
        bcnt = 0;
        if(vbat == 0) {                  // vbat still zero... debounced
          buttonState = BUTTON_DOWN;
        }
      }
     break;
    case BUTTON_DOWN:       // *** buttonState = 23
      if(vbat > 0) {                     // button released...
        transition = 1;                  // we had a transition
        buttonState = BUTTON_UP;
      }
     break; 
    }
    delay(1);
    //Serial.print(" millis= ");
    //Serial.println(millis() - lastmillis);
    lastmillis = millis();
    
  }

int Readanal(byte n){

ADMUX = 
            (1 << ADLAR) |     // left shift result
            (0 << REFS2) |
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC2 for input (PB4), MUX bit 3
            (0 << MUX2)  |     // use ADC2 for input (PB4), MUX bit 2
            (n & 3);           // n = 0(ADC0),1(ADC1),2(ADC2),3(ADC3)
            
ADCSRA = 
            (1 << ADEN)  |     // Enable ADC 
            (1 << ADPS2) |     // set prescaler to 64, bit 2 
            (1 << ADPS1) |     // set prescaler to 64, bit 1 
            (0 << ADPS0);      // set prescaler to 64, bit 0  

 // take 128x 8-bit samples and calculate average
 
 int count = 0;
 int AD = 0; 
 int sample_loop;

 for (sample_loop=8; sample_loop > 0 ; sample_loop --)
 {

   ADCSRA |= (1 << ADSC);         // start ADC measurement
   while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 
   AD = int(ADCH);
   count = count + AD;
 }
 return count>>3;

}

