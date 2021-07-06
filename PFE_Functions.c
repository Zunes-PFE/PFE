#include "PFE_Functions.h"
#include "Bitmap.h"
#include "Assignments_and_Csts.h"


//--------------------- General functions Start Here ---------------------------

void NoIntrDelay(char Nx100){
  intcon.gie=0;
  while(Nx100>0){
   Delay_ms(100);
   Nx100--;
  }
  intcon.gie=1;
}

void Debug(){
 int i ;
 for(i =114 ;i<127; i++){
   //SPI_Glcd_V_Line(0, 63, i, 1);
   SPI_Glcd_Dot(i, 49, 2);
  }
}

void CuteFace(){
 SPI_Glcd_Image (TestCuteFace);                      // Draw image
 Delay_ms(1000);
 SPI_Glcd_fill(0x00);                               // Clear GLCD
}

// to add cool face(auto) and nerd face (learning) + manual face hhhh ;p


//----------------------- Mode functions Start Here ----------------------------

void ModeMenu(){
 // 4 paddings 11 dots + 3 sentenses of M:29, A:19, L:36
 SPI_Glcd_H_Line(0, 127, 49, 1);
 SPI_Glcd_Write_Text_Adv("Manual", 11, 53);
 SPI_Glcd_Write_Text_Adv("Auto", 51, 53);
 SPI_Glcd_Write_Text_Adv("Learnin", 81, 53);
 SPI_Glcd_Write_Char_Adv('g',112,51);
}

void ChangeToMode(char Dir, char* operationMode){
 // Manual:1  Auto:2  Learning:3
 if(Dir)   (*operationMode)++;
 else      (*operationMode)--;
 if(*operationMode == 4) *operationMode=1;
 if(*operationMode==0)   *operationMode=3;
}


void MALRectangular(char operationMode, char i) {
/* work on 3 modes :
 1st removes rect             ==> i=0;
 2nd is to validate the rectangle ==> i=1
 3rd toggle the rectangle         ==> i=2 */
 
switch (operationMode){
  case 1 :                //Manual
  // MRectungular Start
  SPI_Glcd_Rectangle(9, 52, 41, 63, i);
  // MEnd
  break;
  case 2 :                //Auto
  // Arectungular Start
  SPI_Glcd_Rectangle(49, 52, 71, 63, i);
  //  AEnd
  break;
  case 3 :                //Learning
  // Lrectungular Start
  SPI_Glcd_Rectangle(79, 52, 117, 63, i);
  // LEnd
  break;
 }
}

void ModeSelectionAnimation(char selectedMode,char* removePrevious){
 if(*removePrevious){                          // to remove the previous rect one time :p
  MALRectangular(*removePrevious,0);
  *removePrevious=0;
 }
 MALRectangular(selectedMode,2);        // to toggle the rectangular on glcd
}


//---------------------- SubMode Functions Start Here --------------------------

void SubModeMenu(char selectedMode ){
 SPI_Glcd_V_Line(0, 48, 18, 1);
 SPI_Glcd_Set_Font(&Font_Glcd_System3x5, 3, 5, 32);
 SPI_Glcd_Write_Text_Adv("N",  5, 5);
 SPI_Glcd_Write_Text_Adv("Qs", 5, 20);
 SPI_Glcd_Write_Text_Adv("Rs", 5, 35);
}

void ChangeToSubMode(char Dir, char* selectedSubMode) {
 // Navigation:1  Pool(statistics):2  Soon(:p):3
 if(Dir)   (*selectedSubMode)++;
 else         (*selectedSubMode)--;
 if(*selectedSubMode==4) *selectedSubMode=1;
 if(*selectedSubMode==0) *selectedSubMode=3;
}

void Hyphen(char selectedSubMode, char i){
 switch (selectedSubMode){
  case 1:
   SPI_Glcd_H_Line(1, 3, 10, i);                 // for navigation
  break;
  case 2:
   SPI_Glcd_H_Line(1, 3, 25, i);                 // for Question Statistics
  break;
  case 3:
   SPI_Glcd_H_Line(1, 3, 40, i);                 // for Robot Statistics
  break;
 }
}

void SubModeSelectionAnimation(char selectedSubMode,char* removePrevious){
 if(*removePrevious){                          // to remove the previous rect one time :p
  Hyphen(*removePrevious,0);
  *removePrevious=0;
 }
 Hyphen(selectedSubMode,2);        // to toggle the rectangular on glcd
}



                     //-------SubMode Details--------//                               <--------------------- most important Fcts

void SelectPatientText(){
 SPI_Glcd_Set_Font(Font_Glcd_Character8x7 ,8, 7, 32);
 SPI_Glcd_Write_Text("            ", 25, 0, 1);
 SPI_Glcd_Write_Text("Patient N  ", 25, 1, 1);
 SPI_Glcd_Circle(108, 8, 2, 1);
 SPI_Glcd_Write_Text("           ", 25, 2, 1);
 SPI_Glcd_Write_Text("           ", 25, 4, 1);
 //SPI_Glcd_Box(58, 24, 73, 30, 1);
}

