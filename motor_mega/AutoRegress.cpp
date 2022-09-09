#include <Arduino.h>
#include "AutoRegress.h"
# include "Parameter.h"


AutoRegress::AutoRegress(){ AutoRegress(_PID_P_, _PID_I_); }
AutoRegress::AutoRegress(int p_adjust, int i_adjust){
    this->p_adjust = p_adjust;
    this->i_adjust = i_adjust;
    last_time = millis();
    i_error = 0;
}
AutoRegress::~AutoRegress(){}

int AutoRegress::calculate_w(int target_angle, int now_angle){

    unsigned long now_time = millis();
    float delta_time = (now_time - last_time) / 1000;
    int error = now_angle - target_angle;
    i_error += delta_time * error;

    last_time = now_time;

    return p_adjust * error + i_adjust * i_error;
}

void AutoRegress::start_loss_calculate(){
    
}

int AutoRegress::get_loss(){

}