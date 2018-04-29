#include "Otto.h"
#include "SoftSerialCommand.h"

SoftwareSerial BT(BT_Tx,BT_Rx); // Set Up Bluetooth connection on SoftwareSerial (Tx is D7, Rx is D6 in Arduino Side)  
// creates a "virtual" serial port/UART
// connect BT module TX to D6
// connect BT module RX to D7
// connect BT Vcc to 5V, GND to GND

SoftSerialCommand SCmd(BT); //The SerialCommand object

Otto Ottobot;  //my name is Ottobot! Hello World!

//-- Movement parameters
int T=1000;              //Initial duration of movement -> show the speed of movement
int moveId=0;            //type of movement
int moveSize=30;         //Size of movement (amplitude of movement)

void setup()  
{
  // set digital pin to control as an output
  //pinMode(13, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  Serial.begin(9600); //init for Serial interface for Debug data in PC 

  Ottobot.init(HIP_L, HIP_R, FOOT_L, FOOT_R, true, PIN_NoiseSensor, PIN_Buzzer,PIN_Trigger, PIN_Echo);  
    
  randomSeed(analogRead(A6));
  //Setup callbacks for SerialCommand commands 
  SCmd.addCommand("T", recieveBuzzer);    //  sendAck & sendFinalAck
  SCmd.addCommand("M", receiveMovement);  //  sendAck & sendFinalAck
  SCmd.addCommand("H", receiveGesture);   //  sendAck & sendFinalAck
  SCmd.addCommand("K", receiveSing);      //  sendAck & sendFinalAck
  SCmd.addDefaultHandler(receiveStop);
}

void loop() 
{
  if (BT.available() > 0) {
    SCmd.readSerial();

    //If Ottobot is moving yet
    if (Ottobot.getRestState()==false) {
      move(moveId);
    }
  }
}

//-- Function to receive buzzer commands
void recieveBuzzer(){
    //sendAck & stop if necessary
    sendAck();
    Ottobot.home(); 

    bool error = false; 
    int frec;
    int duration; 
    char *arg; 
    
    arg = SCmd.next(); 
    if (arg != NULL) { frec=atoi(arg); }    // Converts a char string to an integer   
    else {error=true;}
    
    arg = SCmd.next(); 
    if (arg != NULL) { duration=atoi(arg); } // Converts a char string to an integer  
    else {error=true;}

    if(error==true){
      delay(2000);
    }else{ 
      Ottobot._tone(frec, duration, 1);   
    }

    sendFinalAck();
}

//-- Function to receive movement commands
void receiveMovement(){

    sendAck();
    Serial.print("Move Command: ");
    if (Ottobot.getRestState()==true){
        Ottobot.setRestState(false);
    }

    //Definition of Movement Bluetooth commands
    //M  MoveID  T   MoveSize  
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) {moveId=atoi(arg); Serial.println(moveId); Serial.print(" ");}
    else{
      Ottobot.putMouth(xMouth);
      delay(2000);
      Ottobot.clearMouth();
      moveId=0; //stop
    }
    
    arg = SCmd.next(); 
    if (arg != NULL) {T=atoi(arg); Serial.println(T); Serial.print(" ");}
    else{
      T=1000;
    }

    arg = SCmd.next(); 
    if (arg != NULL) {moveSize=atoi(arg); Serial.println(moveSize); Serial.print(" ");}
    else{
      moveSize =30;
    }
}


//-- Function to execute the right movement according the movement command received.
void move(int moveId){

  bool manualMode = false;

  switch (moveId) {
    case 0:
      Ottobot.home();
      break;
    case 1: //M 1 1000 
      Ottobot.walk(1,T,1);
      break;
    case 2: //M 2 1000 
      Ottobot.walk(1,T,-1);
      break;
    case 3: //M 3 1000 
      Ottobot.turn(1,T,1);
      break;
    case 4: //M 4 1000 
      Ottobot.turn(1,T,-1);
      break;
    case 5: //M 5 1000 30 
      Ottobot.updown(1,T,30);
      break;
    case 6: //M 6 1000 30
    moveSize =30;
      Ottobot.moonwalker(1,T,moveSize,1);
      break;
    case 7: //M 7 1000 30
      moveSize =30;
      Ottobot.moonwalker(1,T,moveSize,-1);
      break;
    case 8: //M 8 1000 30
    moveSize =30;
      Ottobot.swing(1,T,moveSize);
      break;
    case 9: //M 9 1000 30 
    moveSize =30;
      Ottobot.crusaito(1,T,moveSize,1);
      break;
    case 10: //M 10 1000 30
      moveSize =30; 
      Ottobot.crusaito(1,T,moveSize,-1);
      break;
    case 11: //M 11 1000 
      Ottobot.jump(1,T);
      break;
    case 12: //M 12 1000 30
    moveSize =30;
      Ottobot.flapping(1,T,moveSize,1);
      break;
    case 13: //M 13 1000 30
    moveSize =30;
      Ottobot.flapping(1,T,moveSize,-1);
      break;
    case 14: //M 14 1000 20
    moveSize = 20;
      Ottobot.tiptoeSwing(1,T,moveSize);
      break;
    case 15: //M 15 500 
      Ottobot.bend(1,T,1);
      break;
    case 16: //M 16 500 
      Ottobot.bend(1,T,-1);
      break;
    case 17: //M 17 500 
      Ottobot.shakeLeg(1,T,1);
      break;
    case 18: //M 18 500 
      Ottobot.shakeLeg(1,T,-1);
      break;
    case 19: //M 19 500 20
    moveSize =30;
      Ottobot.jitter(1,T,moveSize);
      break;
    case 20: //M 20 500 15
    moveSize =30;
      Ottobot.ascendingTurn(1,T,moveSize);
      break;
    default:
        manualMode = true;
      break;
  }

  if(!manualMode){
    sendFinalAck();
  }
       
}


