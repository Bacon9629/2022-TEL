#include <Arduino.h>
#include "Tool.h"
#include "Communicate.h"
#include "pin.h"
#include <BluetoothSerial.h>

int speed_range[3] = {40, 120, 200};

char move_dir_code = ' ';  // 移動方向編碼、無段的那種，詳見編碼表
char action_code = ' ';  // jetson nano to mega 行動指令編碼
int move_speed = 0;
int target_angle = 0;
int hand_motor_angle[2] = {90, 90};
bool pump_active = false;
int now_angle = 0;

Tool tool;
Communicate commu;
FallPin fallpin;
BluetoothSerial SerialBT;


void setup(){

    Serial.begin(115200);
    SerialBT.begin("ESP32_BT"); //藍牙顯示名稱，可自行更改，需避免與他人重複命名

}

void loop(){

    // commu.read_serial_buffer();

    if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.println(SerialBT.readString());
  }
  delay(50);


}