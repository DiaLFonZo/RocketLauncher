// Display
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
const byte PIN_CLK = 2;   // define CLK pin (any digital pin)
const byte PIN_DIO = 3;   // define DIO pin (any digital pin)
SevenSegmentExtended    display(PIN_CLK, PIN_DIO);

// Encoder
#include <Encoder.h>
Encoder myEnc(6, 7);
long oldPosition  = -999;
long newPosition;

// Timers
int sequenceTime;

unsigned long prevTimeMillis = 0;
unsigned long currentTimeMillis = 0;
unsigned long prevBuzzMillis = 0;
unsigned long currentBuzzMillis = 0;
unsigned long prevLaunchMillis = 0;
unsigned long currentLaunchMillis = 0;

// Pins
#define TimeButt    4          //Pin connected to the ARM button.
#define LaunchButt  5          //Pin connected to the GO button.
#define LaunchRelay 8          //Pin connected to the Fuse relay.
#define Buzzer      9          //Pin Connected to the Speaker.
#define ArmButt    10         //Pin Arming is present

void setup(){
  pinMode(LaunchRelay,OUTPUT);
  pinMode(TimeButt, INPUT);        // set "ARM" button pin to input
  pinMode(LaunchButt, INPUT);         // set "GO" button pin to input
  pinMode(ArmButt, INPUT);         // set "GO" button pin to input
  digitalWrite(LaunchRelay,LOW);         //OPEN the fuse circuit.
  digitalWrite(TimeButt, HIGH);    // turn on pullup resistor 
  digitalWrite(LaunchButt, HIGH);     // turn on pullup resistor 

  delay(1000);
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %              
  display.clear();                      // clear the display
  display.print("INIT");
  delay(1000);
  Serial.begin(9600);
}

int   menuLocation = 1;           // Sequence location
const long sysInterval = 1000;    // Base interval for the slowest buzzer

void loop() {
  switch (menuLocation) {
    case 1:
        while(digitalRead(TimeButt)==1){
          updateEncoder();
          updateTime();
          }
        menuLocation = 2;
        while(!digitalRead(TimeButt));
      break;
    case 2:
        updateTime();
        if(digitalRead(ArmButt)==1){
            updateBuzz();
            if(digitalRead(LaunchButt)==1) {
                countDown();
                menuLocation = 3;
            break;
            }
        }
        if(digitalRead(LaunchButt)==1) {
            tone(Buzzer, 300, 1000);
            delay(500);
          break;
        }
        if(digitalRead(TimeButt)==0) {
            tone(Buzzer, 300, 1000);
            delay(500);
            menuLocation = 1;
          break;
        }
      break;
    case 3:
          launch();
          menuLocation = 1;
        break;
      default:
        menuLocation = 1;
        break;
  }
}

void updateTime() {
  currentTimeMillis = millis();
  
  int num0 = (sequenceTime / 10) % 10;
  int num1 = (sequenceTime / 1) % 10;

  if (currentTimeMillis - prevTimeMillis >= sysInterval / 20) {
    display.setColonOn(true);
    display.setCursor(0, 0);
    display.print(num0);
    display.setCursor(0, 1);
    display.print(num1);
    display.setCursor(0, 2);
    display.print(0);
    display.setCursor(0, 3);
    display.print(0);
  }
  if (menuLocation == 1){
    if (currentTimeMillis - prevTimeMillis >= (sysInterval / 6)) {
      prevTimeMillis = currentTimeMillis;
      display.clear();
    } 
  }
}

void countDown() {
  while(sequenceTime > 0){
    tone(Buzzer, 1500, 50);
    updateTime();
    sequenceTime--;
    delay(1000);
  } 
  updateTime();
}
void launch() {
  digitalWrite(LaunchRelay, 1);
  tone(Buzzer, 1500, 4000);
  display.blink();
  delay(250);
  display.blink();
  delay(250);
  display.blink();
  delay(250);
  display.blink();
  digitalWrite(LaunchRelay, 0);
  myEnc.write(0);
  delay(500);
  display.clear(); 
  display.print("DONE");
  delay(5000);  
}

void updateEncoder() {
  newPosition = (myEnc.read() / 4);
  if (newPosition < 0){
    myEnc.write(0);
  }
  else if (newPosition > 60) {
    myEnc.write(60 * 4);
  }
  else {
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      sequenceTime = newPosition;
      tone(Buzzer, 1500, 15);
    }
  }
}

void updateBuzz() {
  currentBuzzMillis = millis();
  if (currentBuzzMillis - prevBuzzMillis >= (sysInterval / 4)) {
    tone(Buzzer, 1000, 500);
  }
  if (currentBuzzMillis - prevBuzzMillis >= (sysInterval / 2)) {
    prevBuzzMillis = currentBuzzMillis;
    tone(Buzzer, 1100, 500);
  }
}
