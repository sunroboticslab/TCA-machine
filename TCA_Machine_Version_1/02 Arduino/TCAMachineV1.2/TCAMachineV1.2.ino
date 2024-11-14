// For RAMPS 1.4
//#include <Esplora.h>
//Version, 7/5/2019, By Brandon Tighe, Jiefeng Sun. at colorado state university. 
//Library AccelStepper.h is available at http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#aa279a50d30d0413f570c692cff071643
// Note: Libray MUST BE ALTERED by the following:
//    In the lib file AccelStepper.CPP:
//      search "void    AccelStepper::disableOutputs()" and change LOW to HIGH
//      search "void    AccelStepper::enableOutputs()" and change HIGH to LOW
//
//
//


#include <AccelStepper.h> ////## VERY IMPORTANT,  I switched the Enableoutput and Disableoutput. 
// E0,E1 are for the mandrel.
// Z is for twisting.
// X is for lead screw motor controling traveler.
// Y is the lead screw motor for controlling the position of the twisting motor. 

//address pins.....................................................................
//MOTORS..........................................................
//Motor X 
#define X_STEP_PIN         54       //
#define X_DIR_PIN          55       // selects direction (phase)
#define X_ENABLE_PIN       38       // speed control (enable)
#define X_MIN_PIN           3       // mechanical stop
#define X_MAX_PIN           2       // mechanical stop

//Motor Y
#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
//#define Y_MIN_PIN          14      
//#define Y_MAX_PIN          15      //only need two mechanical stops, using this for FOR/REV switch

//Motor Z
#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

//Motor E0
#define E0_STEP_PIN         26
#define E0_DIR_PIN          28
#define E0_ENABLE_PIN       24

//Motor E1
#define E1_STEP_PIN         36
#define E1_DIR_PIN          34
#define E1_ENABLE_PIN       30

//CONTROL.................................................................
  #define DirSwitch           40      //Choose direction. note: must use pulldown resistor for button
  #define Button1             42     //Button for Mode 1
  #define Button2             44 
  #define Button3             57 
  #define Button4             58 
  #define Button5             63 
  
  //byte DirSwitch=16;                
  bool Direction=              0;
  int SpeedPot=               A5;     // Potentiometer
//Various definitions.....................................................
//
#define ERROR_WINDOW 20  // +/- this value
#define BUTTONDELAY 200

////Not used (for 3D printer heating elements/thermistors/etc.)
//#define HEATER_0_PIN       10       //
//#define HEATER_1_PIN       8        //
//#define TEMP_0_PIN          13      // 
//#define TEMP_1_PIN          14      // 
//#define SDPOWER            -1       //
//#define SDSS               53       //
//#define LED_PIN            13       //
//
//#define FAN_PIN            9        //
//
//#define PS_ON_PIN          12       //
//#define KILL_PIN           -1       //


int val=0;
int mode = 0;                         //Sets "mode" that corresponds with a button and its specific motor combinations
const int stepsPerRevolution = 200;   // change this to fit the number of steps per revolution for your motor***
int button=0;
bool X_MIN=0;
bool X_MAX=0;
//int SpeedAdjust;                  // adjustment from pot. input
float AnalogValue;                  // exact pot. input
int CoilSpeed;                    // speed of mandrels
bool ButtonOne;
bool ButtonTwo;
bool ButtonThree;
bool ButtonFour;
bool ButtonFive;

byte k=0;                           // for using relative .move in mode1 
byte n=0;

//use Library to set motor "Class"    ***note: for more information on this type of coding to to https://www.arduino.cc/en/Hacking/LibraryTutorial
AccelStepper stepE0(AccelStepper::DRIVER, E0_STEP_PIN, E0_DIR_PIN);  // 1 step 2 dir
AccelStepper stepE1(AccelStepper::DRIVER, E1_STEP_PIN, E1_DIR_PIN);  // 
AccelStepper stepX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);     // 
AccelStepper stepY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);     // 
AccelStepper stepZ(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);     // 

