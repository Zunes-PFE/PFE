#include "PFE_Functions.h"
#include "Assignments_and_Csts.h"
#include "PFE_Macros.h"




void __InitMcu(){
 trisa=0xfE;
 lata.b0=0; // remove wela sgm  D8 of node
 ansela=0;
 ansela.b3=1; ansela.b5=1;  // remove or segemha
 trisb=0xff;
 anselb=0;
 trisc=0x00;
 anselc=0;
 trisd=0x0F;
 anseld=0;
 trise=0;
 ansele=0;
 latd.B4=1; //trisa.b0=0; lata.b0=0;   // to let the node boot up
 intcon= 0xD0;
 intcon3.int2ie =1;
 intcon2.intedg2=0;
 t0con=0x97;
 trisc.B2=0; trisc.B1=0;
 EN1=0;
 ADC_Init();
/*trisd.b2=0;trisd.b1=0;
 M2DirW=0;M1DirW=0;
 delay_ms(4000);
  M2DirW=1;M1DirW=1;
 delay_ms(4000);
 M2DirW=1;M1DirW=0;
 delay_ms(4000);
 M2DirW=0;M1DirW=1;
 delay_ms(4000);
 while(1);*/
}

void __InitGlcd(){
// Port Expander Library uses SPI1 module
 SPI1_Init();                                         // Initialize SPI module used with PortExpander
 SPI_Glcd_Init(0);                                    // Initialize Glcd via SPI
 Glcd_Set_Font_Adv(Font_Glcd_System3x5, 1, 0);           // did it make things faster !!? :o
 SPI_Glcd_Fill(0x00);                              // Clear Glcd  p
}

void __ModeSelectionMacro(){
 while(sameMode==0){
  ModeSelectionAnimation(selectedMode,&removePrevious); // this fct removes the previous(one time) and toggle the selected
  NoIntrDelay(1);
 }                                                  // End of while (~sameMode)
 MALRectangular(selectedMode,1);                   // to validate the mode on glcd
 Hyphen(selectedSubMode,0) ;                       // to remove the previous subMode
 selectedSubMode=DefaultSubMode;                   // every time when new mode reset all sub mode decisions
}

void __SubModeSelectionMacro(){//(char* sameSubMode, char* selectedSubMode, char* removePrevious){

 while(sameSubMode==0){               // code for sub mode selection
  SubModeSelectionAnimation(selectedSubMode,&removePrevious);
  //latd.b5=~latd.b5;
  NoIntrDelay(2);                                            // i could use the tmr0 delay but i should modify the remove previous to remove all the 3 hyphens
 }
 Hyphen(selectedSubMode,1) ;        //validation

}

void __InitPatient(){
 patientNumber = DefaultPatientNumber;           //   <------------------------------ fresh start declarations
 removePrevious=0;                             // to review
 SelectPatientText();
 tmr0h=AnimationDelayH;                                 //// for the sake of animation delay ... because we aren't working with interruptions
 tmr0l=AnimationDelayL;                                 //// for the sake of animation delay ... because we aren't working with interruptions
 intcon.tmr0if=0;
}

void __PatientSelectionMacro(){
 samePatient=0;
 SPI_Glcd_Box(63,24,68,30,0);         // to remove or modify when filling the other subs
 while(sameSubMode && sameMode && OkBtn){
  SelectPatientAnimation(patientNumber, &removePrevious);

 }
      samePatient=1;                                  /// after ok button clicked
      SPI_Glcd_write_char((patientNumber)+48, 63, 3, 1);   /// to validate your choise
}

void __InitQuestion(char statusORAnswer){
 questionNumber = DefaultQuestionNumber;           //   <------------------------------ fresh start declarations
 removePrevious=0;                             // to review
 QHeaderText(statusORAnswer);                                 //bbbbbbbbbbbbbbbbbbbbb
 tmr0h=AnimationDelayH;                                 //// for the sake of animation delay ... because we aren't working with interruptions
 tmr0l=AnimationDelayL;                                 //// for the sake of animation delay ... because we aren't working with interruptions
 intcon.tmr0if=0;
}

