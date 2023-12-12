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
  // 5 Closed Economy
  // 6 Extinction
  // 7 FR1 (reversed)
  // 8 FR3 (reversed)
  // 9 FR5 (reversed)
  // 10 PR (reversed)
  // 11 CE (reversed)

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

//variables for CE tasks
int pellets_in_current_block = 0;                      //pellet number in current block
unsigned long poketime = 0;                            //time of poke
int resetInterval = 600;                               //number of seconds without a poke to reset

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
  //                                                                     Modes 1-3: Fixed Ratio Programs FR1, FR3, FR5
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if ((fed3.FEDmode == 1) or (fed3.FEDmode == 2) or (fed3.FEDmode == 3)) {
    if (fed3.FEDmode == 1) fed3.sessiontype = "FR1";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 2) fed3.sessiontype = "FR3";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 3) fed3.sessiontype = "FR5";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Left) {
      fed3.logLeftPoke();                               //Log left poke
      if (fed3.LeftCount % fed3.FR == 0) {              //if fixed ratio is  met
        fed3.ConditionedStimulus();                     //deliver conditioned stimulus (tone and lights)
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
        fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
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
  //                                                                     Mode 5: Closed Economy
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 5) {
    fed3.sessiontype = "ClosedEcon";                     //The text in "sessiontype" will appear on the screen and in the logfile
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    checkReset();                                        //Check if it's time to reset to FR1
    if (fed3.Left) {                                     //If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();                                //Log left poke
      poke_num++;                                        //increment poke number
      poketime = fed3.unixtime;                          //update the current time of poke
      serialoutput();                                    //print data to the Serial monitor - EnableSleep must be false to use Serial monitor
      if (poke_num == pokes_required) {                  //check if current FR has been achieved
        fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
        pellets_in_current_block++;                      //increment the pellet number by 1
        fed3.BlockPelletCount = pellets_in_current_block;
        fed3.Feed();                                     //Deliver pellet
        fed3.BNC(500, 1);                                //Send 500ms pulse to the BNC output when pellet is detected (move this line to deliver this pulse elsewhere)
        pokes_required = round((5 * exp((fed3.BlockPelletCount + 1) * 0.2)) - 5);  //increment the number of pokes required according to the progressive ratio:
        fed3.FR = pokes_required;
        poke_num = 0;                                    //reset the number of pokes back to 0, for the next trial
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
  //                                                                     Mode 6: Extinction
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 6) {
    fed3.sessiontype = "Extinct";                        //The text in "sessiontype" will appear on the screen and in the logfile
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    if (fed3.Left) {
      fed3.logLeftPoke();                                //Log left poke
      fed3.ConditionedStimulus();                        //deliver conditioned stimulus (tone and lights)
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
  //                                                                     Mode 7-9: Reversed Fixed Ratio Programs FR1, FR3, FR5
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ((fed3.FEDmode == 7) or (fed3.FEDmode == 8) or (fed3.FEDmode == 9)) {
    if (fed3.FEDmode == 7) fed3.sessiontype = "FR1 (rev)";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 8) fed3.sessiontype = "FR3 (rev)";    //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.FEDmode == 9) fed3.sessiontype = "FR5 (rev)";    //The text in "sessiontype" will appear on the screen and in the logfile
      fed3.activePoke = 0;                                  //Set activePoke to 0 to make right poke active
      if (fed3.Left) {                                      //If left poke
        fed3.logLeftPoke();                                 //Log left poke
      }
    
    if (fed3.Right) {                                     //If right poke is triggered
      fed3.logRightPoke();                                //Log Right Poke
      if (fed3.RightCount % fed3.FR == 0) {               //if fixed ratio is  met
        fed3.ConditionedStimulus();                       //Deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                      //deliver pellet
      }
    }
    if (digitalRead(1) == LOW) {                                   //If magazine break is triggered
      fed3.logRightPoke();                                //Log FAUX right poke
      delay(500);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 10: PR (reversed)
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 10) {
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
        fed3.ConditionedStimulus();                     //Deliver conditioned stimulus (tone and lights)
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
  //                                                                     Mode 11: Closed Economy
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 11) {
    fed3.sessiontype = "CE (rev)";                       //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.activePoke = 0;                                 //Right poke is active
    if (Start == 1) {
      fed3.Event = "Start";
      fed3.logdata();
      Start = 0;
    }
    checkReset();                                        //Check if it's time to reset to FR1
    
    if (fed3.Right) {                                     //If right poke is triggered and pellet is not in the well
      fed3.logRightPoke();                                //Log right poke
      poke_num++;                                        //increment poke number
      poketime = fed3.unixtime;                          //update the current time of poke
      serialoutput();                                    //print data to the Serial monitor - EnableSleep must be false to use Serial monitor
      if (poke_num == pokes_required) {                  //check if current FR has been achieved
        fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
        pellets_in_current_block++;                      //increment the pellet number by 1
        fed3.BlockPelletCount = pellets_in_current_block;
        fed3.Feed();                                     //Deliver pellet
        fed3.BNC(500, 1);                                //Send 500ms pulse to the BNC output when pellet is detected (move this line to deliver this pulse elsewhere)
        pokes_required = round((5 * exp((fed3.BlockPelletCount + 1) * 0.2)) - 5);  //increment the number of pokes required according to the progressive ratio:
        fed3.FR = pokes_required;
        poke_num = 0;                                    //reset the number of pokes back to 0, for the next trial
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

//////////////////////////////////////////////////////////////////////////////////////
//if more than resetInterval mins has passed since last poke -- reset the block and parameters
//////////////////////////////////////////////////////////////////////////////////////
void checkReset() {
  if (fed3.unixtime - poketime >= resetInterval) {   //if the reset interval has elapsed since last poke
    pellets_in_current_block = 0;
    fed3.BlockPelletCount = pellets_in_current_block;
    poke_num = 0;
    pokes_required = 1;
    fed3.FR = pokes_required;
    Serial.println("          ");
    Serial.println("****");                           //print **** on the serial monitor

    fed3.pixelsOn(5, 5, 5, 5);
    delay(200);
    fed3.pixelsOff();
    poketime = fed3.unixtime;                        //store the current time of poke
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Use Serial.print statements for debugging
//////////////////////////////////////////////////////////////////////////////////////
void serialoutput() {
  Serial.print("Unixtime: ");
  Serial.println(fed3.unixtime);
  Serial.println("Pellets   RightPokes   LeftPokes   Poke_Num  Pel  Pokes_Required  PokeTime   Reset  FR");
  Serial.print("   ");
  Serial.print(fed3.PelletCount);
  Serial.print("          ");
  Serial.print(fed3.RightCount);
  Serial.print("          ");
  Serial.print(fed3.LeftCount);
  Serial.print("          ");
  Serial.print(poke_num);
  Serial.print("          ");
  Serial.print(pellets_in_current_block);
  Serial.print("          ");
  Serial.print(pokes_required);
  Serial.print("       ");
  Serial.print(poketime);
  Serial.print("          ");
  Serial.print(fed3.FR);
  Serial.println(" ");
  Serial.println(" ");
}
