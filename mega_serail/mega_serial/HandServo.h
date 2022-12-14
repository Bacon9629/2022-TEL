
#ifndef _HAND_MOTOR_H_
#define _HAND_MOTOR_H_

#include <Arduino.h>
#include <Servo.h>

class HandServo
{
private:
    int last_angle[2] = {-1, -1};
    bool pump_active = false;
    Servo servos[2];
    byte pump_pin;


public:
    HandServo();
    ~HandServo();

    void attach(const int servo_pin[2], const byte pump_pin);

    void do_it(int angle_0, int angle_1, bool pump_active);
    void do_it(int *angles, bool pump_active);
    inline void move_hand(int angle_a, int angle_b);
    inline void move_hand(int *angles);
    inline void active_pump(bool active);
    inline bool get_pump_active();
};


#endif