void __QusetionSelectionMacro(char statusORAnswer){
  // 0=status , 1=answer
 sameQuestion=0;
 while(sameSubMode && sameMode && OkBtn){
  //if(intcon.tmr0if==1){                       // timer0 delay      <---- if you want to add an animation
   if(removePrevious != questionNumber){
    SPI_Glcd_Write_Text("  ", 30, 4, 1);       //remove >> and insert it only when completed with animation fct
    __ReadPatientQInfo(1);
    while (patientQinfo ==1 && statusORAnswer ==1){
     ChangeToQuestion2(RotSelDir, &questionNumber); // RotSelDir :: will it be as it was ??? , speed issues
     __ReadPatientQInfo(1);
    }
    intcon.gie=0;                                          // disable rottery int to not to print another status to another question
    PatientQOption(questionNumber);
    ShowPatientQInfo (patientQinfo, statusORAnswer);
    intcon.gie=1;                                           // reeanable int
    removePrevious = questionNumber;           // will be on __showqinfo macro
   }
   /*tmr0h=AnimationDelayH;                                 <------ wright the animation here (ex >>,2) ;p
   tmr0l=AnimationDelayL;
   intcon.tmr0if=0;
  }*/
 }
 sameQuestion=1;                                  /// after ok button clicked
}

void __ReadPatientQInfo (char justTheQInfo){        //eeprom function
 unsigned int Adr;                                    //  <------ loading the Qinfo start here
 if     (patientNumber == 1){ Adr=FirstPatientQInfoAdr;}
 else if(patientNumber == 2){ Adr=SecondPatientQInfoAdr;}
 else if(patientNumber == 3){ Adr=ThirdPatientQInfoAdr;}
 patientQinfo= EEPROM_Read(Adr);                      //  <------ loaded Row Qinfo

 if(justTheQInfo){
  if     (questionNumber == 2){ patientQinfo= patientQinfo >> 2;}
  else if(questionNumber == 3){ patientQinfo= patientQinfo >> 4;}
  else if(questionNumber == 4){ patientQinfo= patientQinfo >> 6;}
  patientQinfo &= 0b00000011;                                //  <------ loaded useful Qinfo
 }
 else{
  if     (questionNumber == 1){ patientQinfo&= 0b11111100;}  //  <------ loaded ~useful Qinfo
  else if(questionNumber == 2){ patientQinfo&= 0b11110011;}
  else if(questionNumber == 3){ patientQinfo&= 0b11001111;}
  else if(questionNumber == 4){ patientQinfo&= 0b00111111;}
 }
}

void __WrightPatientQInfo(char newPatientQinfo){       //eeprom function
 unsigned int Adr;
 if(newPatientQinfo==4) {                                                         //  <------ loading the NewQinfo to ram start here
  if(removePrevious==1) { removePrevious=0; newPatientQinfo=0; }
  else                  { removePrevious=1; newPatientQinfo=1; }
 }
 else removePrevious= newPatientQinfo;
 
 if     (questionNumber == 2){ newPatientQinfo= newPatientQinfo << 2;}            //  <------ making NewQinfo ready for writing
 else if(questionNumber == 3){ newPatientQinfo= newPatientQinfo << 4;}
 else if(questionNumber == 4){ newPatientQinfo= newPatientQinfo << 6;}
 newPatientQinfo |= patientQinfo;
 
 if     (patientNumber == 1){ Adr=FirstPatientQInfoAdr;}                         //  <------ loading the writing @ start here
 else if(patientNumber == 2){ Adr=SecondPatientQInfoAdr;}
 else if(patientNumber == 3){ Adr=ThirdPatientQInfoAdr;}

 EEPROM_Write(Adr, newPatientQinfo);                                             //  <------ writing NewQinfo
 delay_ms(20);
 patientQinfo= removePrevious;
}

void __AskQwaitR(){
                                                                                     // initQ
 questionNumber = 1;
 PatientQAnswerText();                                                                                   //// end initQ
 while(questionNumber<5){
  __ReadPatientQInfo(1);                                // read it   useable
  while(patientQinfo ==1){
   questionNumber++;
   if(questionNumber==5) break;
  }
  if(questionNumber==5) break;
  __ReadPatientQInfo(0);                               // read semi_row
  PatientQOption(questionNumber);                      // afficher  la qst
  tmr0h=WaitQDelayH;                                 //// for the sake of animation delay ... because we aren't working with interruptions
  tmr0l=WaitQDelayL;                                 //// for the sake of animation delay ... because we aren't working with interruptions
  intcon.tmr0if=0;
  while(1){
   Tempoo=ADC_Read(3);
   if(Tempoo<512){
    delay_ms(20);
    __WrightPatientQInfo(3);
    break;
   }
   Tempoo=ADC_Read(4);
   if(Tempoo<512){
    delay_ms(20);
    __WrightPatientQInfo(2);
    break;
   }
   else if(~OkBtn || intcon.tmr0if){
    delay_ms(20);
    __WrightPatientQInfo(0);
    break;
   }
  }
  questionNumber++;
 }
}


