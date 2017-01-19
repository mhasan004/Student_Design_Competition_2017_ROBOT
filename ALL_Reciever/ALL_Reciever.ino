//Reciever will have the motors
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24_config.h>
#define CE_PIN   8
#define CSN_PIN  53
RF24 radio(CE_PIN, CSN_PIN);  
const uint64_t pipe = 0xE8E8F0F0E1LL;   
 
#include "DualVNH5019MotorShield.h"                     //This is the Pololu DC Motor driver library
DualVNH5019MotorShield motors;                          //Motor1 will be the lifting motor. Motor2 i dont kow yet, will be the ball hitting motor i think
#include <Servo.h>  
Servo LeftMotor1;        
Servo LeftMotor2;
Servo LeftMotor3;
Servo RightMotor1;
Servo RightMotor2;
Servo RightMotor3;
int LeftMotor_pin1 = 8;
int LeftMotor_pin2 = 9;
int LeftMotor_pin3 = 10;  
int RightMotor_pin1 = 5;
int RightMotor_pin2 = 6;
int RightMotor_pin3 = 7;
int Motor1; int Motor2; int Motor3; int Motor4; int Motor5; int Motor6;   //declaring the data.ESC->Motor values  
int micro_sw1 = 40;                           //(1&2)LIMIT Switches used for Lifting Mode to know if the max/min height is reached. 
int micro_sw2 = 41; 
int micro_sw3 = 42;                           //(3&4) LIMIT Switches used for BALL HITTING Mode, need more info
int micro_sw4 = 43;               
int led = 13;                                 //this is an LED that will be connected to pin 13 and ground of the arduino, it will lightup if the arduino is in ARMING mode in the SETUP

struct controllerData{                        
  int ESC0; int ESC_1;int ESC_2;int ESC_3;int ESC_4;int ESC_5;int ESC_6;     
  int motorSpeed_Pot;
  bool levelUP_sw;
  bool levelDOWN_sw;
  bool left_sw;    
  bool right_sw;
  bool lifting_sw;
  bool throwing_sw;
  bool climbing_sw;
  bool hitting_sw;   
  bool STOP_sw;
};controllerData data;

void setup(){                  
  Serial.begin(250000);
  Serial.println("Starting");
  pinMode(led, OUTPUT);
  motors.init();                                    //Initiate the DC motor shield
  radio.begin();                                      //Link The Radio
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15, 15);
  radio.openReadingPipe(1,pipe); 
  LeftMotor1.attach(LeftMotor_pin1);  
  LeftMotor2.attach(LeftMotor_pin2);  
  LeftMotor3.attach(LeftMotor_pin3);   
  RightMotor1.attach(RightMotor_pin1); 
  RightMotor2.attach(RightMotor_pin2);  
  RightMotor3.attach(RightMotor_pin3);                               
  Serial.print("Arming Test Starts in ");             //Get Ready to Arm in 3 seconds
  for(int i = 2; i > 0; i--){
    digitalWrite(led,HIGH);
    digitalWrite(RightMotor_pin1, HIGH);
    digitalWrite(RightMotor_pin2, HIGH);
    digitalWrite(RightMotor_pin3, HIGH);
    digitalWrite(LeftMotor_pin1, HIGH);
    digitalWrite(LeftMotor_pin2, HIGH);
    digitalWrite(LeftMotor_pin3, HIGH);
    Serial.print(i);
    Serial.print("... ");
    delay(1000);
  }//end of "get readt to Arm" loop
  Serial.println();  
  for (int k = 90; k <= 110; k++)
  { 
    digitalWrite(led,HIGH);
    Serial.print("ARM LOOP  ");
    Serial.print(k); 
    Serial.print("              ");                                    
    if (k >= 92 && k<= 97){      
      LeftMotor1.write(k);                          //Arm the 35 Amp ESC5  (92-97 ARM)(92-98 NEUTRAL)(0-15 dont work!)
      Serial.print("ARMING L5  ");
      Serial.print(k);
      delay(500);
    } 
    if (k >= 97 && k <= 101){       
      RightMotor2.write(k);                         //Arm the 35 Amp ESC2  (97-101 ARM)(93-101 NEUTRAL)(0-15 dont work!)
      Serial.print("           ");
      Serial.print("ARMING R2  ");
      Serial.print(k);
      delay(500);
    }     
    Serial.println();          
  }//end of ARMING loop
  digitalWrite(led,LOW);
  Serial.println("Arming Done and Starting Loop...");               
  radio.startListening(); 
}//End of Setup