void SelectPatientAnimation (char patientNumber, char* removePrevious){
 if(intcon.tmr0if==1){
  if(*removePrevious){                          // to remove the previous rect one time :p
   //SPI_Glcd_Box(63,24,68,30,0);
   SPI_Glcd_write_char(' ', 63, 3, 1);
   *removePrevious=0;
  }
  SPI_Glcd_write_char(':', 115, 1, 2);
  SPI_Glcd_write_char(patientNumber+48, 63, 3, 2); // if decided not pretty use backspace erase
  tmr0h=AnimationDelayH;
  tmr0l=AnimationDelayL;
  intcon.tmr0if=0;
 }
}

void ChangeToPatient(char Dir, char* patientNumber){
 // right now i'm using stable 3 patients for every mode using nav sub
 if(Dir)   (*patientNumber)++;
 else      (*patientNumber)--;
 if(*patientNumber == 4) *patientNumber=1;
 if(*patientNumber==0)   *patientNumber=3;
}

void Confirm(char textMode,char color){
 SPI_Glcd_Set_Font(Font_Glcd_System3x5 ,3, 5, 32);
 if (color==1){
  if(textMode==1)      SPI_Glcd_Write_Text("CONFIRM", 53, 4, 0);
  else if(textMode==4) SPI_Glcd_Write_Text("SHOULD I AVOID OBS", 30, 4, 0);
  else {
   //SPI_Glcd_Write_Text("         ", 51, 4, 1);
   SPI_Glcd_Write_Text("                ", 30, 4, 1); //remove >> and previous writing
   if(textMode==2)      SPI_Glcd_Write_Text("INVERT STATUS", 40, 4, 0);
   else if(textMode==3) SPI_Glcd_Write_Text("EXIT", 60, 4, 0);
  }
  //SPI_Glcd_Write_Text("YES  NO", 53, 5, 1);
  SPI_Glcd_Write_Text("<-- YES  NO -->", 37, 5, 1);
 }
 else if(color==0){
  if(textMode==1)      SPI_Glcd_Write_Text("       ", 53, 4, 1);
  else if(textMode==2) SPI_Glcd_Write_Text("             ", 40, 4, 1);
  else if(textMode==3) SPI_Glcd_Write_Text("    ", 60, 4, 1);
  else if(textMode==4) SPI_Glcd_Write_Text("                  ", 30, 4, 1);

  SPI_Glcd_Write_Text("               ", 37, 5, 1);                                  // removes the yes no text
 }
 if(textMode==1 || textMode==4)      SPI_Glcd_Set_Font(Font_Glcd_Character8x7 ,8, 7, 32);
 else if(textMode==2 || textMode==3) SPI_Glcd_Set_Font(FontSystem5x7_v2, 5, 7, 32);

 intcon.gie=~intcon.gie;
}



/*void AutoNav(){
 //SPI_Glcd_Fill(0x00);
 //CuteFace();
 PWM1_Start();
 PWM2_Start();
 while(okbtn){}
 PWM1_Stop();
 PWM2_Stop();


}*/










//---------------------- Patient Interface Start Here --------------------------
void PatientQAnswerText(){
 SPI_Glcd_Set_Font(Font_Glcd_Character8x7 ,8, 7, 32);
 SPI_Glcd_Write_Text("Plz Answer :", 25, 0, 1);
 //SPI_Glcd_Write_Text("           ", 25, 1, 1);
 SPI_Glcd_Set_Font(FontSystem5x7_v2, 5, 7, 32);
 SPI_Glcd_Write_Text("To Start Survey ", 25, 1, 1);
 SPI_Glcd_Box(63,24,68,30,0);
 SPI_Glcd_Write_Text("Tap OK", 45, 4, 1);
 while(OkBtn){}
 SPI_Glcd_Write_Text("YES  NO", 45, 4, 1);
}

void QHeaderText(char statusORAnswer){
 // 0=status , 1=answer
 SPI_Glcd_Set_Font(Font_Glcd_Character8x7 ,8, 7, 32);
 if (statusORAnswer==0) SPI_Glcd_Write_Text("Qst Status: ", 25, 0, 1);
 else                   SPI_Glcd_Write_Text("Qst Answers:", 25, 0, 1);
 SPI_Glcd_Set_Font(FontSystem5x7_v2, 5, 7, 32);
 SPI_Glcd_Box(63,24,68,30,0);
}

void PatientQOption(char questionNumber){
 switch (questionNumber){
  case 1:
   SPI_Glcd_Write_Text("have you eaten   ", 25, 1, 1);
   SPI_Glcd_Write_Text("your food 1 ?    ", 25, 2, 1);
  break;
  case 2:
   SPI_Glcd_Write_Text("just some random", 25, 1, 1);
   SPI_Glcd_Write_Text("writing 2?    ", 25, 2, 1);
  break;
  case 3:
   SPI_Glcd_Write_Text("to tweak and    ", 25, 1, 1);
   SPI_Glcd_Write_Text("replace 3?      ", 25, 2, 1);
  break;
  case 4:
   SPI_Glcd_Write_Text("blablabla     ", 25, 1, 1);
   SPI_Glcd_Write_Text("my number is 4 ?   ", 25, 2, 1);
  break;
 }
}