/*BIIIIIIIIIIIIIIIIIIIIIIIIIG MACROS TO REIMPLEMENT INTO YOUR CODE*/

/*BIIIIIIIIIIIIIIIIIIIIIIIIIG MACROS TO REIMPLEMENT INTO YOUR CODE*/
void __LearnNav(){
 //char i;
 latd.b4=1; // give the node mcu the steering wheel
 trisd.b1=1; trisd.b2=1;    // listen to road
 temp.dominatedCommand = 7;
 intcon3.INT1IE= 1; //  Activate MEncoder interruptions
 while(1){
  if(~OKBtn){    break;}
 }                             // here road info are stored in ram ;p
 /*PWM1_Stop();
 PWM2_Stop();
 trisc.b1=0; trisc.b2=0;
 portc.b1=0; portc.b2=0; */
 latd.b4=0; // take steering wheel
 intcon3.INT1IE= 0; //  Disactivate MEncoder interruptions

 roadInfo[ramAdress]= temp;
 /*SPI_Glcd_write_char(ramAdress+48, 63, 3, 2);
 while(~OkBtn);
 while(OkBtn);
 while(~OkBtn);

 temp.dominatedCommand=9; temp.npulse=0; temp.nDeg=0;
 for(i=ramAdress+1;i<40;i++){
  roadInfo[i]= temp;
 }

 ramAdress=39;
 SPI_Glcd_write_char(ramAdress+48, 63, 3, 2);

 while(~OkBtn);
 while(OkBtn);
 while(~OkBtn);*/
 
 // if big amount of time then wright something on screen "WAIT" ;p
 __WrightRoad();        // this fct wrightes ram stored infos to eeprom
}

void __WrightRoad(){                        // PLZ Add 20ms delay to complete
 char adr, fAdr, i;
 switch (patientNumber){                      // move to store road macro or function ;p
  case 1: adr = FirstPatientRoadInfoAdr;  fAdr=FirstPatientEndRoadAdr;  break;
  case 2: adr = SecondPatientRoadInfoAdr; fAdr=SecondPatientEndRoadAdr; break;
  case 3: adr = ThirdPatientRoadInfoAdr;  fAdr=ThirdPatientEndRoadAdr;  break;
 }
 i=0;
 while(i<=ramAdress){
  if ( roadInfo[i].dominatedCommand%5 == 0 ){       // writing the translation commands
   if(i==0 && roadInfo[i].dominatedCommand == 10){ EEPROM_Write(adr++, 0x00); delay_ms(100);  EEPROM_Write(adr++, 0xB4); delay_ms(100); EEPROM_Write(adr++, 0x00);delay_ms(100); }// rotate 180 deg from right ;p
   while (roadInfo[i].nMeter >= 63){
    EEPROM_Write(adr++, 0xFF); // cmd = 1 unit = Meter  number =63
    delay_ms(100);
    roadInfo[i].nMeter -= 63;
    if(adr == fAdr) break;
   }
   if (roadInfo[i].nMeter){ EEPROM_Write(adr++, 0b11000000+roadInfo[i].nMeter); delay_ms(100); if(adr == fAdr) break; }
   if (roadInfo[i].nDeci){  EEPROM_Write(adr++, 0b10010000+roadInfo[i].nDeci); delay_ms(100);  if(adr == fAdr) break; }
   if (roadInfo[i].nCenti){ EEPROM_Write(adr++, 0b10000000+roadInfo[i].nCenti); delay_ms(100); if(adr == fAdr) break; }
   if (roadInfo[i].nPulse){ EEPROM_Write(adr++, 0b10100000+roadInfo[i].nPulse); delay_ms(100); if(adr == fAdr) break; }
  }
  else{                                          // writing rotation commands
   if ( (roadInfo[i-1].dominatedCommand==10) ^ (roadInfo[i+1].dominatedCommand==10) ){   // XOR ;p    to invert command with 180-deg
    if( roadInfo[i].dominatedCommand == 6 ) roadInfo[i].dominatedCommand = 9;
    else                                    roadInfo[i].dominatedCommand = 6;
    
    if( roadInfo[i].nPulse ){ roadInfo[i].nPulse = ClaculatedPulseTo4Deg - roadInfo[i].nPulse; roadInfo[i].nDeg+=4; }
    if( roadInfo[i].nDeg ){ roadInfo[i].nDeg = 180 - roadInfo[i].nDeg; }

   }
   if ( roadInfo[i].dominatedCommand == 6 ){              // right rotation
    if (roadInfo[i].nDeg){ if(adr+3 == fAdr) {break;}  EEPROM_Write(adr++, 0b01000000); delay_ms(100);  EEPROM_Write(adr++, roadInfo[i].nDeg); delay_ms(100); EEPROM_Write(adr++, 0b01000000); delay_ms(100); }
    if (roadInfo[i].nPulse){ EEPROM_Write(adr++, 0b01100000+roadInfo[i].nPulse); delay_ms(100); if(adr == fAdr) break; }
   }
   else{                                                  // left rotation
    if (roadInfo[i].nDeg){ if(adr+3 == fAdr) {break;}  EEPROM_Write(adr++, 0b00000000); delay_ms(100);  EEPROM_Write(adr++, roadInfo[i].nDeg); delay_ms(100); EEPROM_Write(adr++, 0b00000000); delay_ms(100); }
    if (roadInfo[i].nPulse){ EEPROM_Write(adr++, 0b00100000+roadInfo[i].nPulse); delay_ms(100); if(adr == fAdr) break; }
    else if(roadInfo[i].nPulse==0 && roadInfo[i].nDeg==0) {EEPROM_Write(adr++, 0b00000000); delay_ms(100); break;}
   }
  }
  i++;
 }
 if(adr==0) EEPROM_Write(fAdr,0);
 else EEPROM_Write(fAdr, adr-1);
}






