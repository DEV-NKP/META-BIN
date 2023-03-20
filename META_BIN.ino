#include <Servo.h>
#include "SD.h"
#define SD_ChipSelectPin 53
#include "TMRpcm.h"
#include "SPI.h"
#include<AFMotor.h>
#include <SoftwareSerial.h>                        
SoftwareSerial esp8266(A8,A9); //Rx,Tx                  
#define serialCommunicationSpeed 9600               
#define DEBUG true    
                             
int count=0;

Servo servo;
TMRpcm tmrpcm;

#define LEFT A0
#define echopin A1 // echo pin
#define trigpin A2 // Trigger pin
#define RIGHT A3

AF_DCMotor Motor1(1,MOTOR12_1KHZ);
AF_DCMotor Motor2(2,MOTOR12_1KHZ);
AF_DCMotor Motor3(3,MOTOR34_1KHZ);
AF_DCMotor Motor4(4,MOTOR34_1KHZ);

Servo myservo;
 
int pos =0;
long time;

int checkstatus=0;

int const trigPinBin = 23;    //6
int const echoPinBin = 22;    //5
int const forceOpenBin = 26;  //switch bin

int const trigPinTrash = 24;  //1
int const echoPinTrash = 25;  //2

int const alertLight_Bin = 27;  //13
int const idleLight_Bin = 2;    //2
int const trashFullLight_Bin = 31;    //31

int const alertLight01_Trash = 28;  //40
int const alertLight02_Trash = 29;  //41
int const alertLight03_Trash = 30;  //42

int const decorLED = 41;    
int const forwardLED = 34; 
int const backwardLED =35; 
int const leftLED = 36; 
int const rightLED = 37; 
int const ldrLED = 39;    


int LDR=A7;
int ldr_reading=0;




int binStatus = LOW;
int trashStatus = HIGH;
int carStatus = LOW;
int forceStatusBin = LOW;

int delay_timer(int milliseconds) {
  int count = 0;
  while (1) {
    if (TCNT0 >= 16)  // Checking if 1 millisecond has passed
    {
      TCNT0 = 0;
      count++;
      if (count == milliseconds)  //checking if required milliseconds delay has passed
      {
        count = 0;
        break;  // exits the loop
      }
    }
  }
  return 0;
}

void setup() {

  //TIMER 0 SETUP START
  TCCR0A = 0b00000000;
  TCCR0B = 0b00000101;
  TCNT0 = 0;
  //TIMER 0 SETUP END


  //BIN and TRASH PIN SETUP START
  pinMode(trigPinBin, OUTPUT);
  pinMode(trigPinTrash, OUTPUT);
  pinMode(forceOpenBin, INPUT);
  pinMode(alertLight_Bin, OUTPUT);
  pinMode(trashFullLight_Bin, OUTPUT);
  pinMode(alertLight01_Trash, OUTPUT);
  pinMode(alertLight02_Trash, OUTPUT);
  pinMode(alertLight03_Trash, OUTPUT);


  pinMode(rightLED, OUTPUT);
    pinMode(leftLED, OUTPUT);
      pinMode(backwardLED, OUTPUT);
        pinMode(forwardLED, OUTPUT);
          pinMode(decorLED, OUTPUT);
pinMode(ldrLED, OUTPUT);
  pinMode(echoPinBin, INPUT);
  pinMode(echoPinTrash, INPUT);
  servo.attach(44);  //44
  //BIN and TRASH PIN SETUP END

/*
  //SOUND SETUP START
  pinMode(SD_ChipSelectPin, OUTPUT);
  tmrpcm.speakerPin = 46;  //5,6,11 or 46 on Mega //9,10 for timer2 //selected pin is 5 (timer3)
  */
/*
timer 0 ----- pin 4
timer 1 ----- pin 11, 12, 13
timer 2 ----- pin 9, 10
timer 3 ----- pin 2, 3, 5
timer 4 ----- pin 6, 7, 8
timer 5 ----- pin 44, 45, 46
*/


  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }

  tmrpcm.setVolume(5);
/*
  tmrpcm.play("fifa.wav");
delay_timer(15000);
tmrpcm.disable();*/
  //SOUND SETUP END


  //CAR setup START
  
