#include <stdint.h>

int dart_count;

boolean Trigger_state;
boolean Rev_state;
boolean dart_state;
boolean first_scan;

typedef enum _three_state{
    kTop,
    kBottom,
    kMiddle
}three_state_t;

typedef enum _status_t{
    kSuccess,
    kFail
}status_t;



class Momentary {    
    public:
    uint_fast8_t pin; // Pin number    
    uint_fast8_t state; // Internal state of the switch
    /// Construct a new momentary switch
    Momentary(uint_fast8_t _pin){
        this->pin = _pin;
        pinMode(pin, INPUT);
        state = digitalRead(pin);
    }
    
    /// Update the pin, return the state
    void Update(){
        state = digitalRead(pin);
    }
};

class ThreeWay {   
    uint_fast32_t update_ms = 65;
    uint_fast32_t prev_ms;
    
    public:
    uint_fast8_t pin1;
    uint_fast8_t pin2;    
    /// internal states
    three_state_t state; 
    ThreeWay(uint_fast8_t _pin1, uint_fast8_t _pin2){
        this->pin1 = _pin1;
        this->pin2 = _pin2;
        pinMode(pin1, INPUT_PULLUP);
        pinMode(pin2, INPUT_PULLUP);
        StateChk();
    }
    
    void Update(){
        uint_fast32_t curr_ms = millis();
        if ((curr_ms - prev_ms) >= update_ms){
            StateChk();
            prev_ms = curr_ms;
        }
    }
    
    private:
    void StateChk(){
        if (digitalRead(pin1) == LOW){
            state = kTop;
        } else if(digitalRead(pin2) == LOW){
            state = kBottom;
        } else {
            state = kMiddle;
        }
    }
};

class Motor {
    uint_fast32_t update_ms = 65;
    uint_fast32_t prev_ms;
    
    public:
    uint_fast8_t pin;
    uint_fast8_t speed;
    
    Motor(uint_fast8_t _pin){
        this->pin = pin;
        pinMode(pin, OUTPUT);
    }
    
    void Update(uint_fast8_t speed){
        uint_fast32_t curr_ms = millis();
        if ((curr_ms - prev_ms) >= update_ms){
            analogWrite(pin, speed);
            prev_ms = curr_ms;
        }
    }   
};


void full_auto(Momentary Trigger, Motor Feedbelt, uint_fast8_t speed) {
  Feedbelt.Update(speed);
  while (Trigger.state == 1){
      Trigger.Update();
  }
  Feedbelt.Update(0);
}


status_t fire(Momentary Dart, Momentary Trigger, Motor Feedbelt, uint_fast8_t num, uint_fast8_t speed){
    ++num;
    Feedbelt.Update(speed);
    while(--num){
        while(Dart.state == 1 || Trigger.state == 1){
            Dart.Update();
            Trigger.Update();
        }
        if (Trigger.state == 0) return kFail;
        while(Dart.state == 0 || Trigger.state == 1){
            Dart.Update();
            Trigger.Update();
        }
        if (Trigger.state == 0) return kFail;
    }
    Feedbelt.Update(0);
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

Momentary Trigger(7);
Momentary Rev(8);
Momentary Clip(4);
Momentary Door(3);
Momentary Dart(9);
ThreeWay Selector(10,11);
Motor Flywheel(6);
Motor Feedbelt(5);



void setup() 
{

//  pinMode(led, OUTPUT);
//  Serial.begin(9600);
}

void loop() {
  Trigger.Update();
  Rev.Update();
  Selector.Update();

   if (Rev.state == 1) {
    Flywheel.Update(255);
  }
    else {
    Flywheel.Update(0);
  }
    
  if (Trigger.state == 1 && Rev.state == 1 ){
    if (Selector.state == kTop){
      fire(Dart, Trigger, Feedbelt, 1, 255);
    }
    else if (Selector.state == kBottom){
      fire(Dart, Trigger, Feedbelt, 1, 255);
    }
    else {
      full_auto(Trigger, Feedbelt, 255);
    }
  }
}