void __AutoNav(short Task){      // 1 for going -1 for retracting
 char rowInfo,extractedCmd,previousExtractedCmd;
 char  fAdr,Dir;
 short adr;
 
 unsigned nPulses=0;
 switch (patientNumber){                      // move to store road macro or function ;p
  case 1: adr = FirstPatientRoadInfoAdr;  fAdr=EEPROM_Read(FirstPatientEndRoadAdr);  delay_ms(20); break;
  case 2: adr = SecondPatientRoadInfoAdr; fAdr=EEPROM_Read(SecondPatientEndRoadAdr); delay_ms(20); break;
  case 3: adr = ThirdPatientEndRoadAdr;   fAdr=EEPROM_Read(ThirdPatientEndRoadAdr);  delay_ms(20); break;
 }
if ( Task == -1 ){ // retracting so we are reading the opposite
  rowInfo = adr;
  adr     = fAdr;
  fAdr    = rowInfo;
  __ExecutePulses(962,3);               /// 180° rotation
 }

// previousExtractedCmd= 1;
 while ( adr<=fAdr && Task== 1 || adr>=fAdr && Task== -1 ){
  rowInfo = EEPROM_Read(adr);
  delay_ms(100);
  // search for the command " is it rotation or translation ?
  extractedCmd = rowInfo & 0b10000000;
  if( previousExtractedCmd!= extractedCmd ){
   if(nPulses) __ExecutePulses(nPulses,Dir);
   delay_ms(1000);                                        //<--------------------------- TO RMV
   latd.b6=0;
   latd.b5=0;
   delay_ms(200); //simulation sake                       //<--------------------------- TO RMV
   nPulses=0;
   previousExtractedCmd  =  extractedCmd;
  }
  if (extractedCmd == 0 ){     // rotation
   //set the dir;
   if (rowInfo & 0b01000000)  Dir=2; //R
   else                       Dir=3; //L
   if (Task == -1) {
    if (Dir == 2)   Dir=3;
    else            Dir=2;
   }
   //serach for the unit :p
   if (rowInfo & 0b00100000) nPulses+= (rowInfo  & 0b00011111);                              // Pulse
   else{ adr +=Task; rowInfo = EEPROM_Read(adr); delay_ms(100); adr +=Task;  nPulses+= (rowInfo/4) * ClaculatedPulseTo4Deg;  }  // Degrees  !! can it be simplified to just /9 ??
   
  }
  else{                       // Translation
   //set the dir;
   Dir=1;                    //translation
   //serach for the unit :p
   if      (rowInfo & 0b01000000) nPulses+= (rowInfo  & 0b00111111)*ClaculatedPulseToCenti*100;   // Meter
   else if (rowInfo & 0b00100000) nPulses+= (rowInfo  & 0b00011111);                              // Pulse
   else if (rowInfo & 0b00010000) nPulses+= (rowInfo  & 0b00001111)*ClaculatedPulseToCenti*10;    // Dm
   else                           nPulses+= (rowInfo  & 0b00001111)*ClaculatedPulseToCenti;       // Cm
  }
  adr+=Task;
 }
 //execute the final task
 __ExecutePulses(nPulses,Dir);
 if ( Task == -1 ){ // retracting so we are reading the opposite
  __CalibrateRobot();// add Calibration here :p:p:p:p;p;p;p;p;p    + 180° ;p;p;p

 }
}