Serial.begin(9600);
myservo.attach(10);

for(pos = 90; pos <= 180; pos += 1){
myservo.write(pos);
delay_timer(15);
}
 
for(pos = 180; pos >= 0; pos-= 1) {
myservo.write(pos);
delay_timer(15);
}

for(pos = 0; pos<=90; pos += 1) {
myservo.write(pos);
delay_timer(15);
}

pinMode(RIGHT, INPUT);
pinMode(LEFT, INPUT);

pinMode(trigpin, OUTPUT);
pinMode(echopin, INPUT);

 //Serial.begin(serialCommunicationSpeed);           
 // esp8266.begin(serialCommunicationSpeed);     
 // InitWifiModule(); 
  
}





void loop() {
decoration();
wifi();
  
if(carStatus==LOW)
{
  checkForceBin();
  trash();  //responsible for detect how much trash it have
  bin();    //responsible for all of the BIN works
 // test_sound();
}

if(binStatus==LOW)
{
  car();
}
delay(5);
}

void decoration()
{
  digitalWrite(decorLED,HIGH);
ldr_reading=analogRead(LDR);
if(ldr_reading<800)
{
digitalWrite(ldrLED,HIGH);
}
 else{
digitalWrite(ldrLED,LOW);   
 } 
}

void wifi()
{
  count++;
  if(esp8266.available())                                           
 {    
    if(esp8266.find("+IPD,"))
    {
     delay(1000);
 
     int connectionId = esp8266.read()-48;                                                
     
     String web= "<h1></h1><h1 style='margin-left:700px;margin-top:60px; margin-right:500px; color:red;'> Meta Bin</h1>";
     String web1="<div style='margin-left:500px;margin-top:20px; margin-right:500px;border: 3px solid red; padding-left:80px;padding-right:80px;padding-top:5px;padding-bottom:5px;color:green;'><h3 style='padding-left:30px;'>|&nbsp;&nbsp;Wellcome to Meta Bin program&nbsp;&nbsp; |<h3><h3> Manage your trash bin by determining level: </h3></div> ";
     String web2="<h1 style='margin-left:500px;color:green'>-------------------------------------------------</h1>";
     String con="";
     if(checkstatus==0)
     {String con="<h3 style='border:2px solid red; margin-left:500px;margin-right:500px; padding:60px;color:green;'>There is no trash in the bin.</h3>";}
     else if(checkstatus==1)
     {String con="<h3 style='border:2px solid red; margin-left:500px;margin-right:500px; padding:60px;color:green;'>Your bin level status is LOW</h3>";}
     else if(checkstatus==2)
     {String con="<h3 style='border:2px solid red; margin-left:500px;margin-right:500px; padding:60px;color:green;'>Your bin level status is MEDIUM</h3>";}
     else if(checkstatus==3)
     {String con="<h3 style='border:2px solid red; margin-left:500px;margin-right:500px; padding:60px;color:green;'>Your bin level status is HIGH</h3>";}
     else
     {String con="<h3 style='border:2px solid red; margin-left:500px;margin-right:500px; padding:60px;color:green;'>Your bin level status is EXTREME.<br>The bin is now locked</h3>";}
     
     String webpage = web+web1+web2+con;
     
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
 
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";    
     sendData(closeCommand,3000,DEBUG);
    }
  }
 }

 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";                                             
    esp8266.print(command);                                          
    long int time = millis();                                      
    while( (time+timeout) > millis())                                 
    {      
      while(esp8266.available())                                      
      {
        char c = esp8266.read();                                     
        response+=c;                                                  
      }  
    }    
    if(debug)                                                        
    {
      Serial.print(response);
    }    
    return response;                                                  
}

void InitWifiModule()
{
  sendData("AT+RST\r\n", 2000, DEBUG);                                                  
  sendData("AT+CWJAP=\"Depal\",\"ankitasaha1975\"\r\n", 2000, DEBUG);        
  delay (3000);
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG);                                             
  delay (1500);
  sendData("AT+CIFSR\r\n", 1500, DEBUG);                                             
  delay (1500);
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG);                                             
  delay (1500);
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);                                     

}

