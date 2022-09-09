#include <Arduino.h>
#include "FallDetect.h"
#include "Parameter.h"

FallDetect::FallDetect(){}
FallDetect::~FallDetect(){}

void FallDetect::enable(bool active){
    /**
     * 是否啟動防落偵測
     * 
     */
    enable_active = active;
}

inline bool FallDetect::get_is_fall(byte pin){
    if (!enable_active) {
        return false;
    }

    return get_distance(pin) > _FALL_SENSOR_DETECT_FALL_DISTANCE_;
}

bool FallDetect::get_is_fall(byte *pins, int amount){
    if (!enable_active) {
        return false;
    }

    for (int i=0;i<amount;++i){
        if (get_is_fall(pins[i])){
            return true;
        }
    }
    return false;
}

inline int FallDetect::get_distance(byte pin){
    if (!enable_active) {
        return 0;
    }

    return 13 * pow((analogRead(pin) * 0.0048828125), -1);
}

inline bool FallDetect::get_is_arrive(byte pin){
    if (!enable_active) {
        return true;
    }

    return get_distance(pin) < _FALL_SENSOR_DETECT_ARRIVE_DISTANCE_;
}