void __ManualNav(short Task){
 latd.b4=1; EN1=1;EN2=1; // give the node mcu the steering wheel
 trisd.b1=1; trisd.b2=1;    // listen to road
 while(1){
/*if(~OKBtn){
   tmr0h=ModePushDelayH;
   tmr0l=ModePushDelayL;                                       //<---------------- To activate this
   intcon.tmr0if=0;
   while(~OkBtn){}
   if (intcon.tmr0if) break;
  }*/


  if(~OKBtn){    break;}
 }                             // here road info are stored in ram ;p
 latd.b4=0; EN1=0;EN2=0; // take steering wheel
 if(Task==-1) __CalibrateRobot();
}


void __ExecutePulses(unsigned long int nPulses,char Dir){ // 1 for forward; 2 for right; 3 for left
 char i=0,j=0,Obs;
 // move that shit to output
 trisb.b1=0;
 trisd.b1=0;
 trisd.b2=0;
 EN1=1;EN2=1;
 // set direction pins
 switch(Dir){
  case 1: // forward
   M1DirW=1;
   M2DirW=0;

  break;
  case 2: // right
   M1DirW=1;
   M2DirW=1;
  break;
  case 3: // left
   M1DirW=0;
   M2DirW=0;
  break;
 }
 addedActions=0;
 // Execute Fpulses
 while (nPulses){
  mPulsesPin=1;
  delay_us(MotorSpeedDelay);
  mPulsesPin=0;

   Trig=1;
   delay_us(10);
   Trig=0;
   delay_us(1500);
   if(~Echo){
    i++;
    if(i==7 && ~addedActions){
    //latd.b7=1;
     tmr0h=OneSecDelayH; //0x67
     tmr0l=OneSecDelayL;  //0x69
     intcon.tmr0if=0;
     while(i>0){
      Trig=1;
      delay_us(10);
      Trig=0;
      delay_us(1000);
      if(Echo) i--;
      else     i=5;
      if(intcon.tmr0if) {
       j++;
       tmr0h=OneSecDelayH;   //0x67
       tmr0l=OneSecDelayL;  //0x69
       intcon.tmr0if=0;
       /*if(j==5) {
        __checkAvailableSol();
       }*/
       if(j>=5){                                  // <------------------- change to 10
        Obs=__AvoidObstacleMacro();
        if (Obs) { addedActions=1;  SPI_Glcd_Write_Text("OBS DN", 30, 4, 0);
                                    delay_ms(1000);
                                    SPI_Glcd_Write_Text("     ", 30, 4, 1);  
        }
        else{
         i=5;
         while(i>0){
          Trig=1;
          delay_us(10);
          Trig=0;
          delay_us(1000);
          if(Echo) i--;
          else     i=5;
         }
        }
        break;
       }
      }
     }
     M1DirW=1; M2DirW=0;
    }
    else if(i==7 && addedActions){
     i=5;
     while(i>0){
      Trig=1;
      delay_us(10);
      Trig=0;
      delay_us(1000);
      if(Echo) i--;
      else     i=5;
     }
    }
   }
   else i=0;
   delay_us(MotorSpeedDelay-1010);

  nPulses--;
 }
 if (addedActions){
  __ExecutePulsesNoUltrasonic(472,addedRotation);
  __ExecutePulsesNoUltrasonic(addedPulses,1);
  if (addedRotation==2) __ExecutePulsesNoUltrasonic(472,3);
  else                  __ExecutePulsesNoUltrasonic(472,2);
  addedActions=0; addedPulses=0;
 }
 // move that shit back to input
 trisb.b1=1;
 trisd.b1=1;
 trisd.b2=1;
 EN1=0;EN2=0;
}

void __ExecutePulsesNoUltrasonic(unsigned long int nPulses,char Dir){ // 1 for forward; 2 for right; 3 for left

 switch(Dir){
  case 1: // forward
   M1DirW=1;
   M2DirW=0;
  break;
  case 2: // right
   M1DirW=1;
   M2DirW=1;
  break;
  case 3: // left
   M1DirW=0;
   M2DirW=0;
  break;
  case 4: //Backward
   M1DirW=0;
   M2DirW=1;
 }
 // Execute Fpulses
 while (nPulses){
  mPulsesPin=1;
  delay_us(MotorSpeedDelay);
  mPulsesPin=0;
  delay_us(MotorSpeedDelay);
  nPulses--;
 }
}

