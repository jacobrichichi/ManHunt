
#include "Serial.h"

ServicePortSerial sp;

boolean setUp = true;
boolean huntersSetup = false;
boolean hidersSetup = false;
boolean gameTime = false;
boolean huntersTurn = false;
boolean hidersTurn = false;

int possibles = 0;
int numHunters = 0;
int currentRole = 0;

int possiblesHiders = 0;
int numHiders = 0;
int currentRoleHiders = 0;

int hidersInSpot = 0;
int huntersInSpot = 0;
int hunterSelected = false;
boolean movementMade = false;
boolean possibleFlashlight = false;
boolean flashlightSpot = false;
int numFlashlights = 0;
int huntersFinished = 0;
boolean possibleMovement = false;


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
              if (valOnFace > numHunters + 25) {
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
      if (currentRole == 2) {
        huntersInSpot = 1;
      }

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
                if (currentRole != 2) {
                  currentRoleHiders = 0;
                  setColor(GREEN);
                }

                // For all pieces, this info needs to be passed forward and kept track of
                setValueSentOnAllFaces(valOnFace);
                possiblesHiders++;
              }
            }

            else if (valOnFace > 45 && valOnFace < 60) {
              if (valOnFace > numHiders + 45) {
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
      if (currentRoleHiders == 2) {
        hidersInSpot = 1;
      }
      hidersSetup = false;
      huntersTurn = true;
    }
  }

  if (huntersTurn) {

    if (huntersInSpot == 0 && flashlightSpot == 0 && !possibleMovement && !possibleFlashlight) {
      setColor(GREEN);
    }
    else if (huntersInSpot > 0) {
      setColor(RED);
    }
    else if (flashlightSpot == 1) {
      setColor(WHITE);
    }
    else if (flashlightSpot == 2) {
      setColor(BLUE);
    }

    else if(possibleFlashlight){
      setColor(MAGENTA);  
    }
    else if(possibleMovement){
      setColor(ORANGE);  
    }

    if (buttonPressed()) {
      
      // Movement already decided?
      if (movementMade) {
        // Is this a possible flashlight blink?
        if (possibleFlashlight) {
          // Set this spot as flashlight
          flashlightSpot = true;
          numFlashlights++;
          setColor(WHITE);

          // Tell everybody else a flashlight has been added
          setValueSentOnAllFaces(4);
        }
      }
      else {
        // Selecting / deselecting hunter for movement
        if (huntersInSpot > 0 && !hunterSelected) {
          hunterSelected = true;
          setValueSentOnAllFaces(0);
          
        }

        else if (huntersInSpot > 0 && hunterSelected) {
          hunterSelected = false;
          setValueSentOnAllFaces(1);
          
        }

        // If this square is a legal move, make it and tell everyone else a move has been made
        else if(huntersInSpot == 0 && possibleMovement){
             sp.print("sjf");
             huntersInSpot++;
             movementMade = true;
             setValueSentOnAllFaces(3);
             
        }
      }

      // Need to figure out

    }

    FOREACH_FACE(f){
        if (!isValueReceivedOnFaceExpired(f)) {
          if (didValueOnFaceChange(f)) {
            int valOnFace = getLastValueReceivedOnFace(f);
            
            if(valOnFace == 0){
                sp.print("possibleMovement");
                possibleMovement = true;
                setColor(ORANGE);
            }
            
            else if(valOnFace == 1){
                possibleMovement = false;
                setColor(GREEN);
            }

            else if(valOnFace == 2){
                possibleFlashlight = true;
            }
            else if(valOnFace == 3){
              sp.print("Possible flashlight");
              if(hunterSelected){
                 huntersInSpot = 0;
                 setValueSentOnAllFaces(6);
              }
              possibleMovement = false;
              movementMade = true;

              possibleFlashlight = true;
              setColor(MAGENTA);
              
              
            }

            else if(valOnFace == 4){
              numFlashlights++;
              setValueSentOnAllFaces(4);  
            }

            else if(valOnFace == 5){
              huntersFinished++;
              numFlashlights = 0;  
            }

            else if(valOnFace == 6){
              possibleMovement = false;  
            }
          }
        }
    }

    if(numFlashlights == 2){
      huntersFinished++;
      numFlashlights = 0; 

      //setValueSentOnAllFaces(5);
    }
    if(huntersFinished == 2){
        huntersTurn = false;
        hidersTurn = true;  
    }

    
  }
  if (hidersTurn) {
    
  }
}


void wipeBoard(int red, int green, int blue) {
  setColor(makeColorRGB(red, green, blue));
  setValueSentOnAllFaces(35);
}
