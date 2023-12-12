/*
  Feeding experimentation device 3 (FED3)

  This script demonstrates how to write a menu system so you can access multiple programs from the FED3 startup screen.  You will scroll through programs by assigning them to "modes".

  In this example, we run four modes:
  // FEDmodes:
  // 0 Free feeding
  // 1 FR1
  // 2 RR5
  // 3 RR10
  // 4 RR20
  // 5 Extinction

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz

*/


#include <FED3.h>                //Include the FED3 library 
String sketch = "Menu";          //Unique identifier text for each sketch
FED3 fed3 (sketch);              //Start the FED3 object

//assigning random ratios

int FR1to20 = random(11, 21);  //11 to 20
int FR1to10 = random(6, 11);  //6 to 10
int FR1to5 = random(1, 6);   //1 to 5
int currentPokeCount = 0;
int Start = 1;

bool LeftActive = false;                               //Set to false to make right poke active

void setup() {
  randomSeed(analogRead(0));
  fed3.FED3Menu = true;                                //Activate the menu function at startup
  fed3.begin();                                        //Setup the FED3 hardware

  //Add these two lines to disable sleep and monitor the pellet well from the Arduino script
  fed3.disableSleep();                                  
  pinMode (1, INPUT_PULLUP);
}

void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 1: Free feeding
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 0) {
    fed3.sessiontype = "Free_feed";                     //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.DisplayPokes = false;                          //Turn off poke indicators for free feeding mode
    fed3.UpdateDisplay();                               //Update display for free feeding session to remove poke displayt (they are on by default)
    fed3.Feed();
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 2: Fixed Ratio 1
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 1)  {
    fed3.sessiontype = "FR1";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Right) {
      fed3.logRightPoke();                               //Log right poke
      if (fed3.RightCount % fed3.FR == 0) {              //if fixed ratio is  met
        fed3.ConditionedStimulus();                     //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                    //deliver pellet
      }
    }
    if (fed3.Left) {                                    //If left poke is triggered
      fed3.logLeftPoke();
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 3: Random Ratio 5
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 2) {
    fed3.sessiontype = "RRatio5";                  //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Right) {
      currentPokeCount ++;
      fed3.Click();                                     //click stimulus
      fed3.logRightPoke();                               //Log right poke
      if (LeftActive == false) {
        if (currentPokeCount % FR1to5 == 0) {                 //if fixed ratio is  met
          fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
          fed3.Feed();                                  //deliver pellet
          FR1to5 = random(1, 6);                            //Pick a random number between 1 and 5 to be the FR
          fed3.FR = FR1to5;                                 //Share the FR ratio with the fed3 library
          currentPokeCount = 0;
        }
      }
    }

    // If Left poke is triggered
    if (fed3.Left) {
      fed3.logLeftPoke();                              //Log Left poke
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 4: Random Ratio 10
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 3) {
    fed3.sessiontype = "RRatio10";                 //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Right) {
      currentPokeCount ++;
      fed3.Click();                                     //click stimulus
      fed3.logRightPoke();                               //Log right poke
      if (LeftActive == false) {
        if (currentPokeCount % FR1to10 == 0) {                 //if fixed ratio is  met
          fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
          fed3.Feed();                                  //deliver pellet
          FR1to10 = random(6, 11);                           //Pick a random number between 1 and 10 to be the FR
          fed3.FR = FR1to10;                                 //Share the FR ratio with the fed3 library
          currentPokeCount = 0;
        }
      }
    }

    // If Left poke is triggered
    if (fed3.Left) {
      fed3.logLeftPoke();                              //Log Left poke
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 5: Random Ratio 20
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 4) {
    fed3.sessiontype = "RRatio20";                 //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Right) {
      currentPokeCount ++;
      fed3.Click();                                     //click stimulus
      fed3.logRightPoke();                               //Log right poke
      if (LeftActive == false) {
        if (currentPokeCount % FR1to20 == 0) {                 //if fixed ratio is  met
          fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
          fed3.Feed();                                  //deliver pellet
          FR1to20 = random(11, 21);                           //Pick a random number between 1 and 20 to be the FR
          fed3.FR = FR1to20;                                 //Share the FR ratio with the fed3 library
          currentPokeCount = 0;
        }
      }
    }

    // If Left poke is triggered
    if (fed3.Left) {
      fed3.logLeftPoke();                              //Log Left poke
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 6: Extinction
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 5) {
    fed3.sessiontype = "Extinct";                        //The text in "sessiontype" will appear on the screen and in the logfile
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    if (fed3.Right) {
      fed3.logRightPoke();                                //Log right poke
      fed3.ConditionedStimulus();                        //deliver conditioned stimulus (tone and lights)
      delay(350);
      fed3.pixelsOff();
    }

    if (fed3.Left) {                                    //If left poke is triggered
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