void __CalibrateRobot(){
char inTrack=0;
// move that shit to output
 trisb.b1=0;
 trisd.b1=0;
 trisd.b2=0;
 EN1=1;EN2=1;
 SPI_Glcd_Write_Text("Find Line  ", 25, 2, 1);
 delay_ms(1000);
 inTrack=FindLine();
 delay_ms(1000);
 if(inTrack==2) { 
  SPI_Glcd_Write_Text("  Put Me   ", 25, 2, 1);
  SPI_Glcd_Write_Text("Into Track ", 25, 4, 1);
  latd.b4=1;
  while(Okbtn) {}
  while(~OkBtn){}
  latd.b4=0;
  SPI_Glcd_Write_Text("           ", 25, 2, 1);
  SPI_Glcd_Write_Text("           ", 25, 4, 1);
 }
 else if(inTrack==0){ 
  SPI_Glcd_Write_Text("  Out      ", 25, 2, 1);
 delay_ms(1000);
 OutTrackCalibration();
 }
 SPI_Glcd_Write_Text("  In       ", 25, 2, 1);
 delay_ms(1000);
 InTrackCalibration(1);        //
 SPI_Glcd_Write_Text("  180      ", 25, 2, 1);
 delay_ms(1000);
 __ExecutePulsesNoUltrasonic(962,2);               /// 180° rotation
 SPI_Glcd_Write_Text("  In       ", 25, 2, 1);
 delay_ms(1000);
 InTrackCalibration(1);       //
 
// move that shit back to input
 trisb.b1=1;
 trisd.b1=1;
 trisd.b2=1;
 EN1=0;EN2=0;
}


char FindLine(){
 int i=1,nR=0,nL=0;
 nR=0; nL=0;
               // Right Rotation
 while (nR<=20 && nL<=20 && i<=472)  {       // 472 pulses = 90 deg
  __ExecutePulsesNoUltrasonic(1,2);
  i++;
  if(RIR) {nR++;}
  else if (nR>0 ) {nR=0;}

  if(LIR) {nL++;}
  else if (nL>0 ) {nL=0;}
 }

 if (nL>=21) {__ExecutePulsesNoUltrasonic(i,3); return 1;} // we are in track  }   /// in case we where stikstik
 
 if (i==473) {__ExecutePulsesNoUltrasonic(473,3); i=0;}
 else return 0;



 nR=0; nL=0;
                // Left Rotation
 while (nL<=20 && i<=472)  {       // 472 pulses = 90 deg
  __ExecutePulsesNoUltrasonic(1,3);
  i++;
  if(LIR) {nL++;}
  else if (nL>0 ) {nL=0;}
 }
 if (i>=473) {
  return 2;
 }
 else return 0;

}

void OutTrackCalibration(){
 while ( RIR || LIR )  {
   if (RIR) {__ExecutePulsesNoUltrasonic(1,2); }
   if (LIR) {__ExecutePulsesNoUltrasonic(1,3); }
  __ExecutePulsesNoUltrasonic(1,1);
 }
}

void InTrackCalibration(char ForB) {
 int nR=0,nL=0;
 
 if(ForB==1){
  while ( 1 )  {
   if (RIR) {__ExecutePulsesNoUltrasonic(1,2); nR++; }
   else if (nR>0 ) {nR=0;}

   if (LIR) {__ExecutePulsesNoUltrasonic(1,3); nL++; }
   else if (nL>0 ) {nL=0;}
   __ExecutePulsesNoUltrasonic(1,1);
   if ((nR>=20)&&(nL>=20))    break;
  }
 }

 else {
  while ( 1 )  {
   if (RIR) {__ExecutePulsesNoUltrasonic(1,3); nR++; }
   else if (nR>0 ) {nR=0;}

   if (LIR) {__ExecutePulsesNoUltrasonic(1,2); nL++; }
   else if (nL>0 ) {nL=0;}
   __ExecutePulsesNoUltrasonic(1,4);
   if ((nR>=20)&&(nL>=20))    break;
  }
 }
}

void __MoveTostartPoint(){
 /*while(~OkBtn){}
 while(Okbtn) {latd.b6=1; delay_ms(250); latd.b6=0; delay_ms(250);}
 while(~OkBtn){}*/
 // move that shit to output
 trisb.b1=0;
 trisd.b1=0;
 trisd.b2=0;
 EN1=1;EN2=1;
 

 __ExecutePulsesNoUltrasonic(750,4);
 InTrackCalibration(1);
// move that shit back to input
 trisb.b1=1;
 trisd.b1=1;
 trisd.b2=1;

}

