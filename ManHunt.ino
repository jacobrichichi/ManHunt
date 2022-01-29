
#include "Serial.h"

ServicePortSerial sp;


boolean huntersSetup = false;
boolean hidersSetup = false;

int possibles = 0;
int numHunters = 0;
int currentRole = 0;
boolean setUp = true;

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
    setColor(ORANGE);
  }
}


void wipeBoard(int red, int green, int blue) {
  setColor(makeColorRGB(red, green, blue));
  setValueSentOnAllFaces(35);
}
