#include <Arduino.h>
#include "Communicate.h"
#include "MotorControl.h"

bool auto_regress_active = false;  // 移動校正系統設定，true為啟動

const int lf_pwm_pin = 9;
const int lb_pwm_pin = 6;
const int rf_pwm_pin = 5;
const int rb_pwm_pin = 3;

const int lf_dir_pin = 37;
const int lb_dir_pin = 35;
const int rf_dir_pin = 33;
const int rb_dir_pin = 31;

byte all_pwm_pin[4] = {lf_pwm_pin, lb_pwm_pin, rf_pwm_pin, rb_pwm_pin};
byte all_dir_pin[4] = {lf_dir_pin, lb_dir_pin, rf_dir_pin, rb_dir_pin};

const int speed_range[] = {80, 150, 200, 255};


int now_angle = 0;
int target_angle = 0;
int speed = 0;
char dir_code = ' '; // 可能隨著不同type編碼方式不同，此變數僅是暫存用的而已
int count = 0;

Communicate comm;
MotorControl motor(all_pwm_pin, all_dir_pin);

void recieve_angle(int now_angle);
void recieve_move(char dir_code, int speed);
void recieve_stop();

void setup() {
  // put your setup code here, to run once:
  for (int i=0;i<4;i++){
    pinMode(all_dir_pin[i], OUTPUT);
    digitalWrite(all_dir_pin[i], 1);
  }

  // Serial.begin(115200);

  comm.attach_Serial(&Serial1);
//  comm.attach_Serial(&Serial);
//  comm.attach_Serial(&Serial1);
//  comm.attach_Serial(&Serial1);
  // comm.start_debug_with_Serial0();
  // motor.start_debug_with_Serial0();


  comm.register_recieve_wheel_pwm(&recieve_wheel_pwm);
  comm.register_recieve_angle(&recieve_angle);
//  comm.register_recieve_move_code(&recieve_move);
//  comm.register_recieve_stop(&recieve_stop);

}

void loop() {
  // put your main code here, to run repeatedly:
  comm.read_code_buffer();

  // motor.one_wheel_move(2,120);

  // motor.move('w', 120, 0);
  // delay(3000);
  // motor.move('p', 120, 0);
  // delay(500);
  // motor.move('d', 120, 0);
  // delay(3000);
  // motor.move('d', 1, 0);
  // delay(500);

  // digitalWrite(lf_dir_pin, 1);
  // digitalWrite(lb_dir_pin, 0);
  // digitalWrite(rf_dir_pin, 1);
  // digitalWrite(rb_dir_pin, 1);
  // analogWrite(lf_pwm_pin, 220);
  // analogWrite(lb_pwm_pin, 220);
  // analogWrite(rf_pwm_pin, 220);
  // analogWrite(rb_pwm_pin, 220);

  
}

void recieve_wheel_pwm(int pwm0, int pwm1, int pwm2, int pwm3){
    motor.one_wheel_move(0, pwm0);
    motor.one_wheel_move(1, pwm1);
    motor.one_wheel_move(2, pwm2);
    motor.one_wheel_move(3, pwm3);
}

void recieve_angle(int now_angle_, int target_angle_){
  now_angle = now_angle_;
  target_angle = target_angle_;
  
  // char temp[30];
  // sprintf(temp, "%03d, %03d", now_angle, target_angle);
  // Serial.println(temp);
}

void recieve_move(char dir_code, int speed){
  /**
   * 接收到資料後馬上執行移動
   * 
   */

  // char temp[30];
  // sprintf(temp, "%c, %03d", dir_code, speed);
  // Serial.println(temp);

  motor.move(dir_code, speed, now_angle - target_angle);
  // motor.move(dir_code, speed, target_angle, now_angle);
}

void recieve_stop(){
  motor.stop();
}