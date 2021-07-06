#include "PFE_Functions.h"
#include "Assignments_and_Csts.h"
#include "PFE_Macros.h"

int Tempoo,addedPulses;




//#define Debugging
char selectedMode=0, removePrevious=0, selectedSubMode, patientNumber, questionNumber, sOption,addedRotation;      // for memory allocation optimisation we have chosen char type :p
bit sameMode, sameSubMode, onePass, samePatient, sameQuestion, sameSOption, subCompleted, changeQStatus,addedActions;



char patientQinfo; // devided into 4 sections for each question :
                        // 00 not answered , 01 parametricaly disabled , 10 ansewed NO , 11 answered YES
char command, ramAdress;
struct RoadInstructions temp, roadInfo[30];          // you'll find the declaration of this type on assignement and constants



void interrupt() {               // INT SCOPE
  if(intcon3.int2if){                      // Rb interruptions
   latd.b0=~latd.b0;
   if( ValidateBtn==0 ) {               // validate Btn interruption
    tmr0h=ModePushDelayH;
    tmr0l=ModePushDelayL;
    intcon.tmr0if=0;
    if(sameMode==0) intcon.tmr0if=1;         // to disable the delay once you are on mode selection
    while (ValidateBtn==0){}                 // while still clicked
    if(intcon.tmr0if && sameSubMode){           // if "1" change modeMenu Status      ,,, added && sameSubMode so when on sub selection even delayed push would validate it ( to recheck :p )
     if(sameMode==0) sameMode=1;    // to invert bit         <--------- to replace with ~ ;p
     else sameMode=0;               // to invert bit         <--------- to replace with ~ ;p
    }
    else if(sameMode==1) {                      // if "0" change subModeMenu Status
     if(sameSubMode==0) sameSubMode=1; // to invert bit       <--------- to replace with ~ ;p
     else sameSubMode=0;               // to invert bit        <--------- to replace with ~ ;p
    }
   }                                     // END validate Btn interruption
   intcon3.int2if=0;
  }                                      // END Rb interruptions
  
  if(intcon.int0if){                     // Select Roterry interruptions
   if(sameMode==0){
    removePrevious = selectedMode;
    ChangeToMode( RotSelDir, &selectedMode);
   }
   else if(sameSubMode==0){
    removePrevious = selectedSubMode;
    ChangeToSubMode( RotSelDir, &selectedSubMode);
   }
   else if(samePatient==0) {
    removePrevious = patientNumber;
    ChangeToPatient(RotSelDir, &patientNumber);
   }
   else if(sameQuestion==0) {
    removePrevious = questionNumber;
    ChangeToQuestion(RotSelDir, &questionNumber);
   }
  else if(sameSOption==0) {
    removePrevious = sOption;
    ChangeToSOption(RotSelDir, &sOption);
   }
   intcon.int0if=0;
  }                                      // END Select Roterry interruptions
  
  if(intcon3.int1if){                     // Motor Encoder interruptions
   // /*          remove the "//" if you are working with Dc with Encoders
   command= M1DirR + M2DirR*2 ;
   switch (command){
    case 1 : command = 5;  break;  /// Forward  :p
    case 2 : command = 10; break;  /// Backward :p
    case 0 : command = 9;  break;  /// Right    :p
    case 3 : command = 6;  break;  /// Left     :p
   }
   // */
   //command= M2Backword*8 + M2Forword*4 + M1Backword*2 + M1Forword ;
   if ( temp.dominatedCommand ==7) {                                      /// only on first time when learnig command=7 ;p
    temp.dominatedCommand = command;
    temp.nPulse=1;
    temp.nDeci=0;temp.nCenti=0;temp.nDeg=0;temp.nMeter=0;
    ramAdress=0;
   }
   else if ( command%5 == 0 && temp.dominatedCommand%5 == 0 ) {          // we are making the Move bytes
    if( command == temp.dominatedCommand ){                      //if same cmnd then add to pulses
     temp.nPulse++;
     if( temp.nPulse == ClaculatedPulseToCenti ) { temp.nPulse=0; temp.nCenti++;}
     if( temp.nCenti == 10 )                     { temp.nCenti=0; temp.nDeci++; }
     if( temp.nDeci  == 10 )                     { temp.nDeci=0; temp.nMeter++; }
    }
    //}
    else{                                                       //if diff cmnd then retract from pulses
     temp.nPulse--;
     if( temp.nPulse == -1 ) { temp.nPulse=ClaculatedPulseToCenti-1; temp.nCenti--; }
     if( temp.nCenti == -1 ) { temp.nCenti=9; temp.nDeci--; }
     if( temp.nDeci == -1 ) { temp.nDeci=9; temp.nMeter--; }
     if( temp.nMeter == -1 ) {                                 //if ~overflow then chose the shortest path ;p
      temp.nMeter=0;
      temp.nPulse=1; temp.nCenti=0; temp.nDeci=0;     // be sure if it's 1
      if ( temp.dominatedCommand ==5 )  temp.dominatedCommand=10;
      else                              temp.dominatedCommand=5;
     }
    }
   }
   else if ( command%3 == 0 && temp.dominatedCommand%3 == 0 ){           // we are making the Rot bytes
    if(command == temp.dominatedCommand){                      //if same cmnd then add to pulses
     temp.nPulse++;
     if( temp.nPulse == ClaculatedPulseTo4Deg ) { temp.nPulse=0; temp.nDeg+=4; }
     if( temp.nDeg == 180 ) {                                  //if overflow then chose the shortest path ;p
      //temp.nDeg=171;
      if ( temp.dominatedCommand ==6 )  temp.dominatedCommand=9;
      else                              temp.dominatedCommand=6;
     }
    }
    else{                                               //if diff cmnd then retract from pulses
     temp.nPulse--;
     if( temp.nPulse < 0 ) { temp.nPulse=ClaculatedPulseTo4Deg-1; temp.nDeg-=4; }
     if( temp.nDeg   < 0 ) {                                 //if overflow then chose the shortest path ;p
      temp.nDeg=0;
      temp.nPulse=1; temp.nCenti=0; temp.nDeci=0;      // Confirm if its 1
      if ( temp.dominatedCommand ==6 )  temp.dominatedCommand=9;
      else                              temp.dominatedCommand=6;
     }
    }
   }
   else {                                                 // if you have changed the Command
    roadInfo[ramAdress]=temp;
    ramAdress++;
    temp.dominatedCommand = command;
    temp.nPulse=1;
    temp.nCenti=0; temp.nDeci=0; temp.nMeter=0; temp.nDeg=0;
   }
   intcon3.int1if=0;
  }                                      // End Motor Encoder interruptions

}                         // END INT SCOPE