unsigned long lastRecvTime = 0;
void loop() 
{
  while ( radio.available()) {
    radio.read(&data, sizeof(controllerData));
    lastRecvTime = millis(); //here we receive the data 
  }  
 
  
  unsigned long now = millis();                              
  Motor1 = data.ESC_1;     //(Left Motors:1-3)(Right Motors: 4-6)
  Motor2 = data.ESC_2;
  Motor3 = data.ESC_3;
  Motor4 = data.ESC_4;
  Motor5 = data.ESC_5;
  Motor6 = data.ESC_6;
  int levelUP_sw = data.levelUP_sw;
  int levelDOWN_sw = data.levelDOWN_sw;
  int left_sw = data.left_sw;  
  int right_sw = data.right_sw;
  int lifting_sw = data.lifting_sw;
  int throwing_sw = data.throwing_sw;
  int climbing_sw = data.climbing_sw;
  int hitting_sw = data.hitting_sw;
  int STOP_sw = data.STOP_sw;
  Serial.print("Buttons:");
  Serial.print(lifting_sw);Serial.print(" ");
  Serial.print(throwing_sw);Serial.print(" ");
  Serial.print(climbing_sw);Serial.print(" ");
  Serial.print(data.STOP_sw);Serial.print(" ");
  ALL_MOTORS();                                   
/////////////////////////////////////////(If Statements: RACING)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  //DONE//
  if ((data.lifting_sw==0 && data.throwing_sw==0 && data.climbing_sw==0 && /*data.hitting_sw==0 &&*/ data.STOP_sw==0) || (data.lifting_sw==0 && data.throwing_sw==0 && data.climbing_sw==1 /*&& data.hitting_sw==0 */&& data.STOP_sw==0))
  {
    Serial.print("      RACING MODE        ");
  }
/////////////////////////////////////////(If Statements: STOP)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////       
  //DONE//
  else if (lifting_sw==0 && throwing_sw==0 && climbing_sw==0 /*&& hitting_sw==0 */&& STOP_sw==1)
  {
    Serial.print("     STOP ACTIVE       ");                       
  }   
/////////////////////////////////////////(If Statements: LIFTING)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
  //***READY FOR TESTING***//
  else if (lifting_sw==1 && throwing_sw==0 && climbing_sw==0 /*&& hitting_sw==0*/ && STOP_sw==0)
  {
    if (levelUP_sw == 1){           //DC motor Forward if UP switch is pressed
      motors.setM1Speed(400);       //full FORWARD
      if (micro_sw1 == 1){
        motors.setM1Brake(400);     //if the motor hits the micro switch (which tells us that we cant go any more) then stop the motors
      }
    }
    else if (levelDOWN_sw == 1){    //DC motor REVERSE if DOWN switch is pressed
      motors.setM1Speed(-400);      //full REVERSE
      if (micro_sw2 == 1){
        motors.setM1Brake(400);     
      }
    }      
    else{
      motors.setM1Brake(400);       //full BRAKE I think? if both buttons are pressed
    }
  }  
/////////////////////////////////////////(If Statements: THROWING)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
  //*****NEED MORE INFORMATION*****//
  else if (lifting_sw==0 && throwing_sw==1 && climbing_sw==0 /*&& hitting_sw==0*/ && STOP_sw==0)
  {       
  }
/////////////////////////////////////////(If Statements: CLIMING)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
  //DONE//
  else if (lifting_sw==0 && throwing_sw==0 && climbing_sw==1 && hitting_sw==0 && STOP_sw==0)
  {    
    Serial.print("      CLIMBING MODE        ");
  }
/////////////////////////////////////////(If Statements: HITTING)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  //*****NEED MORE INFORMATION*****//
  else if (lifting_sw==0 && throwing_sw==0 && climbing_sw==0 /*&& hitting_sw==1*/ && STOP_sw==0)
  {      
  }
/////////////////////////////////////////(END)////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////              
  Serial.print("    LEFT AXIS: ");  
  Serial.print(Motor1);
  Serial.print(" "); 
  Serial.print(Motor2);
  Serial.print(" "); 
  Serial.print(Motor3);
  Serial.print(" "); 
  
  Serial.print("    RIGHT AXIS: ");  
  Serial.print(Motor4);
  Serial.print(" "); 
  Serial.print(Motor5);
  Serial.print(" "); 
  Serial.print(Motor6);
  Serial.print(" ");

  if(now-lastRecvTime > 4000) {
    Serial.print("                 lost");
  }
  Serial.println();      
  radio.startListening(); 
}


