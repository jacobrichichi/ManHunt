
#include "Serial.h"

ServicePortSerial sp;


boolean huntersSetup = false;
boolean hidersSetup = false;

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
  sp.print("hi");

  if (setUp) {
    timer.set(1);
    for (int i = 0; i < 6; i++) {
      if (didValueOnFaceChange(i) && getLastValueReceivedOnFace(i) == 4) {
        wipeBoard(0, 255, 0);
        setUp = false;
        huntersSetup = true;
      }

    }
  }


  else if (huntersSetup) {
    if (buttonPressed()) {
      if (currentRole == 0) {
        currentRole++;
        setColor(MAGENTA);
        setValueSentOnAllFaces(currentRole);
        timer.set(300);

      }
      else if (currentRole == 1) {
        currentRole++;
        setColor(RED);
        setValueSentOnAllFaces(currentRole);

        timer.set(300);
      }
    }

    else if(timer.isExpired()) {
      for (int i = 0; i < 6; i++) {
        if (didValueOnFaceChange(i)) {
          int valOnFace = getLastValueReceivedOnFace(i);

          if (valOnFace == 1 && currentRole != 2) {
            currentRole = 0;
            setColor(GREEN);
            setValueSentOnAllFaces(1);
            timer.set(300);
          }

          else if (valOnFace == 2) {
            numHunters++;
            setValueSentOnAllFaces(2);
            timer.set(300);
          }

        }
      }
    }
    if (numHunters == 2) {
      huntersSetup = false;
      hidersSetup = true;
    }
  }

  if(hidersSetup){
    setColor(ORANGE);
    }
}


void wipeBoard(int red, int green, int blue) {
  setColor(makeColorRGB(red, green, blue));
  setValueSentOnAllFaces(4);
}
