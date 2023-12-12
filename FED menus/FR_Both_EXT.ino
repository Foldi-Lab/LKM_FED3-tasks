/*
  Feeding experimentation device 3 (FED3)
  Classic FED3 script
  This script mimicks the classic FED3 menuing system for selecting among the following programs

  // FEDmodes:
  // 0 Free feeding
  // 1 FR1
  // 2 FR3
  // 3 FR5
  // 4 Progressive Ratio
  // 5 Extinction
  // 6 FR1 (reversed)
  // 7 FR3 (reversed)
  // 8 FR5 (reversed)
  // 9 PR (reversed)

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz

*/

#include <FED3.h>                //Include the FED3 library 
String sketch = "Classic";       //Unique identifier text for each sketch
FED3 fed3 (sketch);              //Start the FED3 object

//variables for PR tasks
int poke_num = 0;                                      // this variable is the number of pokes since last pellet
int pokes_required = 1;                                // increase the number of pokes required each time a pellet is received using an exponential equation
int Start = 1;

void setup() {
  fed3.ClassicFED3 = true;
  fed3.begin();                                        //Setup the FED3 hardware

  
  //Add these two lines to disable sleep and monitor the pellet well from the Arduino script
  fed3.disableSleep();                                  
  pinMode (1, INPUT_PULLUP);
}

void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 0: Free feeding
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 0) {
    fed3.sessiontype = "Free_feed";                     //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.DisplayPokes = false;                          //Turn off poke indicators for free feeding mode
    fed3.UpdateDisplay();                               //Update display for free feeding session to remove poke displayt (they are on by default)
    fed3.Feed();
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Modes 1-3: Fixed Ratio Programs LEFT FR1, FR3, FR5
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if ((fed3.FEDmode == 1) or (fed3.FEDmode == 2) or (fed3.FEDmode == 3)) {
    if (fed3.FEDmode == 1) fed3.sessiontype = "FR1";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 2) fed3.sessiontype = "FR3";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 3) fed3.sessiontype = "FR5";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Left) {
      fed3.logLeftPoke();                               //Log left poke
      if (fed3.LeftCount % fed3.FR == 0) {              //if fixed ratio is  met
        fed3.ConditionedStimulus();                     //deliver conditioned stimulus (lights)
        fed3.Feed();                                    //deliver pellet
      }
    }
    if (fed3.Right) {                                    //If right poke is triggered
      fed3.logRightPoke();
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 4: Progressive Ratio
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 4) {
    fed3.sessiontype = "ProgRatio";                      //The text in "sessiontype" will appear on the screen and in the logfile
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    if (fed3.Left) {                                     //If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();                                //Log left poke
      poke_num++;                                        //store this new poke number as current poke number.
      if (poke_num == pokes_required) {                  //check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
        fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (lights)
        fed3.Feed();                                     //Deliver pellet
        pokes_required = round((5 * exp((fed3.PelletCount + 1) * 0.2)) - 5);  //increment the number of pokes required according to the progressive ratio:
        fed3.FR = pokes_required;
        poke_num = 0;                                    //reset the number of pokes back to 0, for the next trial
        fed3.Left = false;
      }
      else {
        fed3.Click();                                   //If not enough pokes, just do a Click
      }
    }
    if (fed3.Right) {                                    //If right poke is triggered and pellet is not in the well
      fed3.logRightPoke();
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 5: Extinction
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 5) {
    fed3.sessiontype = "Extinct";                        //The text in "sessiontype" will appear on the screen and in the logfile
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    if (fed3.Left) {
      fed3.logLeftPoke();                                //Log left poke
      fed3.ConditionedStimulus();                        //deliver conditioned stimulus (lights)
      delay(350);
      fed3.pixelsOff();
    }

    if (fed3.Right) {                                    //If right poke is triggered
      fed3.logRightPoke();
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 6-8: Reversed Fixed Ratio Programs RIGHT FR1, FR3, FR5
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ((fed3.FEDmode == 6) or (fed3.FEDmode == 7) or (fed3.FEDmode == 8)) {
    if (fed3.FEDmode == 6) fed3.sessiontype = "FR1 (rev)";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 7) fed3.sessiontype = "FR3 (rev)";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 8) fed3.sessiontype = "FR5 (rev)";    //The text in "sessiontype" will appear on the screen and in the logfile
      fed3.activePoke = 0;                                  //Set activePoke to 0 to make right poke active
      if (fed3.Left) {                                      //If left poke
        fed3.logLeftPoke();                                 //Log left poke
      }
    
    if (fed3.Right) {                                     //If right poke is triggered
      fed3.logRightPoke();                                //Log Right Poke
      if (fed3.RightCount % fed3.FR == 0) {               //if fixed ratio is  met
        fed3.ConditionedStimulus();                       //Deliver conditioned stimulus (lights)
        fed3.Feed();                                      //deliver pellet
      }
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 9: PR (reversed)
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 9) {
    fed3.sessiontype = "PR (rev)";                          //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.activePoke = 0;                                //Right poke is active
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    if (fed3.Right) {                                   //If Right poke is triggered and pellet is not in the well
      fed3.logRightPoke();                              //Log Right poke
      poke_num++;                                       //store this new poke number as current poke number.
      if (poke_num == pokes_required) {                 //check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
        fed3.ConditionedStimulus();                     //Deliver conditioned stimulus (lights)
        fed3.Feed();                                    //Deliver pellet
        pokes_required = round((5 * exp((fed3.PelletCount + 1) * 0.2)) - 5);  //increment the number of pokes required according to the progressive ratio:
        fed3.FR = pokes_required;
        poke_num = 0;                                   //reset the number of pokes back to 0, for the next trial
        fed3.Right = false;
      }
      else {
        fed3.Click();                                   //If not enough pokes, just do a Click
      }
    }
    if (fed3.Left) {                                    //If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                    Call fed.run at least once per loop
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  fed3.run();
  }
