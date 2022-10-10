#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <Arduino.h>

class MotorControl
{
private:
    void speed_adjust(char dir, int target_speed, int w, int result_speeds[]);
    byte *all_pwm_pin;
    byte *all_dir_pin;
    bool is_debug = false;

public:
    MotorControl(byte *all_pwm_pin, byte *all_dir_pin);
    ~MotorControl();
    void start_debug_with_Serial0();

    void move(char dir, int speed);
    void move(char dir, int speed, int angle_offset);  // angle_offset: 越大則表示車子越往順時針偏移，越負則逆時針，會依此值回正
    void stop_little_time();
    void stop();
    
    void one_wheel_move(int which_wheel, int speed);
};



#endif