//---- Related to the 7 segments display ---//
#include "SevenSegmentTM1637.h"                     //Display library (https://github.com/bremme/arduino-tm1637)
#include "SevenSegmentExtended.h"
const byte PIN_CLK = 2;                           // define CLK pin (any digital pin)
const byte PIN_DIO = 3;                           // define DIO pin (any digital pin)
SevenSegmentExtended display(PIN_CLK, PIN_DIO);   //Creating the display object

//---- Related to the rotary encoder ----//
#include <Encoder.h>                                //Encoder library used (https://github.com/PaulStoffregen/Encoder)
Encoder timeEncoder(9, 10);                         //Creating the encoder object
long oldPosition  = -999;                           //Variable to store the old encoder position
long newPosition  = 0;                              //Variable to store the new encoder position

//---- Related to the time ----//
int sequenceTime;                                   //Variable to store the sequence time set by the Encoder

//---- Variables for different Millis use case ----//
unsigned long prevTimeMillis = 0;
unsigned long currentTimeMillis = 0;
unsigned long prevBuzzMillis = 0;
unsigned long currentBuzzMillis = 0;

//---- Setting the Pins ----//
#define armingButton    4     //Pin Arming is present
#define launchButton    6     //Pin connected to the GO button.
#define timeButton      8     //Pin connected to the ARM button.
#define buzzerOutput    7     //Pin Connected to the Speaker.
#define relayOutput     11    //Pin connected to the Fuse relay.

void setup(){
  pinMode(relayOutput,OUTPUT);
  pinMode(timeButton, INPUT);        // set "ARM" button pin to input
  pinMode(launchButton, INPUT);         // set "GO" button pin to input
  pinMode(armingButton, INPUT);         // set "GO" button pin to input
  digitalWrite(relayOutput,LOW);         //OPEN the fuse circuit.
  digitalWrite(timeButton, HIGH);    // turn on pullup resistor 
  digitalWrite(launchButton, HIGH);     // turn on pullup resistor 

  delay(1000);
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %              
  display.clear();                      // clear the display
  display.print("INIT");
  delay(1000);
  Serial.begin(9600);
}

int   stateMachine = 1;           // Sequence location
const long sysInterval = 1000;    // Base interval for the slowest buzzerOutput

const char stateSetTime = 1;
const char stateLaunchWait = 2;
const char stateLaunch = 3;

void loop() {
  
  switch (stateMachine) {
    case stateSetTime:
        while(digitalRead(timeButton)==1){
          EncoderUpdate();
          updateTime();
          }
        stateMachine = stateLaunchWait;
        while(!digitalRead(timeButton));
        tone(buzzerOutput, 1500, 15);
        delay(250);
      break;
    case stateLaunchWait:
        updateTime();
        if(digitalRead(armingButton)==1){
            updateBuzz();
            if(digitalRead(launchButton)==1) {
                countDown();
                stateMachine = stateLaunch;
            break;
            }
        }
        if(digitalRead(launchButton)==1) {
            tone(buzzerOutput, 300, 500);
            delay(250);
          break;
        }
        if(digitalRead(timeButton)==0) {
            tone(buzzerOutput, 1500, 15);
            stateMachine = stateSetTime;
            delay(250);
          break;
        }
      break;
    case stateLaunch:
          launch();
          stateMachine = stateSetTime;
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
  if (stateMachine == 1){
    if (currentTimeMillis - prevTimeMillis >= (sysInterval / 6)) {
      prevTimeMillis = currentTimeMillis;
      display.clear();
    } 
  }
}

void countDown() {
  while(sequenceTime > 0){
    tone(buzzerOutput, 1500, 50);
    updateTime();
    sequenceTime--;
    delay(1000);
  } 
  updateTime();
}
void launch() {
    digitalWrite(relayOutput, 1);
    tone(buzzerOutput, 1500, 4000);
    display.blink();
    delay(250);
    display.blink();
    delay(250);
    display.blink();
    delay(250);
    display.blink();
    digitalWrite(relayOutput, 0);
    timeEncoder.write(0);
    delay(500);
    display.clear();
    display.setColonOn(false); 
    display.print("DONE");
    delay(5000);  
}

void EncoderUpdate() {
  newPosition = (timeEncoder.read() / 4);
  if (newPosition < 0){
    timeEncoder.write(0);
  }
  else if (newPosition > 60) {
    timeEncoder.write(60 * 4);
  }
  else {
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      sequenceTime = newPosition;
      tone(buzzerOutput, 1500, 15);
    }
  }
}

void updateBuzz() {
  currentBuzzMillis = millis();
  if (currentBuzzMillis - prevBuzzMillis >= (sysInterval / 4)) {
    tone(buzzerOutput, 1000, 500);
  }
  if (currentBuzzMillis - prevBuzzMillis >= (sysInterval / 2)) {
    prevBuzzMillis = currentBuzzMillis;
    tone(buzzerOutput, 1100, 500);
  }
}

void DigitalUpdate() {
  digitalRead(timeButton);
  
}
