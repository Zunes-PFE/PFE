//-------------------- External Declaration Start Here -------------------------
extern char selectedMode, removePrevious, selectedSubMode, patientNumber, questionNumber, sOption, addedRotation;
extern bit sameMode, sameSubMode, onePass, samePatient, sameQuestion, sameSOption, subCompleted,addedActions;
extern char patientQinfo;
extern char ramAdress;
extern struct RoadInstructions temp, roadInfo[];
extern int Tempoo,addedPulses;

//--------------------- Macro Declaration Start Here ---------------------------

void __InitMcu();
void __InitGlcd();
void __ModeSelectionMacro();
void __SubModeSelectionMacro();
void __InitPatient();
void __PatientSelectionMacro();
void __InitQuestion(char statusORAnswer);
void __QusetionSelectionMacro(char statusORAnswer);            //<-- big one
void __ReadPatientQInfo(char justTheQInfo);
void __WrightPatientQInfo(char newPatientQinfo);
void __AskQwaitR();
void __LearnNav();
void __WrightRoad();
void __AutoNav(short Task);       // 1 for going -1 for retracting
void __ManualNav(short Task);
void __ExecutePulses(unsigned long int pulses,char Dir);
void __ExecutePulsesNoUltrasonic(unsigned long int pulses,char Dir);
void __CalibrateRobot();
void InTrackCalibration(char ForB);
void OutTrackCalibration();
char FindLine();
void __MoveTostartPoint();
void __SensorStatusMacro(char SpecialMenu);
void __SOptionSelectionMacro();
void __ExecuteSpecialOption(char option);


//
//
//
// 7


void __checkAvailableSol();
char __AvoidObstacleMacro();







