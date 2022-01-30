
//#include "Serial.h"

//ServicePortSerial sp;

boolean setUp = true;
boolean huntersSetup = false;
boolean hidersSetup = false;
boolean gameTime = false;
boolean huntersTurn = false;
boolean hidersTurn = false;
boolean inbetweenTurns = false;
boolean finishingTurn = false;

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
int flashlightPropagation = 4;
boolean flashlightState = true;
int hidersCaught = 0;
boolean anyHunterChoosing = false;
boolean possibleHiderMovement = false;
boolean anyHidersChoosing = false;
boolean hiderSelected = false;
int hidersMoved = 2;
int turnsLeft = 5;


Timer timer;

void setup() {

  //sp.begin();

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
                //sp.print("changing");
                //sp.print(currentRole);
                currentRole = 0;
                setColor(GREEN);
                setValueSentOnAllFaces(valOnFace);
                possibles++;
              }
            }

            else if (valOnFace > 25 && valOnFace < 35) {
              if (valOnFace > numHunters + 25) {
                //sp.print("increase hunters");
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
               // sp.print("changing");
               // sp.print(currentRoleHiders);

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
               // sp.print("increase hiders");
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
          setColor(WHITE);

          // Tell everybody else a flashlight has been added

          if(numFlashlights == 0 && huntersFinished == 0){
          //  sp.print("00");
            numFlashlights = 1;
            setValueSentOnAllFaces(7);  
          }
          else if (numFlashlights == 1 && huntersFinished == 0){
           //   sp.print("10");
              numFlashlights = 0;
              huntersFinished = 1;
              movementMade = false;
              setValueSentOnAllFaces(8);  
          }
          else if(numFlashlights == 0 && huntersFinished == 1){
            //  sp.print("01");
              numFlashlights = 1;
              setValueSentOnAllFaces(9);
          }

          
          else if(numFlashlights == 1 && huntersFinished == 1){
            //sp.print("11");
            setValueSentOnAllFaces(10);
            possibleMovement = false;
            possibleFlashlight = false;

            huntersTurn = false;
            inbetweenTurns = true; 
          }
          

          flashlightState = false;
        }
      }
      else {
        // Selecting / deselecting hunter for movement
        if (huntersInSpot > 0 && !hunterSelected) {

          if(!anyHunterChoosing){
            hunterSelected = true;
            anyHunterChoosing = true;
            setValueSentOnAllFaces(0);
          }
          
          
        }

        else if (huntersInSpot > 0 && hunterSelected) {
          hunterSelected = false;
          setValueSentOnAllFaces(1);
          
        }

        // If this square is a legal move, make it and tell everyone else a move has been made
        else if(huntersInSpot == 0 && possibleMovement){
             huntersInSpot = 1;
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
            
            if(valOnFace == 11){
              anyHunterChoosing = true;
            }
            
            if(valOnFace == 12){
              anyHunterChoosing = false; 
            }
            
            if(valOnFace == 0 && huntersInSpot == 0){
                
                possibleMovement = true;
                setValueSentOnAllFaces(11);
                setColor(ORANGE);
            }

            
            else if(valOnFace == 1){
                setValueSentOnAllFaces(12);
                possibleMovement = false;
                setColor(GREEN);
            }

            else if(valOnFace == 2 && huntersInSpot == 0){
                possibleFlashlight = true;
            }
            else if(valOnFace == 3){
              if(hunterSelected){
                 huntersInSpot = 0;
                 setValueSentOnAllFaces(6);
              }
              possibleMovement = false;
              movementMade = true;


              if(huntersInSpot == 0){
                possibleFlashlight = true;
                setColor(MAGENTA);
              }
              
              
              
            }

            else if(valOnFace == 5){
      
              huntersFinished++;
              numFlashlights = 0;  
            }

            else if(valOnFace == 6){
              possibleMovement = false;  
            }

            else if(valOnFace == 7){

              //sp.print("Val is 7");
              numFlashlights = 1;
              setValueSentOnAllFaces(7); 
               
            }
            else if(valOnFace == 8){
            //  sp.println("val is 8");
              numFlashlights = 0;
              huntersFinished = 1;
              hunterSelected = false;
              movementMade = false;

              possibleMovement = false;
              possibleFlashlight = false;
              anyHunterChoosing = false;
              
              setValueSentOnAllFaces(8); 
            }

            else if(valOnFace == 9){
            //  sp.println("val is 9");
              numFlashlights = 1;        
              
              setValueSentOnAllFaces(9); 
            }
            else if(valOnFace == 10){
           //   sp.println("val is 10");
              numFlashlights = 0;
              huntersFinished = 2;

              possibleMovement = false;
              possibleFlashlight = false;
          //    sp.println("hunt turn over");
              huntersTurn = false;
              inbetweenTurns = true; 
        
              
              setValueSentOnAllFaces(10); 
            }
          }
        }
    }

    
  }
  if(inbetweenTurns){

    
    if(flashlightSpot && hidersInSpot > 0){
      setColor(BLUE);
    //  inbetweenTurns = false;
      //hidersTurn = true;
    }  
    else if(!flashlightSpot && huntersInSpot == 0){
      setColor(GREEN);  
    }

    if(huntersInSpot > 0 && hidersInSpot > 0){
      
      setColor(MAGENTA);
      //Hunters win
      if(hidersCaught == 1){
        
        setValueSentOnAllFaces(50);
        gameOver(false);  
        inbetweenTurns = false;
      }  
      
      //Only one caught
      else{
        hidersInSpot = 0;
        hidersCaught = 1;
        setValueSentOnAllFaces(40);
       // inbetweenTurns = false;
       // hidersTurn = true;
      }
    }

    if(buttonPressed()){
      setValueSentOnAllFaces(30);
      inbetweenTurns = false;
      hidersTurn = true;
    }
    FOREACH_FACE(f){
        if (!isValueReceivedOnFaceExpired(f)) {
          if (didValueOnFaceChange(f)) {
            int valOnFace = getLastValueReceivedOnFace(f);

            if(valOnFace == 30){
                inbetweenTurns = false;
                hidersTurn = true;
                setValueSentOnAllFaces(30);
              }

              else if(valOnFace == 40){
                hidersCaught = 1;
                setValueSentOnAllFaces(40);
              }
              else if(valOnFace == 50){
                hidersCaught = 2;
                gameOver(false);
                inbetweenTurns = false;
                setValueSentOnAllFaces(50);
              }
          }
       }
    }
  }
  
  if (hidersTurn) {
    if (hidersInSpot == 0 && huntersInSpot == 0 && !possibleHiderMovement) {//empty space
      setColor(GREEN);
    }
    else if (huntersInSpot > 0) {//Hunter space
      setColor(RED);
    }
    else if(hidersInSpot > 0){
      setColor(BLUE);  
    }
    else if (possibleHiderMovement) {//possible Movement
      setColor(ORANGE);
    }

    if(buttonPressed()){
      if(hidersInSpot > 0 && !anyHidersChoosing){
        hiderSelected = true; //note that this is a selected hider
        anyHidersChoosing = true;
        setValueSentOnAllFaces(0);  
      }

      else if(possibleHiderMovement){
        hidersInSpot = 1;

        if(hidersMoved == 2){
          setValueSentOnAllFaces(2);
          hidersMoved = 1;
            
        }
        
        else if(hidersMoved == 1){
          setValueSentOnAllFaces(20);
          hidersMoved = 0;
            
        }
      }
    }

     FOREACH_FACE(f){
        if (!isValueReceivedOnFaceExpired(f)) {
          if (didValueOnFaceChange(f)) {
            int valOnFace = getLastValueReceivedOnFace(f);

            if(valOnFace == 0 && hidersInSpot == 0 && huntersInSpot == 0){
                possibleHiderMovement = true;
                anyHidersChoosing = true;
                setValueSentOnAllFaces(1);
            }

            if(valOnFace == 1){
              anyHidersChoosing = true;  
            }

            if(valOnFace == 2){
              if(hiderSelected){
                 hiderSelected = false;
                 hidersInSpot = 0;
              }
              anyHidersChoosing = false;
              hidersMoved = 1;
              possibleHiderMovement = false;
              setValueSentOnAllFaces(2);
            }

            if(valOnFace == 20){
                if(hiderSelected){
                  hiderSelected = false;
                  hidersInSpot = 0;
                }
                hidersMoved = 0;
                anyHidersChoosing = false;
                possibleHiderMovement = false;
                hidersTurn = false;
                finishingTurn = true;
                
                hunterSelected = false;
                movementMade = false;
                possibleFlashlight = false;
                flashlightSpot = false;
                numFlashlights = 0;
                huntersFinished = 0;
                possibleMovement = false;
                anyHunterChoosing = false;
                possibleHiderMovement = false;
                anyHidersChoosing = false;
                hiderSelected = false;
                hidersMoved = 2;

                setValueSentOnAllFaces(20);

            }

            
          }
        }
     }

  }

  if(finishingTurn){
    turnsLeft--;
    finishingTurn = false;
    if(turnsLeft == 0){
      gameOver(true);  
    }  
    else{
      huntersTurn = true;  
    }
  }
}


void wipeBoard(int red, int green, int blue) {
  setColor(makeColorRGB(red, green, blue));
  setValueSentOnAllFaces(35);
}

void gameOver(boolean winner){
    if(winner){
      setColor(BLUE);
    }
    else{
      setColor(RED);
    }
      
  }