void ShowPatientQInfo (char patientQinfo, char statusORAnswer){
 SPI_Glcd_Write_Text(">>", 30, 4, 1);
   switch (statusORAnswer){
    case 0: // <-- Status
     if(patientQinfo == 1) SPI_Glcd_Write_Text("Disabled", 45, 4, 1);
     else                  SPI_Glcd_Write_Text("Enabled ", 45, 4, 1);
    break;
    case 1: // <-- Anwser
     switch (patientQinfo){
      case 0:              SPI_Glcd_Write_Text(" Passed ", 45, 4, 1); break;
      //case 1:              SPI_Glcd_Write_Text("        ", 45, 4, 1); break;
      case 2:              SPI_Glcd_Write_Text("   No   ", 45, 4, 1); break;
      case 3:              SPI_Glcd_Write_Text("   Yes  ", 45, 4, 1); break;
     }
    break;
   }
}

void ChangeToQuestion(char Dir, char* questionNumber){
 if(Dir)   (*questionNumber)++;
 else      (*questionNumber)--;
 if(*questionNumber == 5) *questionNumber=1;
 if(*questionNumber == 0) *questionNumber=4;
}

void ChangeToQuestion2(char Dir, char* questionNumber){
 if(Dir)   (*questionNumber)++;
 else      (*questionNumber)--;
 if(*questionNumber == 5) *questionNumber=1;
 if(*questionNumber == 0) *questionNumber=4;
}




void SensorText(){
 SPI_Glcd_Set_Font(Font_Glcd_Character8x7 ,8, 7, 32);
 SPI_Glcd_Write_Text("Sensor Stat", 25, 0, 1);
 SPI_Glcd_Write_Text("           ", 25, 2, 1);
 SPI_Glcd_Write_Text("           ", 25, 4, 1);
 
 SPI_Glcd_Write_Text("           ", 25, 3, 1);
 SPI_Glcd_Write_Text("           ", 25, 5, 1);

 SPI_Glcd_Set_Font(Font_Glcd_System3x5 ,3, 5, 32);
 SPI_Glcd_Write_Text("      ULTRASONIC:    ", 25, 1, 1);
 SPI_Glcd_Circle(55,  25, 6, 1);
 SPI_Glcd_Circle(85,  25, 6, 1);
 SPI_Glcd_Write_Text("RIR:", 30, 4, 1);
 SPI_Glcd_Write_Text(":LIR", 95, 4, 1);
 SPI_Glcd_Rectangle(50, 35, 90, 43, 1);
 SPI_Glcd_V_Line(35, 43, 63, 1);
 SPI_Glcd_V_Line(35, 43, 76, 1);
}

void SpecailMenu(){
 SPI_Glcd_Set_Font(Font_Glcd_Character8x7 ,8, 7, 32);
 SPI_Glcd_Write_Text("Reset Menu:", 25, 0, 1);
 SPI_Glcd_Set_Font(Font_Glcd_System3x5 ,3, 5, 32);
 SPI_Glcd_Write_Text("EXIT             ", 30, 1, 1);
 SPI_Glcd_Write_Text("DISABLE ALL QUESTIONS", 30, 2, 1);
 SPI_Glcd_Write_Text("RESET QUESTIONS INFO", 30, 3, 1);
 SPI_Glcd_Write_Text("RESET ROAD INFO      ", 30, 4, 1);
 SPI_Glcd_Write_Text("FACTORY RESET", 30, 5, 1);


}

void SpecialHyphen(char option, char i){
 switch (option){
  case 1:
   SPI_Glcd_H_Line(26, 28, 12, i);
  break;
  case 2:
   SPI_Glcd_H_Line(26, 28, 20, i);
  break;
  case 3:
   SPI_Glcd_H_Line(26, 28, 28, i);
  break;
  case 4:
   SPI_Glcd_H_Line(26, 28, 36, i);
  break;
  case 5:
   SPI_Glcd_H_Line(26, 28, 44, i);
  break;
 }
}

void ChangeToSOption(char Dir, char* sOption) {
 // Navigation:1  Pool(statistics):2  Soon(:p):3
 if(Dir)      (*sOption)++;
 else         (*sOption)--;
 if(*sOption==6) *sOption=1;
 if(*sOption==0) *sOption=5;
}

void SOptionAnimation(char sOption,char* removePrevious){
 if(*removePrevious){                          // to remove the previous rect one time :p
  SpecialHyphen(*removePrevious,0);
  *removePrevious=0;
 }
 SpecialHyphen(sOption,2);
}