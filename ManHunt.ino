
#include "Serial.h"

ServicePortSerial sp;

boolean setUp = true;
boolean huntersSetup = false;
boolean hidersSetup = false;
boolean gameTime = false;

int possibles = 0;
int numHunters = 0;
int currentRole = 0;

int possiblesHiders = 0;
int numHiders = 0;
int currentRoleHiders = 0;

Timer timer;

void setup() {

  sp.begin();

  wipeBoard(0, 255, 0);
  setUp = false;
  huntersSetup = true;
}


void loop() {

  // Set up phase creates a completely green board
  if (setUp) {
    for (int i = 0; i < 6; i++) {
      if (didValueOnFaceChange(i) && getLastValueReceivedOnFace(i) == 35) {
        wipeBoard(0, 255, 0);
        setUp = false;
        huntersSetup = true;
      }

    }
  }


  else if (huntersSetup) {
    if (buttonPressed()) {
      
      // If there was no possible hunter in this spot, tell all other blinks of new possible spot
      if (currentRole == 0) {

        currentRole += 1;
        setColor(MAGENTA);
        possibles += 1;
        setValueSentOnAllFaces(possibles);
      }

      // If possible hunter here, set this spot as decided hunter start point
      else if (currentRole == 1) {
        currentRole++;
        
        numHunters++;
        setColor(RED);
        setValueSentOnAllFaces(25 + numHunters);

      }
    }

    else {
      for (int i = 0; i < 6; i++) {

        if (!isValueReceivedOnFaceExpired(i)) {
          if (didValueOnFaceChange(i)) {
            int valOnFace = getLastValueReceivedOnFace(i);

            if (valOnFace < 25 && currentRole != 2) {
              if (possibles != valOnFace) {
                sp.print("changing");
                sp.print(currentRole);
                currentRole = 0;
                setColor(GREEN);
                setValueSentOnAllFaces(valOnFace);
                possibles++;
              }
            }

            else if (valOnFace > 25 && valOnFace < 35) {
              if(valOnFace > numHunters + 25){
                sp.print("increase hunters");
                numHunters++;
                setValueSentOnAllFaces(valOnFace);
              }
            }
          }
        }
      }

    }
    if (numHunters == 2) {
      huntersSetup = false;
      hidersSetup = true;
    }
  }

  if (hidersSetup) {
    if (buttonPressed()) {
      
      // If there was no possible hider in this spot, tell all other blinks of new possible spot
      // Also, if this was a hunters chosen position, do not let any logic play out
      if (currentRole != 2 && currentRoleHiders == 0) {

        currentRoleHiders += 1;
        setColor(WHITE);
        possiblesHiders += 1;
        setValueSentOnAllFaces(possiblesHiders);
      }

      // If possible hunter here, set this spot as decided hunter start point
      // If this was hunters chosen spot, do not let spot be chosen by hider
      else if (currentRole != 2 && currentRoleHiders == 1) {
        currentRoleHiders++;
        
        numHiders++;
        setColor(BLUE);
        setValueSentOnAllFaces(45 + numHiders);

      }
    }

    else {
      for (int i = 0; i < 6; i++) {

        if (!isValueReceivedOnFaceExpired(i)) {
          if (didValueOnFaceChange(i)) {
            int valOnFace = getLastValueReceivedOnFace(i);
            // Less then 25 means that new possible spot is chosen 
            if (valOnFace < 25 && currentRoleHiders != 2) {

              // This doesnt let one action initiate multiple responses on the same blink
              if (possiblesHiders != valOnFace) {
                sp.print("changing");
                sp.print(currentRoleHiders);

                // Only if this is not chosen hunter spot will role and color change
                if(currentRole != 2){
                  currentRoleHiders = 0;
                  setColor(GREEN);
                }

                // For all pieces, this info needs to be passed forward and kept track of
                setValueSentOnAllFaces(valOnFace);
                possiblesHiders++;
              }
            }

            else if (valOnFace >45 && valOnFace < 60) {
              if(valOnFace > numHiders + 45){
                sp.print("increase hiders");
                numHiders++;
                setValueSentOnAllFaces(valOnFace);
              }
            }
          }
        }
      }

    }
    if (numHiders == 2) {
      hidersSetup = false;
      gameTime = true;
    }
  }

  if(gameTime){
   setColor(ORANGE); 
  }
}


void wipeBoard(int red, int green, int blue) {
  setColor(makeColorRGB(red, green, blue));
  setValueSentOnAllFaces(35);
}
