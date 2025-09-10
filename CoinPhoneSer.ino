#include  <Arduino.h>

// variable use to measuer the intervals inbetween impulses
int  i=0;
// Number of impulses detected
int impulsCount=0;
// Sum of all the  coins inseted
float total_amount=0;
int Maxtime=30;
int CurTime = 0;
unsigned long Timer = 0; 
unsigned long BuzzTimer = 0;
unsigned long SecTimer = 0;
bool CountTime = false;
bool WaitPhone = false;
bool SecT = false;

const uint8_t RelayPin = 4;
const uint8_t Relay2Pin = 5;
const uint8_t CoinPin = 0;//D2 pin interrupt
const uint8_t PhoneFBPin = 6;
const uint8_t BuzzPin = 7;

bool OpPhoneState = false;
bool OpReqSate = false;
bool ringing = false;
bool Buzz = false;
bool call = false;


void setupCoin() {  
  delay(2000);
  pinMode(PhoneFBPin, INPUT_PULLUP);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, LOW); 
  pinMode(Relay2Pin, OUTPUT);
  digitalWrite(Relay2Pin, LOW); 
  pinMode(BuzzPin, OUTPUT);
  digitalWrite(BuzzPin, LOW); 

  delay(2000);
   // Interrupt connected  to PIN D2 executing IncomingImpuls function when signal goes from HIGH to LOW

  attachInterrupt(0,incomingImpuls, FALLING);

}

void incomingImpuls()
{
  impulsCount=impulsCount+1;
  i=0;
}

void loopCoin() {

  bool PhoneState = digitalRead(PhoneFBPin);
/*  Serial.println("PHONE:");
  Serial.println(PhoneState);*/
  
  while (Serial.available() > 0) {
    String teststr = Serial.readString();  //read until timeout
    teststr.trim();                        // remove any \r \n whitespace at the end of the String
    if (teststr == "$A$") {  //Opposite ask for a call
      OpReqSate = true;
    }
    if (teststr == "$O$") {  //Opposite end call
      OpReqSate = false;
    }
    if (teststr == "$P$") {  //Opposite ok for call
      OpPhoneState = true;
    }
    if (teststr == "$R$") {  //Opposite ko for call
      OpPhoneState = false;
    }
  }


//I ask

  if (!WaitPhone && !CountTime){
    i=i+1;
/*    Serial.print("i=");
    Serial.print(i);
    Serial.print(" Impulses:");
    Serial.print(impulsCount);
    Serial.print(" Total:");
    Serial.println(total_amount);*/
    if(impulsCount>1){
      impulsCount=0;
    }
    if (i>=3  && impulsCount==1){
      WaitPhone = true;
      impulsCount=0;

      Serial.println("$A$");
      
      digitalWrite(RelayPin,HIGH);      //relay on
      delay(300);
      //Serial.println("$A$");
      SecTimer = millis();
      SecT = true;
    }
  }
  
  if(WaitPhone){
//    Serial.print("wait for phone");

/*    if(SecT && !OpPhoneState &&((millis()-SecTimer)>10000)){
      Serial.println("$A$");
      
      digitalWrite(RelayPin,HIGH);      //relay on
      delay(300);
      SecTimer = millis();
      //Serial.println("$A$");
    }*/
    
    if(!PhoneState && OpPhoneState){
      CountTime = true;
      WaitPhone = false;
      Timer = millis();  
      SecT = false;    
      call=true;
    }
  }
  
  if(CountTime){
      SecT = false;
//    Serial.print("Timer=");
//    Serial.println(CurTime);
    if((millis()-Timer)>1000){
      Timer = millis();
      CurTime++;
    }
    if(call && (!OpPhoneState || PhoneState)){
      call = false;
      CurTime = 40;
    }
    if(CurTime>=Maxtime){
      call = false;
      digitalWrite(RelayPin,LOW);      //relay off
      CurTime = 0;
      CountTime = false;
    }
  }
if(call && PhoneState){
  call=false;
  Serial.println("$O$");
}

//Other ask

  if(OpReqSate){
    SecT = false;
//    Serial.println("LED ON");
    digitalWrite(Relay2Pin,HIGH);
    digitalWrite(RelayPin,LOW);
    WaitPhone = false;
    
    ringing = true;
  }
  if(OpReqSate && !PhoneState){
//    Serial.println("LED OFF");
    digitalWrite(Relay2Pin,LOW);
    ringing = false;
    OpReqSate = false;
    digitalWrite(BuzzPin,false);
    Serial.println("$P$");
    delay(300);
    call = true;
    //Serial.println("$P$");
  }

  if(ringing){
    if((millis()-BuzzTimer)>1000){
      BuzzTimer = millis();
      Buzz = !Buzz;
      digitalWrite(BuzzPin,Buzz);
    }
  }
  
}