void __SensorStatusMacro(char SpecialMenu){
 char i=0,nR=0,nL=0,Us=0,pnR=3,pnL=3,pUs=3;
 if(SpecialMenu){
   SPI_Glcd_Write_Text("S->", 115, 5, 1);
 }
 while(sameSubMode && sameMode)  {
  while(i<30){
   Trig=1;
   delay_us(10);
   Trig=0;
   delay_us(HalfMeterDelay);
   if(~Echo) Us++;
   if (RIR)  nR++;
   if (LIR)  nL++;
   i++;
  }
  if (Us>=20) Us=1;
  else        Us=0;
  
  if (nR>=20) nR=1;
  else        nR=0;
  
  if (nL>=20) nL=1;
  else        nL=0;
  
  if(pUs != Us) {
   SPI_Glcd_Circle_Fill(55, 25, 5, Us);
   SPI_Glcd_Circle_Fill(85, 25, 5, Us);
  }

  if(pnR != nR) SPI_Glcd_Box(51, 36, 62, 42, nR);
  if(pnL != nL) SPI_Glcd_Box(77, 36, 89, 42, nL);

  i=0; pnR=nR; pnL=nL; pUs=Us;
  
  if (~OkBtn && SpecialMenu) {
   tmr0h=ModePushDelayH;
   tmr0l=ModePushDelayL;
   intcon.tmr0if=0;
   while(~OkBtn);
   if (intcon.tmr0if) break;
  }  // End testing ok btn
 }  // End Loop of capture
 if(~SpecialMenu){
  SPI_Glcd_Write_Text("                  ", 25, 3, 1);
  SPI_Glcd_Write_Text("                  ", 25, 5, 1);
  SPI_Glcd_Write_Text("   ", 115, 5, 1);
 }
}  // End __SensorStatusMacro

void __SOptionSelectionMacro(){
 sameSOption=0;
 while(sameSubMode && sameMode && OkBtn){
  SOptionAnimation(sOption, &removePrevious);
  NoIntrDelay(2);
 }
 sameSOption=1;
 SpecialHyphen(sOption,1) ;        //validation
}

void __ExecuteSpecialOption(char option){
 if(sameSubMode && sameMode ) {
  switch (option){
   case 1:        //EXIT
    SPI_Glcd_H_Line(26, 28, 12, 1);
    subCompleted=1;
    SPI_Glcd_Write_Text("DONE           ", 30, 1, 1);

   break;

   case 2:       //DISABLE Q
    SPI_Glcd_H_Line(26, 28, 20, 1);
    EEPROM_Write(FirstPatientQInfoAdr,  0b01010101); delay_ms(100);
    EEPROM_Write(SecondPatientQInfoAdr, 0b01010101); delay_ms(100);
    EEPROM_Write(ThirdPatientQInfoAdr,  0b01010101); delay_ms(100);
    SPI_Glcd_Write_Text("DONE                 ", 30, 2, 1);
   break;

   case 5:         //FACTORY RESET => if you notice no break has been introduced
    SPI_Glcd_H_Line(26, 28, 44, 1);

   case 3:      //RESET QI
    if(option!=5) SPI_Glcd_H_Line(26, 28, 28, 1);
    EEPROM_Write(FirstPatientQInfoAdr,  0x00); delay_ms(100);
    EEPROM_Write(SecondPatientQInfoAdr, 0x00); delay_ms(100);
    EEPROM_Write(ThirdPatientQInfoAdr,  0x00); delay_ms(100);
    if(option!=5){SPI_Glcd_Write_Text("DONE                 ", 30, 3, 1);
   break;}

   case 4:      //RESET ROAD
    if(option!=5) SPI_Glcd_H_Line(26, 28, 36, 1);
    EEPROM_Write(FirstPatientRoadInfoAdr,  0xA0); delay_ms(100); EEPROM_Write(FirstPatientEndRoadAdr,  FirstPatientRoadInfoAdr);  delay_ms(100);
    EEPROM_Write(SecondPatientRoadInfoAdr, 0xA0); delay_ms(100); EEPROM_Write(SecondPatientEndRoadAdr, SecondPatientRoadInfoAdr); delay_ms(100);
    EEPROM_Write(ThirdPatientRoadInfoAdr,  0xA0); delay_ms(100); EEPROM_Write(ThirdPatientEndRoadAdr,  ThirdPatientRoadInfoAdr);  delay_ms(100) ;
    if(option!=5) SPI_Glcd_Write_Text("DONE                ", 30, 4, 1);
    else          SPI_Glcd_Write_Text("DONE         ", 30, 5, 1);
   break;
  }
 }
}