void car()
{
unsigned int distance = read_cm();

int Right_Value = digitalRead(RIGHT);
int Left_Value  = digitalRead(LEFT);

Serial.print("R= ");
Serial.print(Right_Value);
Serial.print(" L= ");
Serial.print(Left_Value);
Serial.print(" D= ");
Serial.println(distance);

if((Right_Value==1) && (distance>=10 && distance<=30)&&(Left_Value==1)){
   Serial.println("Forward");
   carStatus=HIGH;
   forword();
  }
else if((Right_Value==0) && (distance>=10 && distance<=30)&&(Left_Value==0)){
   Serial.println("Forward");
   carStatus=HIGH;
   forword();
  }

else if((Right_Value==0) && (Left_Value==1)){
   Serial.println("turnRight");
   carStatus=HIGH;
  turnRight();
  }
else if((Right_Value==1) && (Left_Value==0)){
   Serial.println("turnLeft");
   carStatus=HIGH;
  turnLeft();
  }

else if(distance > 5 && distance < 10){
   Serial.println("stop");
   carStatus=LOW;
  stop();
  }


else if(distance < 5){
   Serial.println("backword");
 carStatus=HIGH;
  backword();
  }

  else{
    carStatus=LOW;
      Serial.println("stop");
  stop();

  }

}


/*
void test_sound() {
  if (tmrpcm.isPlaying()) {
    Serial.println("playing audio");
  } else {
    Serial.println("audio error");
  }
}
*/

void trash() {
  int duration, distance;
  digitalWrite(trigPinTrash, HIGH);
  delay_timer(1);
  digitalWrite(trigPinTrash, LOW);         // Measure the pulse input in echo pin
  duration = pulseIn(echoPinTrash, HIGH);  // Distance is half the duration devided by 29.1 (from datasheet)
  distance = (duration / 2) / 29.1;        // if distance less than 0.5 meter and more than 0 (0 or less means over range)
  if (distance < 9 && distance >= 4) {
   checkstatus=1;
    TRASH_ALERT3();
  } else if (distance < 12 && distance >= 9) {
    checkstatus=2;
    TRASH_ALERT2();
  } else if (distance < 15 && distance >= 12) {
    checkstatus=3;
    TRASH_ALERT1();
  } else if (distance >= 15) {
    checkstatus=4;
    TRASH_ALERT0();
  } else if (distance < 4 ) {
    checkstatus=0;
    TRASH_ALERT4();
  }

  else {
    //TRASH_ALERT0();
  }
}

void TRASH_ALERT0() {
  digitalWrite(alertLight01_Trash, LOW);
  digitalWrite(alertLight02_Trash, LOW);
  digitalWrite(alertLight03_Trash, LOW);
   digitalWrite(trashFullLight_Bin, LOW);
  analogWrite(idleLight_Bin, 50);
 
  trashStatus = HIGH;
}

void TRASH_ALERT1() {
  digitalWrite(alertLight01_Trash, HIGH);
  digitalWrite(alertLight02_Trash, LOW);
  digitalWrite(alertLight03_Trash, LOW);
  digitalWrite(trashFullLight_Bin, LOW);
  analogWrite(idleLight_Bin, 50);
  trashStatus = HIGH;
}

void TRASH_ALERT2() {
  digitalWrite(alertLight01_Trash, HIGH);
  digitalWrite(alertLight02_Trash, HIGH);
  digitalWrite(alertLight03_Trash, LOW);
  digitalWrite(trashFullLight_Bin, LOW);
  analogWrite(idleLight_Bin, 50);
  trashStatus = HIGH;
}

void TRASH_ALERT3() {
  digitalWrite(alertLight01_Trash, HIGH);
  digitalWrite(alertLight02_Trash, HIGH);
  digitalWrite(alertLight03_Trash, HIGH);
  digitalWrite(trashFullLight_Bin, LOW);
  analogWrite(idleLight_Bin, 50);
  trashStatus = HIGH;
}

void TRASH_ALERT4() {
  digitalWrite(alertLight01_Trash, HIGH);
  digitalWrite(alertLight02_Trash, HIGH);
  digitalWrite(alertLight03_Trash, HIGH);
  digitalWrite(trashFullLight_Bin, HIGH);
  digitalWrite(idleLight_Bin, LOW);

  trashStatus = LOW;
}


