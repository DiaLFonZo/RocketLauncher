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

const byte PIN_CLK = 8;   // define CLK pin (any digital pin)
const byte PIN_DIO = 9;   // define DIO pin (any digital pin)
SevenSegmentExtended    display(PIN_CLK, PIN_DIO);

#define FuseTIME      1500  //Fuse current duration in milliseconds.

#define Fuss     7          //Pin connected to the Fuse relay.
#define GoButt   6          //Pin connected to the GO button.
#define ArmButt  5          //Pin connected to the ARM button.
#define BuzzPin  4          //Connected to the Speaker.
#define SetPot   0          //Analog Pin connected to the Pot.


void setup(){
  pinMode(Fuss,OUTPUT);
  pinMode(ArmButt, INPUT);        // set "ARM" button pin to input
  pinMode(GoButt, INPUT);         // set "GO" button pin to input
  digitalWrite(Fuss,LOW);         //OPEN the fuse circuit.
  digitalWrite(ArmButt, HIGH);    // turn on pullup resistor 
  digitalWrite(GoButt, HIGH);     // turn on pullup resistor 

  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  delay(1000);                // wait 1000 ms
  display.clear();                      // clear the display
  
  delay(10);                      //Wait for Serial Display startup.

}

int  DownCntr;                    // down counter (1/10 Secs.)
int  Go=0;                        // Stopped

//================================================================
void loop(){
  if(!digitalRead(GoButt)||!digitalRead(ArmButt)){
    Go=0;                         //ABORT!!!
    tone(BuzzPin, 440, 1500);
    delay(1500);
  }

  if(Go==0){
    WaitARM();  
    WaitGO();
  }
  
  ShowTimer();
  
  if (DownCntr > 50){
      if (DownCntr % 10 ==0)tone(BuzzPin, 1000, 50);  //Tone every second.
     }
  else if (DownCntr % 2 ==0)tone(BuzzPin, 1000, 50);  //Tone every 1/5 second.

  if (DownCntr ==0){
    //------------------ ROCKET LAUNCH! --------------------
    tone(BuzzPin, 440, FuseTIME);  //Launch audible signal
    digitalWrite(Fuss,HIGH);       //CLOSE the fuse circuit
    delay(FuseTIME);
    digitalWrite(Fuss,LOW);        //OPEN the fuse circuit.
    //------------------------------------------------------
     Go=0;
    }
  while (millis()% 100);        //Wait until the next 1/10 of second.
  delay(50);
  DownCntr--;
}

//----------------------------------------
void WaitGO(){
  ShowTimer();
  while(digitalRead(GoButt));
  Go=1;
  delay(20);
  while(!digitalRead(GoButt));  //Debounce GO button.
}

//------------------------------------------------------
void ReadTimer(){
//  DownCntr = map(analogRead(SetPot), 0, 1023, 5, 60);
//  DownCntr*=10; 
 
  DownCntr = (10);
}
//------------------------------------------------------
void ShowTimer(){
  String seconds = String (DownCntr, DEC);
  while(seconds.length()<4)seconds= "00" + seconds;     //format to 4 numbers.
  display.printTime(0, 0, true);                         //Write to Display.
}

//------------------------------------------------------
void WaitARM(){
  while(digitalRead(ArmButt)==1){
     ReadTimer(); 
     display.clear();    //Turn Off Digits.
     delay(50);
     ReadTimer(); 
     ShowTimer();                   //Show Digits.
     delay(150);
  }

  Go=0;
  ShowTimer();
  tone(BuzzPin, 2000, 150);
  delay(200);
  tone(BuzzPin, 2000, 150);
  delay(200);
  tone(BuzzPin, 2000, 150);

  delay(20);
  while(!digitalRead(ArmButt));  //Debounce ARM button.

}