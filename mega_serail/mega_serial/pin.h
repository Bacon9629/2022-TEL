#include <Arduino.h>

const int hand_servos_pin[2] = {8, 9};
const byte pump_pin = 0;

const byte storage_hand_pin = 0;

class FallPin{
    public:
    byte lff = 0;  // 左 前 向前偵測
    byte mff = 0;  // 中 前 向前偵測
    byte mfd = 0;  // 中 前 向下偵測
    byte lmf = 0;  // 左 中 向前偵測
    byte lmd = 0;  // 左 中 向下偵測
    byte rmf = 0;  // 右 中 向前偵測
    byte rmd = 0;  // 右 中 向下偵測
    byte mbd = 0;  // 中 後 向下偵測
};