void __checkAvailableSol(){

}

char __AvoidObstacleMacro(){
 char i,j=0,Us=0;
 bit solved;
 solved=0;
 /*Confirm(4,1);
 while(1){
  Tempoo=ADC_Read(3);
  if(Tempoo<512){
   Confirm(4,0);
   break;
  }
  Tempoo=ADC_Read(4);
  if(Tempoo<512){
   Confirm(4,0);
   return 0;
  }
 }*/
 //// you came here if you choose to avoid the obs
 __ExecutePulsesNoUltrasonic(472,2);                       //turn right
 for(i=0;i<30;i++){                           // test for clear way
  Trig=1;
  delay_us(10);
  Trig=0;
  delay_us(HalfMeterDelay);
  if(~Echo) Us++;
 }
 if(Us<=20){                                    /// if good Start the right Escape
  Us=0;
  
  SPI_Glcd_Write_Text("R E S", 30, 4, 0);
  delay_ms(1000);
  SPI_Glcd_Write_Text("     ", 30, 4, 1);
  
  do{
   __ExecutePulsesNoUltrasonic(1250,1); j++;
   delay_ms(1000);;
   __ExecutePulsesNoUltrasonic(472,3);
   delay_ms(1000);
   for(i=0;i<30;i++){
    Trig=1;
    delay_us(10);
    Trig=0;
    delay_us(HalfMeterDelay);
    if(~Echo) Us++;
   }
   if(Us<=20){ solved=1; addedRotation=3; addedPulses=j*1250;   break;}                 ////// heeeeeeeeeeeeere
   else {
    Us=0;
    __ExecutePulsesNoUltrasonic(472,2);
    delay_ms(1000);
    for(i=0;i<30;i++){
     Trig=1;
     delay_us(10);
     Trig=0;
     delay_us(HalfMeterDelay);
     if(~Echo) Us++;
    }
   }
  }while(Us<=20);
 }
 
 if (solved) return 1;

 else {                                          /// right Escape Failed
  SPI_Glcd_Write_Text("R E F", 30, 4, 0);
  delay_ms(1000);
  SPI_Glcd_Write_Text("     ", 30, 4, 1);
  __ExecutePulsesNoUltrasonic(962,3);           // trun 180 Left
  SPI_Glcd_Write_Text("return Back", 30, 4, 0);
 delay_ms(1000);
  SPI_Glcd_Write_Text("     ", 30, 4, 1);
  __ExecutePulsesNoUltrasonic(1250*j,1);        // go back to start
  delay_ms(1000);
  
  j=0;
  Us=0;
  for(i=0;i<30;i++){                 // Test if clear (Left)
   Trig=1;
   delay_us(10);
   Trig=0;
   delay_us(HalfMeterDelay);
   if(~Echo) Us++;
  }
  if(Us<=20){
   Us=0;
   
   SPI_Glcd_Write_Text("L E S", 30, 4, 0);
   delay_ms(1000);
   SPI_Glcd_Write_Text("     ", 30, 4, 1);
  
   do{                                          /// Start the Left Escape
    __ExecutePulsesNoUltrasonic(1250,1); j++;
    delay_ms(1000);
    __ExecutePulsesNoUltrasonic(472,2);
    delay_ms(1000);
    Us=0;
    for(i=0;i<30;i++){
     Trig=1;
     delay_us(10);
     Trig=0;
     delay_us(HalfMeterDelay);
     if(~Echo) Us++;
    }
    if(Us<=20){ solved=1; addedRotation=2; addedPulses=j*1250;    break;}                 ////// heeeeeeeeeeeeere
    else {
     Us=0;
     __ExecutePulsesNoUltrasonic(472,3);
     delay_ms(1000);
     for(i=0;i<30;i++){
      Trig=1;
      delay_us(10);
      Trig=0;
      delay_us(HalfMeterDelay);
      if(~Echo) Us++;
     }
    }
   }while(Us<=20);
  }
  if (solved) return 1;
  else{ 
   SPI_Glcd_Write_Text("L E F", 30, 4, 0);
   delay_ms(1000);
   SPI_Glcd_Write_Text("     ", 30, 4, 1);
  }
 }
 __ExecutePulsesNoUltrasonic(962,2);              //return back    /// Left Escape Failed
 SPI_Glcd_Write_Text("return Back", 30, 4, 0);
 delay_ms(1000);
 SPI_Glcd_Write_Text("     ", 30, 4, 1);
 __ExecutePulsesNoUltrasonic(1250*j,1);
 __ExecutePulsesNoUltrasonic(472,3);
 return 0;
}