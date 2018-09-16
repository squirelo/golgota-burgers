
// compiler error handling
#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// mp3 includes
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h>
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
byte result;
int lastPlayed = 0;

// mp3 behaviour defines
#define REPLAY_MODE TRUE  // By default, touching an electrode repeatedly will 
// play the track again from the start each time.
//
// If you set this to FALSE, repeatedly touching an
// electrode will stop the track if it is already
// playing, or play it from the start if it is not.

// touch behaviour definitions
#define firstPin 0
#define lastPin 11

// sd card instantiation
SdFat sd;

// global variables
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

int selection = -1;        // no burger has been pressed beforehand aka burger1
int input_value = -1;      // no 2nd burger has been pressed yet aka burger2
boolean input = false;     // keeps track wether burger was pushed or not
boolean burgerpair[] = {0, 0, 0, 0, 0, 0}; // assign false value to burger pair 0...5
int burgerpairIndex = -1;

// start setup
void setup() {
  Serial.begin(57600);
  Serial.println("+++++ setup starts +++++"); // setup starts

  pinMode(A0, INPUT_PULLUP);                    // button is on pin A0 + avoid debouncing

  pinMode(LED_BUILTIN, OUTPUT);

  // setup mp3

  //while (!Serial) ; {} //uncomment when using the serial monitor
  Serial.println("Bare Conductive Touch MP3 player");

  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(4);      // allows to correct the sensitivity of the touch when pressed — en standard en 40, changé à 4
  MPR121.setReleaseThreshold(2);   // allows to correct the sensitivity of the touch when released — en standard en 20, changé à 2

  result = MP3player.begin();
  MP3player.setVolume(0, 0);      // instead of initial 10

  if (result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
  }

  Serial.println("+++++ setup ends +++++"); // setup ends


}


// start loop
void loop() {

  intro();
  manageInputs();
  playSong();

}


void intro() {
  int sensorVal = digitalRead(A0);
  if (sensorVal != buttonState) {         // if the button state has changed, compare the current value of the button with the button state
    buttonState = sensorVal;
    if (sensorVal == LOW) {                // only reset the game if switch is pressed (LOW)
      Serial.println("+++++ reset button was pushed +++++");
      delay(25);
      MP3player.playTrack(16); // is supposed to play the audio explanation of the game
    }
  }
}


void manageInputs() {
  if (input == true && selection == -1) {     // check if 1. sound has been played when pressing on the burger 2. if no burger pressed before
    selection = input_value;                  // assign a new value to selection that is the id of the 2nd burger pressed
    Serial.print("burger ");
    Serial.print(input_value);
    Serial.println(" is touched");
  }

  if (input == true && selection > -1 && input_value != selection) {
    checkPair();

  }
}

void checkPair() {

  // check if the pair = 11
  if (input_value + selection == 11) {           // if the sum of the 2 burgers index is 11, then this is a pair
    Serial.println("You've got a pair, congrats!");    // print the successful message
    
    if (input_value < selection) {
      burgerpairIndex = input_value;
      selection = -1; 
    } 
    
    else {
      burgerpairIndex = selection;
      selection = -1; 
    }
  }
}

void playSong() {
  input = false;
  if (MPR121.touchStatusChanged()) {

    MPR121.updateTouchData();
    // only make an action if we have one or fewer pins touched
    // ignore multiple touches

    if (MPR121.getNumTouches() <= 1) {
      for (int i = 0; i < 12; i++) { // Check which electrodes were pressed
        if (MPR121.isNewTouch(i)) {
          
          input_value = i;
          input = true;
          Serial.print("burger ");
          Serial.print(input_value);
          Serial.println(" is touched");
          digitalWrite(LED_BUILTIN, HIGH);

          if (i <= lastPin && i >= firstPin) {
            
            if (MP3player.isPlaying() && burgerpairIndex == -1) {
              
              if (lastPlayed == i && !REPLAY_MODE) {
                // if we're already playing the requested track, stop it
                // (but only if we're not in REPLAY_MODE)
                MP3player.stopTrack();
              } 
              
              else {
                // if we're already playing a different track (or we're in
                // REPLAY_MODE), stop and play the newly requested one
                MP3player.stopTrack();
                MP3player.playTrack(i - firstPin);   
                lastPlayed = i;
              }
            } 

            else if (burgerpairIndex != -1) {  

              // success mp3 numbers 20, 21, 22, 23, 24, 25
              int p = 20 + burgerpairIndex;
              MP3player.stopTrack();             
              MP3player.playTrack(p);
              Serial.println( p + " just started");
              burgerpairIndex = -1;
            }
            
            else {  
              MP3player.playTrack(i - firstPin);
              lastPlayed = i;
            }
          }
        } else if (MPR121.isNewRelease(i)) {
           digitalWrite(LED_BUILTIN, LOW);
        }
      }
    }
  }
}
