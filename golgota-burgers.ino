
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

/////////////////////////////////////////////////////////////////////////////////////////// your global variables start here
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

int selection = -1;        // no burger has been pressed beforehand aka burger1
int input_value = -1;      // no 2nd burger has been pressed yet aka burger2
boolean input = false;     // keeps track wether burger was pushed or not
boolean burgerpair[] = {0, 0, 0, 0, 0, 0}; // assign false value to burger pair 0...5







/////////////////////////////////////////////////////////////////////////////////////////// your global variables end here

void setup() {
  Serial.begin(57600);
  Serial.println("+++++ setup starts +++++"); // setup starts

  pinMode(A0, INPUT_PULLUP);                    // button is on pin A0 + avoid debouncing

  pinMode(LED_BUILTIN, OUTPUT);


  //while (!Serial) ; {} //uncomment when using the serial monitor
  Serial.println("Bare Conductive Touch MP3 player");

  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(4);      // allows to correct the sensitivity of the touch when pressed — en standard en 40, changé à 8
  MPR121.setReleaseThreshold(2);   // allows to correct the sensitivity of the touch when released — en standard en 20, changé à 4

  result = MP3player.begin();
  MP3player.setVolume(0, 0);      // instead of initial 10

  if (result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
  }

  Serial.println("+++++ setup ends +++++"); // setup ends


}

void loop() {

  intro();
  readTouchInputs();
  manageInputs();

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

    // check 1. sound played 2. if a burger has been pressed before 3. if the 2nd burger pressed is different from the 1st burger pressed
    Serial.print("burger ");
    Serial.print(input_value);
    Serial.println(" is touched");

    // check if the pair = 11
    if (input_value + selection == 11) {           // if the sum of the 2 burgers index is 11, then this is a pair
      Serial.println("You've got a pair, congrats!");    // print the successful message

      int burgerpairIndex;                        // burgerpairIndex will receive the value of selection or input depending on which one is the lowest

      if (input_value < selection) {
        burgerpairIndex = input_value;
      } else {
        burgerpairIndex = selection;
      }
      /*Serial.print("This is pair number ");
        Serial.println(burgerpairIndex);*/

      if (burgerpairIndex == 0) {
        // delay(300);
        MP3player.stopTrack();
        MP3player.playTrack(9);
        Serial.println("Burgerpair0 is matched, explanation is playing after 2nd burger pressed?");    // print the successful message
      } else if (burgerpairIndex == 1) {
        MP3player.stopTrack();
        MP3player.playTrack(9);
        Serial.println("Burgerpair1 is matched, explanation is playing after 2nd burger pressed?");    // print the successful message
      } else if (burgerpairIndex == 2) {
        // delay(300);
        MP3player.playTrack(15);
        Serial.println("Burgerpair2 is matched, explanation is playing after 2nd burger pressed?");    // print the successful message
      } else if (burgerpairIndex == 3) {
        // delay(300);
        MP3player.playTrack(15);
        Serial.println("Burgerpair3 is matched, explanation is playing after 2nd burger pressed?");    // print the successful message
      } else if (burgerpairIndex == 4) {
        // delay(300);
        MP3player.playTrack(15);
        Serial.println("Burgerpair4 is matched, explanation is playing after 2nd burger pressed?");    // print the successful message
      } else if (burgerpairIndex == 5) {
        // delay(300);
        MP3player.playTrack(15);
        Serial.println("Burgerpair5 is matched, explanation is playing after 2nd burger pressed?");    // print the successful message
      }

      if (burgerpair[burgerpairIndex] == false) { // if the pair hasn't been already selected:

        burgerpair[burgerpairIndex] = true;         // remember which pair was selected
        selection = -1;                           // reset the selection, so we can start again with a new pair later

        Serial.println("this is a new pair");

      } else {
        Serial.println("This is not a pair, booh!");
        selection = -1;                           // reset the selection, so we can start again with a new pair
      }
    }
}






void readTouchInputs() {
  input = false;
  if (MPR121.touchStatusChanged()) {

    MPR121.updateTouchData();

    // only make an action if we have one or fewer pins touched
    // ignore multiple touches

    if (MPR121.getNumTouches() <= 1) {
      for (int i = 0; i < 12; i++) { // Check which electrodes were pressed
        if (MPR121.isNewTouch(i)) {

          //pin i was just touched
          // Serial.print("pin ");
          // Serial.print(i);
          input_value = i;
          input = true;
          //Serial.println(" was just touched");
          digitalWrite(LED_BUILTIN, HIGH);

          if (i <= lastPin && i >= firstPin) {
            if (MP3player.isPlaying()) {
              if (lastPlayed == i && !REPLAY_MODE) {
                // if we're already playing the requested track, stop it
                // (but only if we're not in REPLAY_MODE)
                MP3player.stopTrack();
                //     Serial.print("stopping track ");
                //     Serial.println(i-firstPin);
              } else {
                // if we're already playing a different track (or we're in
                // REPLAY_MODE), stop and play the newly requested one
                MP3player.stopTrack();
                MP3player.playTrack(i - firstPin);
                //   Serial.print("playing track ");
                //   Serial.println(i-firstPin);

                // don't forget to update lastPlayed - without it we don't
                // have a history
                lastPlayed = i;
              }
            } else {
              // if we're playing nothing, play the requested track
              // and update lastplayed
              MP3player.playTrack(i - firstPin);
              // Serial.print("playing track ");
              // Serial.println(i-firstPin);
              lastPlayed = i;
            }
          }
        } else {
          if (MPR121.isNewRelease(i)) {
            /*  Serial.print("pin ");
              Serial.print(i);
              Serial.println(" is no longer being touched");
            */  digitalWrite(LED_BUILTIN, LOW);
          }
        }
      }
    }
  }
}
