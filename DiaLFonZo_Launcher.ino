/*RocketLauncher
ROCKET LAUNCHER CONTROLLER.
Arduining.com  05 Jan 2012 (hardware implemented version)

-A count-down TIMER is implemented.
-The potentiometer is used to set the TIMER.
-Two Push-Buttons: "ARM" to set the counter and "GO" to start the counter.
-Display: Sparkfun's Serial 4 Digit 7-Segment Display . COM-09766 (RED).
-NewSoftSerial library is used to avoid random commands to the display
 during program downloading.
*/
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"

#include <Encoder.h>
Encoder myEnc(6, 7);
long oldPosition  = -999;

const byte PIN_CLK = 2;   // define CLK pin (any digital pin)
const byte PIN_DIO = 3;   // define DIO pin (any digital pin)
SevenSegmentExtended    display(PIN_CLK, PIN_DIO);

#define FuseTIME      1500  //Fuse current duration in milliseconds.

#define TimeButt    4          //Pin connected to the ARM button.
#define LaunchButt  5          //Pin connected to the GO button.
#define LaunchRelay 8          //Pin connected to the Fuse relay.
#define Buzzer      9          //Pin Connected to the Speaker.
#define armState    10         //Pin Arming is present

void setup(){
  pinMode(LaunchRelay,OUTPUT);
  pinMode(TimeButt, INPUT);        // set "ARM" button pin to input
  pinMode(LaunchButt, INPUT);         // set "GO" button pin to input
  digitalWrite(LaunchRelay,LOW);         //OPEN the fuse circuit.
  digitalWrite(TimeButt, HIGH);    // turn on pullup resistor 
  digitalWrite(LaunchButt, HIGH);     // turn on pullup resistor 

  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  delay(1000);                // wait 1000 ms
  display.clear();                      // clear the display
  
  delay(10);                      //Wait for Serial Display startup.
  
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

int  DownCntr;                    // down counter (1/10 Secs.)
int  Go=0;                        // Stopped

//================================================================
void loop(){
  if(!digitalRead(LaunchButt)||!digitalRead(TimeButt)){
    Go=0;                         //ABORT!!!
    tone(Buzzer, 440, 1500);
    delay(1500);
  }

  if(Go==0){
    WaitTime();  
    WaitLaunch();
  }
  
  ShowTimer();
  
  if (DownCntr > 50){
      if (DownCntr % 10 ==0)tone(Buzzer, 1000, 50);  //Tone every second.
     }
  else if (DownCntr % 2 ==0)tone(Buzzer, 1000, 50);  //Tone every 1/5 second.

  if (DownCntr ==0){
    //------------------ ROCKET LAUNCH! --------------------
    tone(Buzzer, 440, FuseTIME);  //Launch audible signal
    digitalWrite(LaunchRelay,HIGH);       //CLOSE the fuse circuit
    delay(FuseTIME);
    digitalWrite(LaunchRelay,LOW);        //OPEN the fuse circuit.
    //------------------------------------------------------
     Go=0;
    }
  while (millis()% 100);        //Wait until the next 1/10 of second.
  delay(50);
  DownCntr--;  
}

//----------------------------------------
void WaitLaunch(){
  ShowTimer();
  while(digitalRead(LaunchButt)==1);
  Go=1;
  delay(20);
  while(!digitalRead(LaunchButt));  //Debounce GO button.
}

//------------------------------------------------------
void ReadTimer(){
  DownCntr = (10);
}
//------------------------------------------------------
void ShowTimer(){
  display.printTime(0, DownCntr, true);                         //Write to Display.
}

//------------------------------------------------------
void WaitTime(){

//  long newPosition = myEnc.read();
//  if (newPosition != oldPosition) {
//    oldPosition = newPosition;
//    Serial.println(newPosition);
//  }
  
  while(digitalRead(TimeButt)==1){
     display.clear();    //Turn Off Digits.
     delay(50);
     ReadTimer(); 
     ShowTimer();                   //Show Digits.
     delay(150);
  }

  Go=0;
  ShowTimer();
  tone(Buzzer, 2000, 150);
  delay(200);
  tone(Buzzer, 2000, 150);
  delay(200);
  tone(Buzzer, 2000, 150);

  delay(20);
  while(!digitalRead(TimeButt));  //Debounce ARM button.

}
