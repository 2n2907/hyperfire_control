#include <stdint.h>

int triger = 7;
int rev = 8;
int clip = 4;
int door = 3;
int flywheel = 6;
int feedbelt = 5;
int dart =9;
int slector_up= 10;
int slector_down= 11;
int led=2;
int ledcount=0;
boolean last_scan;
int dart_count;
int feed_speed_three = 225;
int feed_speed = 125;

boolean Trigger_state;
boolean Rev_state;
boolean dart_state;
boolean first_scan;

typedef enum _three_state{
    single,
    full,
    burst
}three_state_t;

typedef enum _status_t{
    kSuccess,
    kFail
}status_t;

class Momentary {
    uint_fast8_t pin; // Pin number    
    uint_fast8_t state; // Internal state of the switch
    
    public:
    /// Construct a new momentary switch
    Momentary(uint_fast8_t pin){
        pinMode(pin, INPUT);
        state = digitalRead(pin);
    }
    
    /// Update the pin, return the state
    void Update(){
        state = digitalRead(pin);
    }
}

class ThreeWay {
    uint_fast8_t pin1;
    uint_fast8_t pin2;
    
    /// internal states
    three_state_t state;    
    uint_fast32_t update_ms = 65;
    uint_fast32_t prev_ms;
    
    public:
    ThreeWay(uint_fast8_t pin1, uint_fast8_t pin2){
        pinMode(pin1, INPUT_PULLUP);
        pinMode(pin2, INPUT_PULLUP);
        StateChk();
    }
    
    void Update(){
        uint_fast32_t curr_ms = millis();
        if ((curr_ms - prev_ms >= update_ms){
            StateChk();
            prev_ms = curr_ms;
        }
    }
    
    private:
    void StateChk(){
        if (digitalRead(pin1) == LOW){
            state = single;
        } else if(digitalRead(pin2) == LOW){
            state = burst;
        } else {
            state = full;
        }
    }
}

class Motor {
    uint_fast8_t pin;
    uint_fast8_t speed;

    uint_fast32_t update_ms = 65;
    uint_fast32_t prev_ms;
    
    public:
    Motor(uint_fast8_t pin){
        pinMode(pin, OUTPUT);
    }
    
    Update(uint_fast8_t speed){
        uint_fast32_t curr_ms = millis();
        if ((curr_ms - prev_ms >= update_ms){
            analogWrite(pin, speed);
            prev_ms = curr_ms;
        }
    }   
}


void Single()
{
  if (dart_state==LOW && last_scan == LOW && first_scan == HIGH && dart_count == 0)
  {
  analogWrite(feedbelt,feed_speed);
  last_scan=LOW;
  }
  if (dart_state==HIGH && last_scan == LOW && first_scan == HIGH)
  {
    dart_count ++;
    last_scan =HIGH;
  }
  if (dart_state==HIGH && last_scan == LOW && first_scan == LOW)
  {
    analogWrite(feedbelt,feed_speed);
    last_scan =HIGH;
  }
  if (dart_state==LOW && last_scan == HIGH && first_scan == LOW)
  {
    dart_count ++;
  }
  if (dart_state==LOW && last_scan == LOW && first_scan == LOW)
  {
    first_scan=HIGH;
  }
  
  if (dart_count >= 1)
  { 
    digitalWrite(feedbelt,LOW);
  }
  
  if (Trigger_state==LOW )
  {
    dart_count=0;
  }
}

/// @TODO Extend to 

void three()
{
  
  if (dart_state==LOW && dart_count < 2 )
  {
  analogWrite(feedbelt,feed_speed_three);
  last_scan=LOW;
  }
  if (dart_state==HIGH && last_scan == LOW && dart_count >0)
  {
    dart_count ++;
    last_scan =HIGH;
  }
  if (dart_state==HIGH && last_scan == LOW)
  {
    analogWrite(feedbelt,feed_speed_three);
    last_scan =HIGH;
    dart_count ++;
  }
    
    if (dart_count >= 2)
   { 
    digitalWrite(feedbelt,LOW);
   }
    
    if (Trigger_state==LOW )
    {
      dart_count=0;
    }
}
  
status_t full_auto(Momentary *Trigger) {
  digitalWrite(feedbelt,HIGH);
  while (Trigger::state == 1){
      Trigger::Update();
  }
}


status_t fire(Momentary *Dart, Momentary *Trigger, Motor *Feedbelt, uint_fast8_t num, uint_fast8_t speed){
    ++num;
    Feedbelt::Update(speed);
    while(--num){
        while(Dart::state == 1 || Trigger::state == 1){
            Dart::Update();
            Trigger::Update();
        }
        if (Trigger::state == 0) return kFail;
        while(Dart::state == 0 || Trigger::state == 1){
            Dart::Update();
            Trigger::Update();
        }
        if (Trigger::state == 0) return kFail;
    }
    return kSuccess;
}



void testprint()
{
   /* 
  Serial.print("Rev ");
  Serial.println(digitalRead(rev));
  Serial.print("Trigger ");
  Serial.println(digitalRead(triger));
  Serial.print("Clip ");
  Serial.println(digitalRead(clip));
  Serial.print("Door ");
  Serial.println(digitalRead(door));
  Serial.print("Dart ");
  Serial.println(digitalRead(dart));
  Serial.print("slector_up ");
  Serial.println(digitalRead(slector_up));
  Serial.print("slector_down ");
  Serial.println(digitalRead(slector_down));
  delay(1000);
  
  */
}

void setup() 
{
    Momentary Trigger(7);
    Momentary Rev(8);
    Momentary Clip(4);
    Momentary Door(3)
    Momentary Dart(9)
    ThreeWay Selector(10,11);
    Motor Flywheel(6);
    Motor Feedbelt(5);
    pinMode(led, OUTPUT);
//  Serial.begin(9600);
}

void loop() 
{
  Trigger_state = digitalRead(triger);
  Rev_state =digitalRead(rev);
  slector_up_state =digitalRead(slector_up);
  slector_down_state =digitalRead(slector_down);
  dart_state= digitalRead(dart);

   if (Rev_state==HIGH) {
    digitalWrite(flywheel,HIGH);
  }
    else {
    digitalWrite(flywheel,LOW); 
  }
    
  if (Trigger_state==HIGH && Rev_state==HIGH ){
    if (slector_up_state==LOW){
      Single();
    }
    else if (slector_down_state==LOW){
      three();
    }
    else {
      Auto();
    }
  }
  else
  {
   digitalWrite(feedbelt,LOW);
    dart_count=0;
   last_scan =LOW;
   first_scan = LOW; 
  }  
}