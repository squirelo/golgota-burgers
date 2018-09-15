
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
int score = 100;           // Everyone got assigned a score value of 100
boolean input = false;     // keeps track wether burger was pushed or not
boolean burgerpair[] = {0, 0, 0, 0, 0, 0}; // assign false value to burger pair 0...5
int totalpairs = 0;







/////////////////////////////////////////////////////////////////////////////////////////// your global variables end here

void setup() {
  Serial.begin(57600);
  Serial.println("+++++ setup starts +++++"); // setup starts

  ////////////////////////////////////////////// BUTTON TO RESET GAME BEGIN
  pinMode(A0, INPUT_PULLUP);                    // button is on pin A0 + avoid debouncing


  ////////////////////////////////////////////// BUTTON TO RESET GAME ENDS

  pinMode(LED_BUILTIN, OUTPUT);


  //while (!Serial) ; {} //uncomment when using the serial monitor
  Serial.println("Bare Conductive Touch MP3 player");

  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(4);      // en standard en 40, changé à 8
  MPR121.setReleaseThreshold(2);   // en standard en 20, changé à 4

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
  readTouchInputs();
  /////////////////////////////////////////////////////////////////////////////////////////// your program starts here
  /* if (input) {
     Serial.print("burger ");
     Serial.print(input_value);
     Serial.println(" is touched");
    } */

  ///////////////


  ////////////////////////////////////////////// BUTTON TO RESET GAME BEGIN
  int sensorVal = digitalRead(A0);            // read and remember whatever info (whether pressed or not) coming from the button, will be either 1 or 0


  // the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed.
  // when the button's pressed, the game should be reset to start again
  // Turn on pin 13 when the button's pressed (LOW), and off when it's not:


  if (sensorVal != buttonState) {         // if the button state has changed, compare the current value of the button with the button state
    buttonState = sensorVal;
    if (sensorVal == LOW) {                // only reset the game if switch is pressed (LOW)
      Serial.println("+++++ reset button was pushed +++++");
      delay(25);
      MP3player.playTrack(16); // is supposed to play the audio explanation of the game
    }
  }



  ////////////////////////////////////////////// BUTTON TO RESET GAME ENDS





  if (input == true && selection == -1) {     // check if 1. sound has been played when pressing on the burger 2. if no burger pressed before
    selection = input_value;                  // assign a new value to selection that is the id of the 2nd burger pressed
    Serial.print("burger ");
    Serial.print(input_value);
    Serial.println(" is touched");
  }

  if (input == true && selection > -1 && input_value != selection) { // check 1. sound played 2. if a burger has been pressed before 3. if the 2nd burger pressed is different from the 1st burger pressed
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
          delay(300);
          MP3player.playTrack(17);
          Serial.println("Is playing explanation after the 2nd burger is pressed");    // print the successful message
        } else if (burgerpairIndex == 1) {
          delay(300);
          MP3player.playTrack(18);
          Serial.println("Is playing explanation after the 2nd burger is presssed");    // print the successful message
        } else if (burgerpairIndex == 2) {
          delay(300);
          MP3player.playTrack(19);
          Serial.println("Is playing explanation after the 2nd burger is presssed");    // print the successful message
        } else if (burgerpairIndex == 3) {
          delay(300);
          MP3player.playTrack(20);
          Serial.println("Is playing explanation after the 2nd burger is presssed");    // print the successful message
        } else if (burgerpairIndex == 4) {
          delay(300);
          MP3player.playTrack(21);
          Serial.println("Is playing explanation after the 2nd burger is presssed");    // print the successful message
        } else if (burgerpairIndex == 5) {
          delay(300);
          MP3player.playTrack(22);
          }


        
      if (burgerpair[burgerpairIndex] == false) { // if the pair hasn't been already selected:

        burgerpair[burgerpairIndex] = true;         // remember which pair was selected
        score = score + 1;                        // add 1 point to the score
        selection = -1;                           // reset the selection, so we can start again with a new pair later

        Serial.println("this is a new pair");
        Serial.println("You win 1 point");


                  // MP3player.playTrack(12);

                  totalpairs = 0;                             // reset variable to add up all pairs which were found so far
                  for (int i = 0; i < 6; i++) {               // count with i from 0 to 5
                    totalpairs += burgerpair[i];              // add array values
                  }

                } else {
                  Serial.println("But unfortunately this is a pair you already found");
                  Serial.println("You loose 1 point");
                  //delay(10050);
                  //MP3player.playTrack(15);
                  score = score - 1;                        // deduct 1 point from the score
                  selection = -1;                           // reset the selection, so we can start again with a new pair
                }
                Serial.print("Total pairs found so far: ");
                Serial.println(totalpairs);
                if (totalpairs == 6) {
                  Serial.print("You found all the pairs! You, winner! ");
                  Serial.println("Game will restart automatically. ");
                  //delay(10050);
                  //MP3player.playTrack(13);
                  totalpairs = 0;                             // reset variable to start the game again
                }

              } else {
                Serial.println("This is not a pair, booh!");
                Serial.println("You loose 1 point");
                score = score - 1;                        // subscract 1 point to the score
                selection = -1;                           // reset the selection, so we can start again with a new pair
              }

              Serial.print("Your score is now: ");
              Serial.println(score);
              Serial.println("====================");

            }




            /////////////////////////////////////////////////////////////////////////////////////////// your program ends here
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