void main() {

 __InitMcu();
 __InitGlcd();
 CuteFace();                                     // That needs to be Cuter :p
 
 ModeMenu();

 selectedMode=DefaultMode;
 sameMode=1;
 SubModeMenu(selectedMode);
 sameSubMode=1;
 latd.b4=0; trisa.b0=1; //take the stering wheel  and relisten to rotary dir
 
 //InTrackCalibration(0);
 
 while(1) {   // infnite loop
  
  __ModeSelectionMacro();//(&sameMode, &selectedMode, &removePrevious, &selectedSubMode);
  // why variables? => sameMode: if 1 break , selectedMode: continuasly change value + validation
  // removePrevious: rmv previous rectangular , selectedSubMode: assign default
  
  switch( selectedMode ){

   case 1:   //***************************************Manual Mode******************************************************************
    while(sameMode){
     __SubModeSelectionMacro();
     
     switch (selectedSubMode) {          // code for sub mode functions inside Auto mode  <-------------------------------------------
/*1*/case 1 :                          // start of Navigation SubMode
      __InitPatient();
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __PatientSelectionMacro();
       onePass=1;                            // for the sake of passing from confirm() one time
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        if(onePass==1){
         Confirm(1,1);                   //confirm?
         onePass=0;
        }
        Tempoo=ADC_Read(3);
        if(Tempoo<512){
         Confirm(1,0);
         // here you can implement while
         //__MoveTostartPoint();
         __ManualNav(1); // go                                                                // <----------------   Auto Navigation.
         __AskQwaitR();
         __ManualNav(-1); // return
         
         subCompleted=1;
         break;
        }
        //else if(~NoBtn){
        Tempoo=ADC_Read(4);
        if(Tempoo<512){
         Confirm(1,0);                      // remove Confirm text ;p
         break;
        }
       }
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
     break;                            // end of Navigation SubMode

/*2*/case 2 :                           // start Question statistics sub
      __InitPatient();
      __PatientSelectionMacro();
      if ( sameMode && sameSubMode){             // just to not to type this init when clicking on validate btn
       while(~OkBtn){}                  // not to read the okaybtn for the next function
       __InitQuestion(1);                // 1 for answer
       while(~OkBtn){}                  // not to read the okaybtn for the next function
      }
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __QusetionSelectionMacro(1);           // show questions with answer
       onePass=1;                            // for the sake of passing from confirm() one time
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        if(onePass==1){
         Confirm(3,1);         //exit?
         onePass=0;
        }
        Tempoo=ADC_Read(3);
        if(Tempoo<512){
         Confirm(3,0);
         subCompleted =1;

         break;
        }
        Tempoo=ADC_Read(4);
        if(Tempoo<512){
         Confirm(3,0);                      // remove Confirm text ;p
         ShowPatientQInfo (patientQinfo, 1);

         break;
        }
       }
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
      break;                            // end of Question statistics sub
      
/*3*/case 3:
      SensorText();
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __SensorStatusMacro(0);               // show questions with answer
      }
     break;                             // end of Rs sub
     
     }                                   // end of switch(submodes)
    }
   break;     //**********************************End of Manual Mode****************************************************************
   
   case 2:   //**************************************Auto Mode**********************************************************************
   while(sameMode){
    __SubModeSelectionMacro();


    //to write that patient number equals the defult number declared in assignement and cst :p
    switch (selectedSubMode) {          // code for sub mode functions inside Auto mode  <-------------------------------------------
/*1*/case 1 :                          // start of Navigation SubMode
      __InitPatient();
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __PatientSelectionMacro();
       onePass=1;                            // for the sake of passing from confirm() one time
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        if(onePass==1){
         Confirm(1,1);                   //confirm?
         onePass=0;
        }
        Tempoo=ADC_Read(3);
        if(Tempoo<512){
         Confirm(1,0);
         //__MoveTostartPoint();
         __AutoNav(1); // go                                                                // <----------------   Auto Navigation.
         __AskQwaitR();
         __AutoNav(-1); // return
         subCompleted=1;
         break;
        }
        Tempoo=ADC_Read(4);
        if(Tempoo<512){
         Confirm(1,0);                      // remove Confirm text ;p
         break;
        }
       }
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
     break;                            // end of Navigation SubMode
      
/*2*/case 2 :                           // start Question statistics sub
      __InitPatient();
      __PatientSelectionMacro();
      if ( sameMode && sameSubMode){             // just to not to type this init when clicking on validate btn
       while(~OkBtn){}                  // not to read the okaybtn for the next function
       __InitQuestion(1);                // 1 for answer
       while(~OkBtn){}                  // not to read the okaybtn for the next function
      }
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __QusetionSelectionMacro(1);           // show questions with answer
       onePass=1;                            // for the sake of passing from confirm() one time
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        if(onePass==1){
         Confirm(3,1);         //exit?
         onePass=0;
        }
        Tempoo=ADC_Read(3);
        if(Tempoo<512){
         Confirm(3,0);
         subCompleted =1;

         break;
        }
        Tempoo=ADC_Read(4);
        if(Tempoo<512){
         Confirm(3,0);                      // remove Confirm text ;p
         ShowPatientQInfo (patientQinfo, 1);

         break;
        }
       }
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
     break;                            // end of Question statistics sub
     