//-- Function to receive gesture commands
void receiveGesture(){

    //sendAck & stop if necessary
    sendAck();
    Ottobot.home(); 

    //Definition of Gesture Bluetooth commands
    //H  GestureID  
    int gesture = 0;
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) {gesture=atoi(arg);}
    else 
    {
      Ottobot.putMouth(xMouth);
      delay(2000);
      Ottobot.clearMouth();
    }

    switch (gesture) {
      case 1: //H 1 
        Ottobot.playGesture(OttoHappy);
        break;
      case 2: //H 2 
        Ottobot.playGesture(OttoSuperHappy);
        break;
      case 3: //H 3 
        Ottobot.playGesture(OttoSad);
        break;
      case 4: //H 4 
        Ottobot.playGesture(OttoSleeping);
        break;
      case 5: //H 5  
        Ottobot.playGesture(OttoFart);
        break;
      case 6: //H 6 
        Ottobot.playGesture(OttoConfused);
        break;
      case 7: //H 7 
        Ottobot.playGesture(OttoLove);
        break;
      case 8: //H 8 
        Ottobot.playGesture(OttoAngry);
        break;
      case 9: //H 9  
        Ottobot.playGesture(OttoFretful);
        break;
      case 10: //H 10
        Ottobot.playGesture(OttoMagic);
        break;  
      case 11: //H 11
        Ottobot.playGesture(OttoWave);
        break;   
      case 12: //H 12
        Ottobot.playGesture(OttoVictory);
        break; 
      case 13: //H 13
        Ottobot.playGesture(OttoFail);
        break;         
      default:
        break;
    }

    sendFinalAck();
}

//-- Function to receive sing commands
void receiveSing(){

    //sendAck & stop if necessary
    sendAck();
    Ottobot.home(); 

    //Definition of Sing Bluetooth commands
    //K  SingID    
    int sing = 0;
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) {sing=atoi(arg);}
    else 
    {
      delay(2000);

    }

    switch (sing) {
      case 1: //K 1 
        Ottobot.sing(S_connection);
        break;
      case 2: //K 2 
        Ottobot.sing(S_disconnection);
        break;
      case 3: //K 3 
        Ottobot.sing(S_surprise);
        break;
      case 4: //K 4 
        Ottobot.sing(S_OhOoh);
        break;
      case 5: //K 5  
        Ottobot.sing(S_OhOoh2);
        break;
      case 6: //K 6 
        Ottobot.sing(S_cuddly);
        break;
      case 7: //K 7 
        Ottobot.sing(S_sleeping);
        break;
      case 8: //K 8 
        Ottobot.sing(S_happy);
        break;
      case 9: //K 9  
        Ottobot.sing(S_superHappy);
        break;
      case 10: //K 10
        Ottobot.sing(S_happy_short);
        break;  
      case 11: //K 11
        Ottobot.sing(S_sad);
        break;   
      case 12: //K 12
        Ottobot.sing(S_confused);
        break; 
      case 13: //K 13
        Ottobot.sing(S_fart1);
        break;
      case 14: //K 14
        Ottobot.sing(S_fart2);
        break;
      case 15: //K 15
        Ottobot.sing(S_fart3);
        break;    
      case 16: //K 16
        Ottobot.sing(S_mode1);
        break; 
      case 17: //K 17
        Ottobot.sing(S_mode2);
        break; 
      case 18: //K 18
        Ottobot.sing(S_mode3);
        break;   
      case 19: //K 19
        Ottobot.sing(S_buttonPushed);
        break;                      
      default:
        break;
    }

    sendFinalAck();
}

//-- Function to receive Stop command.
void receiveStop(){

    sendAck();
    Ottobot.home();
    sendFinalAck();

}

//-- Function to send Ack comand (A)
void sendAck(){

  delay(30);

  BT.print(F("&&"));
  BT.print(F("A"));
  BT.println(F("%%"));
  BT.flush();
}

//-- Function to send final Ack comand (F)
void sendFinalAck(){

  delay(30);

  BT.print(F("&&"));
  BT.print(F("F"));
  BT.println(F("%%"));
  BT.flush();
}