void checkForceBin()
{

if(digitalRead(forceOpenBin)==HIGH)
{
  binStatus=HIGH;
  servo.write(130);
  delay_timer(10000);
  servo.write(0);
  binStatus=LOW;
}

}

void bin() {
  int duration, distance;
  digitalWrite(trigPinBin, HIGH);
  delay_timer(1);
  digitalWrite(trigPinBin, LOW);         // Measure the pulse input in echo pin
  duration = pulseIn(echoPinBin, HIGH);  // Distance is half the duration devided by 29.1 (from datasheet)
  distance = (duration / 2) / 29.1;      // if distance less than 0.5 meter and more than 0 (0 or less means over range)
  if (distance <= 20 && distance >= 12) {
    if (trashStatus == HIGH) {
      BIN_ALERT();
    }


  } else {
    if (trashStatus == HIGH) {


      BIN_IDLE();
    }
  }
}

void BIN_IDLE() {
  binStatus=LOW;
  servo.write(0);
  analogWrite(idleLight_Bin, 50);
}
void BIN_ALERT() {

binStatus=HIGH;
  servo.write(130);
  digitalWrite(idleLight_Bin, LOW);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(1000);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(500);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(500);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(500);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(500);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(300);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(300);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(300);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(300);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(300);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(300);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(100);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(100);
  digitalWrite(alertLight_Bin, LOW);
  delay_timer(100);
  digitalWrite(alertLight_Bin, HIGH);
  delay_timer(100);
  digitalWrite(alertLight_Bin, LOW);
}


long read_cm(){
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  time = pulseIn (echopin, HIGH);
  return time / 29 / 2;
}

void forword(){// turn it on going forward
digitalWrite(forwardLED,HIGH);
digitalWrite(backwardLED,LOW);
digitalWrite(leftLED,LOW);
digitalWrite(rightLED,LOW);
Motor1.setSpeed(120);
Motor1.run(FORWARD);
Motor2.setSpeed(120);
Motor2.run(FORWARD);
Motor3.setSpeed(120);
Motor3.run(FORWARD);
Motor4.setSpeed(120);
Motor4.run(FORWARD);
}

void backword(){ // the other way
digitalWrite(forwardLED,LOW);
digitalWrite(backwardLED,HIGH);
digitalWrite(leftLED,LOW);
digitalWrite(rightLED,LOW);
Motor1.setSpeed(120);
Motor1.run(BACKWARD); 
Motor2.setSpeed(120);
Motor2.run(BACKWARD);
Motor3.setSpeed(120);
Motor3.run(BACKWARD); 
Motor4.setSpeed(120);
Motor4.run(BACKWARD); 
}

void turnRight(){ // the other right
digitalWrite(forwardLED,LOW);
digitalWrite(backwardLED,LOW);
digitalWrite(leftLED,LOW);
digitalWrite(rightLED,HIGH);
Motor1.setSpeed(200);
Motor1.run(FORWARD);
Motor2.setSpeed(200);
Motor2.run(FORWARD);
Motor3.setSpeed(100);
Motor3.run(BACKWARD);
Motor4.setSpeed(100);
Motor4.run(BACKWARD);
}

void turnLeft(){ // turn it on going left
digitalWrite(forwardLED,LOW);
digitalWrite(backwardLED,LOW);
digitalWrite(leftLED,HIGH);
digitalWrite(rightLED,LOW);
Motor1.setSpeed(100);
Motor1.run(BACKWARD);
Motor2.setSpeed(100);
Motor2.run(BACKWARD);
Motor3.setSpeed(200);
Motor3.run(FORWARD);
Motor4.setSpeed(200);
Motor4.run(FORWARD);
}

void stop(){ // stopped
digitalWrite(forwardLED,LOW);
digitalWrite(backwardLED,LOW);
digitalWrite(leftLED,LOW);
digitalWrite(rightLED,LOW);
Motor1.setSpeed(0);  
Motor1.run(RELEASE);
Motor2.setSpeed(0);
Motor2.run(RELEASE);
Motor3.setSpeed(0);
Motor3.run(RELEASE);
Motor4.setSpeed(0);
Motor4.run(RELEASE);  
}
