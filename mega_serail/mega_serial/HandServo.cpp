
#include <Arduino.h>
#include <Servo.h>
#include "HandServo.h"
#include "Parameter.h"

HandServo::HandServo(){};
HandServo::~HandServo(){};

void HandServo::attach(const int servo_pin[2], const byte pump_pin){
    servos[0].attach(servo_pin[0]);
    servos[1].attach(servo_pin[1]);

    this->pump_pin = pump_pin;
    pinMode(pump_pin, OUTPUT);
    digitalWrite(pump_pin, 1);
}

void HandServo::do_it(int angle_a, int angle_b, bool pump_active){
    move_hand(angle_a, angle_b);
    active_pump(pump_active);
}

void HandServo::do_it(int *angles, bool pump_active){
    move_hand(angles);
    active_pump(pump_active);
}

inline void HandServo::move_hand(int angle_a, int angle_b){
    servos[0].write(angle_a);
    servos[1].write(angle_b);
    
}

inline void HandServo::move_hand(int *angles){
    move_hand(angles[0], angles[1]);
}

inline void HandServo::active_pump(bool active){
    digitalWrite(pump_pin, !active);
    pump_active = active;
    
    delay(_PUMP_DELAY_);
};

inline bool HandServo::get_pump_active(){
    return pump_active;
};
