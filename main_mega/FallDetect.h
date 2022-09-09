#ifndef _FALL_DETECT_H_
#define _FALL_DETECT_H_

#include <Arduino.h>
#include "Parameter.h"

class FallDetect{
    private:
    bool enable_active = true;

    public:
    FallDetect();
    ~FallDetect();

    void enable(bool active);  // 防落偵測是否啟動
    inline bool get_is_fall(byte pin);  // 取得此感測器是否偵測到要掉下來
    bool get_is_fall(byte *pins, int amount);  // 取得這一串感測器是否偵測到要掉下來
    inline bool get_is_arrive(byte pin);  // 偵測下方是否有東西
    inline int get_distance(byte pin);  // 取得這個sensor的距離


};


#endif