//**********************************************************************
// MOTOR SETTINGS
//**********************************************************************
int tempDis = 3000;
float speedRatio = .629;           //this ratio is different for each TCA type (i=.4857, k=.32, N= .629)
float untwistRatio= .2;            //z motor untwists to compensate transfered twist while coiling
//float globalAdjust= 1.81;        //Adjusts the overall motor speed (if Potentiometer is at 100%)
const int MaxSpeed = 900;          //sets max speeed
float SetCoilSpeed = 800;          // speed for coiling mandril


long TravelerSteps = 96000;        //aprx to end of lead screw
long MandralSteps = 800000;        //mandral coiling motor distance setting (arbitrary)
long twisting = 198000;            //calculate for natural thread length using excel file 
                                   //144000;//33000; //25050; //20000;// .// twisting stroke, divided by 200, 720 rotations
/////*******************************************************************

//Serial buttons


// 
void setup()
{  
  Serial.begin(115200);
//MOTORS...............................................................
//was Stepper1
  stepE0.disableOutputs();               //it will disable all steppers (allows motor to spin freely)
  stepE0.setMaxSpeed(MaxSpeed);
  stepE0.setAcceleration(200);
  stepE0.setEnablePin(E0_ENABLE_PIN);
  stepE0.setMinPulseWidth(20);
  
//was Stepper2
  stepE1.disableOutputs();              //it will disable all steppers
  stepE1.setMaxSpeed(MaxSpeed);
  stepE1.setAcceleration(200);
  stepE1.setEnablePin(E1_ENABLE_PIN);
  stepE1.setMinPulseWidth(20);
  
//was Stepper3  
  stepX.disableOutputs(); 
  stepX.setMaxSpeed(MaxSpeed);
  stepX.setEnablePin(X_ENABLE_PIN);
  stepX.setAcceleration(200);
  stepX.setMinPulseWidth(20);
  
//was Stepper4  
  stepY.disableOutputs(); 
  stepY.setMaxSpeed(MaxSpeed);
  stepY.setEnablePin(Y_ENABLE_PIN);
  stepY.setAcceleration(200);
  stepY.setMinPulseWidth(20);
  
//new motor  
  stepZ.disableOutputs(); 
  stepZ.setMaxSpeed(MaxSpeed);
  stepZ.setAcceleration(200);
  stepZ.setEnablePin(Z_ENABLE_PIN);
  stepZ.setMinPulseWidth(20);

//MECHANICAL STOPS.........................................................
  pinMode(X_MIN_PIN,INPUT);             //Bottom stop
  pinMode(X_MAX_PIN,INPUT);             //Top stop

//CONTROL..................................................................
  pinMode(DirSwitch,INPUT);             //used to select Forward or Reverse
  pinMode(SpeedPot,INPUT);
  pinMode(Button1,INPUT);
  pinMode(Button2,INPUT);
  pinMode(Button3,INPUT);
  pinMode(Button4,INPUT);
  pinMode(Button5,INPUT);
  
  }
  