/*3*/case 3:
      SensorText();
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __SensorStatusMacro(0);               // show questions with answer
      }
     break;                             // end of Rs sub
     
    }                                   // end of switch(submodes)
   }
   break;    //***********************************End of Auto Mode*******************************************************************
   
   case 3:   //************************************Learning Mode*********************************************************************
   while(sameMode){
    __SubModeSelectionMacro();


    switch (selectedSubMode) {          // code for sub mode functions inside Learning mode  <-------------------------------------------
/*1*/case 1 :                          // start of Navigation SubMode
      __InitPatient();
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __PatientSelectionMacro();
       onePass=1;                            // for the sake of passing from confirm() one time
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        if(onePass==1){
         Confirm(1,1);                      //confirm?
         onePass=0;
        }
        Tempoo=ADC_Read(3);
        if(Tempoo<512){
         Confirm(1,0);
         //__MoveTostartPoint();
         __LearnNav(); // go   
         __AutoNav(-1); // return                                                              // <----------------   Learn Navigation.
         subCompleted=1;
         break;
        }
        Tempoo=ADC_Read(4);
        if(Tempoo<512){
         Confirm(1,0);                      // remove Confirm text ;p
         break;
        }
       }
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
     break;                            // end of Navigation SubMode

/*2*/case 2 :                           // start Question status sub
      __InitPatient();
      __PatientSelectionMacro();
      if ( sameMode && sameSubMode){             // just to not to type this init when clicking on validate btn
       while(~OkBtn){}                  // not to read the okaybtn for the next function
       __InitQuestion(0);               // 0 for status
       while(~OkBtn){}                  // not to read the okaybtn for the next function
      }
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __QusetionSelectionMacro(0);           // show questions with answer
       onePass=1;                            // for the sake of passing from confirm() one time
       tmr0h=ModePushDelayH;
       tmr0l=ModePushDelayL;
       intcon.tmr0if=0;
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        if(onePass==1){
         while(~OkBtn){ }
         if(intcon.tmr0if){
          changeQStatus=1;
          Confirm(2,1);
         }
         else Confirm(3,1);         //exit?
         onePass=0;
        }
        Tempoo=ADC_Read(3);
        if(Tempoo<512){
         if(changeQStatus){
          removePrevious= patientQinfo;
          __ReadPatientQInfo(0);
          Confirm(2,0);
          delay_ms(20);
          __WrightPatientQInfo(4);
          ShowPatientQInfo (patientQinfo, 0);
          changeQStatus=0;
          removePrevious = questionNumber;
         }
         else {
          Confirm(3,0);
          subCompleted =1;
         }
         break;
        }
        Tempoo=ADC_Read(4);
        if(Tempoo<512){
         if(changeQStatus)   Confirm(2,0);
         else Confirm(3,0);                      // remove Confirm text ;p
         ShowPatientQInfo (patientQinfo, 0);

         break;
        }
       }
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
     break;                            // end of Question statistics sub
/*3*/case 3:
      SensorText();
      while(sameSubMode && sameMode ) {                         // you will loop here if you choose no as a confirmation
       __SensorStatusMacro(1);               // show questions with answer
       sameSOption=0; sameQuestion=1; samePatient=1;
       sOption=1;
       while(sameSubMode && sameMode ) {     // wait for confirmation or break (the two condition is their to prove that the break comes from okbtn )
        SpecailMenu();
        __SOptionSelectionMacro();   //<------- /////work on
        __ExecuteSpecialOption(sOption);
        if (subCompleted)  break;
       }
       SPI_Glcd_Write_Text("                       ", 25, 3, 1);
       SPI_Glcd_Write_Text("              ", 25, 5, 1);
       if (subCompleted) {subCompleted =0; break;}   // to break all whiles and start fresh new submode
      }
     break;
    }                                   // end of switch(submodes)
   }
   break;     //*******************************End of Learning Mode******************************************************************
  } //while selectedMode end
 }//infnite loop end
}//main() end