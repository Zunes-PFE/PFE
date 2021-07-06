
void NoIntrDelay(char Nx100);
void Debug();
void CuteFace();        // To Make Cuter ... :p
// to add cool face(auto) and nerd face (learning) + manual face hhhh ;p

//----------------------- Mode functions Start Here ----------------------------
void ModeMenu();
void ChangeToMode(char Dir, char* selectedMode);
void MALRectangular(char operationMode,char i);
void ModeSelectionAnimation(char selectedMode,char* removePrevious);


//---------------------- SubMode Functions Start Here --------------------------
void SubModeMenu(char selectedMode );
void ChangeToSubMode(char Dir, char* selectedSubMode);
void Hyphen(char selectedSubMode, char i);
void SubModeSelectionAnimation(char selectedSubMode,char* removePrevious);


                     //-------SubMode Details--------//
                   //-------Navigation SubMode--------//
void SelectPatientText();
void SelectPatientAnimation(char patientNumber, char* removePrevious);
void ChangeToPatient(char Dir, char* patientNumber);
void Confirm(char textMode,char i);
//void AutoNav(); // needs a lot of work
//void LearnNav(); // needs more then lot of work



//---------------------- Patient Interface Start Here --------------------------
void PatientQAnswerText();
void QHeaderText(char statusORAnswer);
void PatientQOption(char questionNumber); // Still not
void ShowPatientQInfo(char patientQinfo, char statusORAnswer);
void ChangeToQuestion(char Dir, char* questionNumber);
void ChangeToQuestion2(char Dir, char* questionNumber);       // to resolve reentrency problem







void SensorText();
void SpecailMenu();
void SpecialHyphen(char option, char i);
void ChangeToSOption(char Dir, char* sOption);
void SOptionAnimation(char sOption,char* removePrevious);


//void WalkNcheck(char Dir);