// 2) adjust variable names and class names (stepper1.blah blah 
void loop()
{
  Direction= digitalRead(DirSwitch);           // note (-) sign to ensure REVERSE button is correct
  AnalogValue= analogRead(SpeedPot); 
  int range = map(AnalogValue, 0, 1024, 0, 5);

switch (range) {
case 0: //Potentiometer turned up to 0-25%
  CoilSpeed= 140;
  break;

case 1:   //Potentiometer turned up to 26-50%
  CoilSpeed= 330;
  break;

case 2:   //Potentiometer turned up to 51-75%
  CoilSpeed= 520;
  break;

case 3:   //Potentiometer turned up to 76-100%
  CoilSpeed= 685; 
  
  break;
  
case 4:   //Potentiometer turned up to 76-100%
  CoilSpeed = MaxSpeed; 

case 5:   //Potentiometer turned up to 76-100%
  CoilSpeed = MaxSpeed; 
 
  break;

}




  
  // float SpeedAdjust= (AnalogValue/1023);
 //  float CoilSpeed= SetCoilSpeed*(SpeedAdjust);
  
 //float CoilSpeed= 685; //to test constant speed
  
  float TravelSpeed = CoilSpeed*speedRatio;     // proportionate with CoilSpeed, will decide the helical angle.  
  
// Physical butons-------------------------------------------------------------
  ButtonOne= digitalRead(Button1);
  ButtonTwo= digitalRead(Button2);
  ButtonThree= digitalRead(Button3);
  ButtonFour= digitalRead(Button4);
  ButtonFive= digitalRead(Button5);
  
// Virtual butons--------------------------------------------------------------
   if (Serial.available() >= 1) {
      // read the incoming byte:
       button = Serial.parseInt();        //must have this for correct decimal integer serial read
    
       // say what you got:
      Serial.print("I received: ");
      Serial.println(button);
      
    }
//         Serial.print("ButtonOne: ");   
//         Serial.print(ButtonOne);
//         Serial.print("ButtonTwo: ");   
//         Serial.print(ButtonTwo);
//         Serial.print("ButtonThree: ");   
//         Serial.print(ButtonThree);
//         Serial.print("ButtonFour: ");   
//         Serial.print(ButtonFour);
//         Serial.print("ButtonFive: ");   
//         Serial.print(ButtonFive);
         
         Serial.print("Percent Speed:");   
//         Serial.print(SpeedAdjust*100);
         Serial.print("   ");
         Serial.print(CoilSpeed); 
         Serial.print("   ");
         Serial.print(TravelSpeed); 
         Serial.print("   ");         
//         Serial.print("CoilSpeed: ");
//         Serial.print(CoilSpeed);
//         Serial.print("   ");
         Serial.print("  Direction: ");
         Serial.print(Direction);
         Serial.println("  MODES: [1:Twist] [2:Coil] [3:Coil Copper] [4:Mandril Only] [5:Home Travelers]");
//           X_MIN= digitalRead(X_MIN_PIN);           
//           Serial.println(X_MIN);
                  


//     //For mechanical buttons
//   int   val =0;                        // variable to store the read value
//     digitalWrite(analogPin, HIGH);     // enable the 20k internal pullup
//     val = analogRead(analogPin);       // read the input pin
//     Serial.println(val);
//     
     //==========================================================================================
     //SET MODES
     //==========================================================================================

     //BUTTON ONE [forward]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     //twist thread
             
     if ((ButtonOne==1 || button ==1) && Direction==0) { 
     
     //uses resistors to identify button 1
     Serial.print("Mode 1 Forward");
          
     //sets parameters for motors on MODE 1
     stepE1.enableOutputs();                       //enables output 
     stepZ.enableOutputs() ;                       //enables output 

     //note this uses relative position ".move" opposed to absolute ".moveTo"
     stepE1.move( long (-twisting));         //sets number of steps motor takes *note E1 (-)is CCW (twist thread this dir), (+) is CW
     stepZ.move( long (-twisting));          //note motor Z is (-) CW, (+) CCW(this changed???)
     
     stepE1.setSpeed(MaxSpeed);                    
     stepZ.setSpeed(MaxSpeed);                     // the traveler moving velocity 45 for 1 thread,??? 
      
      Serial.print("E1 DistToGo:");
      Serial.println(stepE1.distanceToGo ());

      mode =1; 

      }

     //BUTTON ONE [reverse]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     //twist thread
             
     if ((ButtonOne==1 || button ==1) && Direction==1) { 
     
     //uses resistors to identify button 1
     Serial.print("Mode 1 Reverse");
  
     
     //sets parameters for motors on MODE 1
     stepE1.enableOutputs();                       //enables output 
     stepZ.enableOutputs() ;                       //enables output 
     
     stepE1.move( long (twisting));         //sets number of steps motor takes *note E1 (-)is CCW, (+) is CW
     stepZ.move( long (twisting));          //note motor Z is (-) CW, (+) CCW
     
     stepE1.setSpeed(MaxSpeed);                    
     stepZ.setSpeed(MaxSpeed);                     // the traveler moving velocity 45 for 1 thread,??? 

      Serial.print("E1 DistToGo:");
      Serial.println(stepE1.distanceToGo ());
      
      mode =1; 

      }

     //BUTTON TWO [forward]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // Coil around mandril     
     
     else if ((ButtonTwo==1 || button ==2) && Direction==0 ) {   //
      Serial.print("Mode 2 Forward");

     stepE0.enableOutputs();                      //enables output 
     stepE1.enableOutputs();                      //enables output 
     stepX.enableOutputs();                       //enables output 
     stepY.enableOutputs();                       //enables output 
     stepZ.enableOutputs();                       //enables output 
     

     stepE0.moveTo(long(MandralSteps));           // (+)steps is CCW (forward for coiling)
     stepE1.moveTo(long (-MandralSteps));       
     stepX.moveTo(-TravelerSteps);                //
     stepY.moveTo(long(-TravelerSteps));          
     stepZ.moveTo( long (MandralSteps));       
     


     stepE0.setSpeed(CoilSpeed);                  // use this after calling moveTo();
     stepE1.setSpeed(CoilSpeed);                  //
     stepX.setSpeed(TravelSpeed);                 //
     stepY.setSpeed(TravelSpeed);                 //    
     stepZ.setSpeed(CoilSpeed*untwistRatio);      // specific ratio to prevent self coiling of thread due to over-twist

      Serial.print("E1 DistToGo:");
      Serial.println(stepE1.distanceToGo ());
                   
      mode = 2;  
      }
     //BUTTON TWO [reverse]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // Coil around mandril     
     
     else if ((ButtonTwo==1 || button ==2) && Direction==1 ) {   // 
      Serial.print("Mode 2 Reverse");
            
     stepE0.enableOutputs();                      //enables output 
     stepE1.enableOutputs();                      //enables output 
     stepX.enableOutputs();                       //enables output 
     stepY.enableOutputs();                       //enables output 
     stepZ.enableOutputs();                       //enables output 
     

     stepE0.moveTo(long(-MandralSteps));           // (+)steps is CCW (forward for coiling)
     stepE1.moveTo(long (MandralSteps));       
     stepX.moveTo(TravelerSteps);                 //
     stepY.moveTo(long(TravelerSteps));          
     stepZ.moveTo( long (-MandralSteps));       
     

     stepE0.setSpeed(CoilSpeed);                  // use this after calling moveTo();
     stepE1.setSpeed(CoilSpeed);
     stepX.setSpeed(TravelSpeed);                 
     stepY.setSpeed(TravelSpeed);
     stepZ.setSpeed(CoilSpeed*untwistRatio);

      Serial.print("E1 DistToGo:");
      Serial.println(stepE1.distanceToGo ());
                   
      mode = 2;  
      }
      
     //BUTTON THREE [forward]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // Coil COPPER around mandril (no z motor)
     
     else if ((ButtonThree==1 || button ==3) && Direction==0) {   
     Serial.print("Mode 3 Forward");
   
     stepE0.enableOutputs();                      //enables output 
     stepE1.enableOutputs();                      //enables output 
     stepX.enableOutputs();                       //enables output 
     stepY.enableOutputs();                      //enables output 

     

     stepE0.moveTo(long(MandralSteps));          
     stepE1.moveTo(long (-MandralSteps));       
     stepX.moveTo(-TravelerSteps);                       //calculate new value for 1m x t8 x 2mmPitch lead screw
     stepY.moveTo(long(-TravelerSteps*.9));          

     

     stepE0.setSpeed(CoilSpeed);                  // use this after calling moveTo();
     stepE1.setSpeed(CoilSpeed);
     stepX.setSpeed(TravelSpeed);                 // the traveler moving velocity 45 for 1 thread, 
     stepY.setSpeed(TravelSpeed);

      Serial.print("E1 DistToGo:");
      Serial.println(stepE1.distanceToGo ());
      
       mode = 3;         
      }

     //BUTTON THREE [reverse]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // Coil COPPER around mandril (no Z motor)   
     
     else if ((ButtonThree==1 || button ==3) && Direction==1) {   
     Serial.print("Mode 3 Reverse");
   
     stepE0.enableOutputs();                      //enables output 
     stepE1.enableOutputs();                      //enables output 
     stepX.enableOutputs();                       //enables output 
     stepY.enableOutputs();                      //enables output 

     

     stepE0.moveTo(long(-MandralSteps));          
     stepE1.moveTo(long (MandralSteps));       
     stepX.moveTo(TravelerSteps);                       //calculate new value for 1m x t8 x 2mmPitch lead screw
     stepY.moveTo(long(TravelerSteps*.9));          

     

     stepE0.setSpeed(CoilSpeed);                  // use this after calling moveTo();
     stepE1.setSpeed(CoilSpeed);
     stepX.setSpeed(TravelSpeed);                 // the traveler moving velocity 45 for 1 thread, 
     stepY.setSpeed(TravelSpeed);

      Serial.print("E1 DistToGo:");
      Serial.println(stepE1.distanceToGo ());
      
       mode = 3;         
      }
      
     //BUTTON FOUR [forward]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // turn mandril only 
     else if ((ButtonFour==1 || button ==4) && Direction==0) {   

      Serial.print("Mode 4 Forward");
     stepE0.enableOutputs();                      //enables output  
     stepE1.enableOutputs();                      //enables output 
     
     stepE0.moveTo(long(MandralSteps));         //(+)CCW rotation (forward for coiling)
     stepE1.moveTo(long (-MandralSteps));       //must be opposite dir as above
     
      stepE0.setSpeed(CoilSpeed);
      stepE1.setSpeed(CoilSpeed);
     
      mode = 4;          
      }

     //BUTTON FOUR [reverse]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // turn mandril only 
     else if ((ButtonFour==1 || button ==4) && Direction==1) {   

      Serial.print("Mode 4 Reverse");
     stepE0.enableOutputs();                      //enables output  
     stepE1.enableOutputs();                      //enables output 
     
     stepE0.moveTo(long(-MandralSteps));         //(+)CCW rotation (forward for coiling)
     stepE1.moveTo(long (MandralSteps));       //must be opposite dir as above
     
      stepE0.setSpeed(CoilSpeed);
      stepE1.setSpeed(CoilSpeed);
     
      mode = 4;          
      }
     //BUTTON FIVE [forward]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // home travelers
     
  else if ((ButtonFive==1 || button ==5) && Direction==0) { 
          Serial.print("Mode 5 Forward");
          
     stepX.enableOutputs();             //X is traveler lead screw
     stepX.moveTo(TravelerSteps);       //(+) moves to top of machine **was MandralSteps
     stepX.setSpeed(CoilSpeed);
     
     stepY.enableOutputs();             //Y is adjustable motor lead screw
     stepY.moveTo(TravelerSteps);       //(+) moves to top of machine 
     stepY.setSpeed(CoilSpeed); 
   
      mode = 5;          
      }

     //BUTTON FIVE [reverse]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // home travelers
     
  else if ((ButtonFive==1 || button ==5) && Direction==1) {
          Serial.print("Mode 5 Reverse");
          
     stepX.enableOutputs();                 //X is traveler lead screw
     stepX.moveTo(-MandralSteps);           //(+) moves to top of machine
     stepX.setSpeed(CoilSpeed);

     
     stepY.enableOutputs();                 //Y is adjustable motor lead screw
     stepY.moveTo(-MandralSteps);           //(+) moves to top of machine 
     stepY.setSpeed(CoilSpeed); 
   
      mode = 5;          
      }
    
 //================================================================================================
 //MODE CHECK AND MOTOR COMMAND
 //================================================================================================  
         while( mode == 1 &&k==0 )   // Twist thread 
      {
      ButtonOne= digitalRead(Button1);      //Read buttonOne in loop to exit loop when switch is off
         
      stepE1.runSpeedToPosition();          // command to start motor
      stepZ.runSpeedToPosition();
//      Serial.print("Z Pos:");
//      Serial.print(stepZ.currentPosition ());
//      Serial.print("E1 Pos:");
//      Serial.println(stepE1.currentPosition ());


      
    //Stop conditons     *consider splitting this into if/ else if to keep on/off function of twist
      if (( abs(stepZ.distanceToGo()) <10)|| ButtonOne==0){ 
      k=1;         
      mode = 0;
      }
      }
//---------------------------------------------------------------------------       
    while(mode == 2)   // Coil around mandril
      {
      ButtonTwo= digitalRead(Button2);
      k=0;  //to disable mode1 rerun prevention
        
        X_MIN= digitalRead(X_MIN_PIN);      //Mechanical stop bottom
        X_MAX= digitalRead(X_MAX_PIN);      //Mechanical stop top
        
 //Serial.println(stepY.distanceToGo());
 
        stepE0.runSpeedToPosition();
        stepE1.runSpeedToPosition();  // command to start motor
        stepX.runSpeedToPosition();
        stepY.runSpeedToPosition();  // command to start motor
        stepZ.runSpeedToPosition();


//Raise and lower Z motor to transfer uneven twist*****************************************************
   if (((abs(stepY.distanceToGo()))< (abs(2*TravelerSteps/3)))&& n==0){          //if travelers have moved 1/3 set distance 
        
   stepY.move(TravelerSteps/3);            //Move Y traveler only up 1/3 of set distance; (+) moves to top of machine 
   stepY.setSpeed(TravelSpeed);
   delay(1000);
   
   n=1;                                     //count placeholder  
     while(abs(stepY.distanceToGo())>10){       //While loop for stepY.runSpedtoPosition
     stepY.runSpeedToPosition();
     }

     
   if (n==1) {    //Stopping condition, direction change
           
   stepY.move(-TravelerSteps/3);           //(+) moves to top of machine 
   stepY.setSpeed(TravelSpeed);
   
   n=2;  
     //while(abs(stepY.distanceToGo())>10){
       while(n==2){
     stepY.runSpeedToPosition();
       if ( (stepY.distanceToGo() == 0) || (X_MIN==0) || (X_MAX==0)|| ButtonTwo==0)
        mode = 0;
        n==0;
     }
   stepY.moveTo(-TravelerSteps); 
   stepY.setSpeed(TravelSpeed);
   
   }        //(+) moves to top of machine 
      }
//*****************************************************************************************************

             
   if ( (stepY.distanceToGo() == 0) || (X_MIN==0) || (X_MAX==0)|| ButtonTwo==0)
        mode = 0;
      }

//---------------------------------------------------------------------------   
    while(mode == 3 )  //Reverse coil around mandril
      {
        ButtonThree= digitalRead(Button3);
        k=0;  //to disable mode1 rerun prevention
        
        X_MIN= digitalRead(X_MIN_PIN);      //Mechanical stop bottom
        X_MAX= digitalRead(X_MAX_PIN);      //Mechanical stop top
        
        // Serial.println(long (twisting +stepY.distanceToGo())/200);
        stepE0.runSpeedToPosition();
        stepE1.runSpeedToPosition();  // command to start motor
        stepX.runSpeedToPosition();
        stepY.runSpeedToPosition();  // command to start motor
             
        if ( stepY.distanceToGo() == 0 || (X_MIN==0) || (X_MAX==0)||ButtonThree==0)
        mode = 0;
       }

//---------------------------------------------------------------------------  
     while( mode == 4 )   // Mandril only
      {
        ButtonFour= digitalRead(Button4);
        k=0;  //to disable mode1 rerun prevention
        
     // Serial.println(stepE0.distanceToGo());
     // Serial.println('n');
      stepE0.runSpeedToPosition();
      stepE1.runSpeedToPosition();

            if (( stepE0.distanceToGo() == 0)||ButtonFour==0)
      mode = 0;
      }

//---------------------------------------------------------------------------  
         while( mode == 5 )   // Home Travelers
      {
        ButtonFive= digitalRead(Button5);
        k=0;  //to disable mode1 rerun prevention
        
      // Serial.println(stepE1.distanceToGo()); //Serial.print('N') 
      
      X_MIN= digitalRead(X_MIN_PIN);      //Mechanical stop bottom
      X_MAX= digitalRead(X_MAX_PIN);      //Mechanical stop top
      
      stepX.runSpeedToPosition();
      stepY.runSpeedToPosition();
      if ( stepY.distanceToGo() == 4  || (X_MIN==0) || (X_MAX==0)||ButtonFive==0){
      mode = 0;
      }
      }

   
      
    stepE0.disableOutputs();
    stepE1.disableOutputs();
    stepX.disableOutputs();
    stepY.disableOutputs();
    stepZ.disableOutputs();
       mode = 0;         
  }
