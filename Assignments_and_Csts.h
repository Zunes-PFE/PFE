
//----------------------- Headers and struct types -----------------------------

struct RoadInstructions {
 char dominatedCommand;
 short nDeci, nCenti, nPulse;
 int   nDeg, nMeter;
} ;

//------------------------ Periphiral Assignement ------------------------------

// Btns connections
sbit ValidateBtn at portb.b2;
//sbit YesBtn      at porta.b3;
sbit OkBtn       at porta.b4;
//sbit NoBtn       at porta.b5;

// End Btns connections

// RotEncoders connections
sbit RotSelDir  at porta.b0;
//sbit RotMot1Dir at porta.b1;
//sbit RotMot2Dir at porta.b2;
// End RotEncoders connections

// Motor Driver connections
sbit M2DirW     at latd.b1;
sbit M1DirW     at latd.b2;
sbit M2DirR     at portd.b1;
sbit M1DirR     at portd.b2;
sbit EN1        at latc.b2;
sbit EN2        at latc.b1;
sbit mPulsesPin at latb.b1;
// END Motor Driver connections

//IR Sensors
sbit RIR at portd.b0;
sbit LIR at portd.b3;
//End IR Sensors

//Ultrasonic Sensor
sbit Trig at late.b0;
sbit Echo at portb.b5;
//End Ultrasonic Sensor

// Port Expander module connections
sbit  SPExpanderRST           at RC0_bit;
sbit  SPExpanderCS            at RC6_bit;// WAS CHANGED TO RC6 TO LET RC1 FOR PWM2
sbit  SPExpanderRST_Direction at TRISC0_bit;
sbit  SPExpanderCS_Direction  at TRISC6_bit;
// End Port Expander module connections



//---------------------------- Csts variables ----------------------------------

#define DefaultMode           2
#define DefaultSubMode        1
#define DefaultPatientNumber  1
#define DefaultQuestionNumber 1

#define ModePushDelayH  0xca // was fd
#define ModePushDelayL  0xac
#define AnimationDelayH 0xEC
#define AnimationDelayL 0xD0
#define WaitQDelayH     0x00    // ;p
#define WaitQDelayL     0x00

#define FirstPatientRoadInfoAdr  0x00
#define FirstPatientEndRoadAdr   0x4F    //dec: 79
#define FirstPatientQInfoAdr     0xF0    //dec: 240

#define SecondPatientRoadInfoAdr 0x50    //dec: 80
#define SecondPatientEndRoadAdr  0x9F    //dec: 159
#define SecondPatientQInfoAdr    0xF1    //dec: 241

#define ThirdPatientRoadInfoAdr  0xA0    //dec: 160
#define ThirdPatientEndRoadAdr   0xE9    //dec: 239
#define ThirdPatientQInfoAdr     0xF2    //dec: 242


#define ClaculatedPulseToCenti 0x19      //dec: 25
#define ClaculatedPulseTo4Deg  0x15      //dec: 21   // to 4 degrees ;p

#define MotorSpeedDelay 1500

#define OneSecDelayH 0x67
#define OneSecDelayL 0x69

#define HalfMeterDelay 3471