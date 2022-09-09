#include <Arduino.h>
#include "Tool.h"
#include "Communicate.h"
#include "HandServo.h"
#include "pin.h"
#include "FallDetect.h"

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
HandServo hand_servo;
FallPin fallpin;
FallDetect falldetct;

Servo storage_hand_servo;

// 監聽函式
void recieve_jetson_nano(char dir_code, int speed, int target_angle, int servo0_angle, int servo1_angle);
void recieve_jetson_nano_action(char mission_code);  // 接收到jetson nano任務資料時會執行的
void recieve_jy61_now_angle(int now_angle_);  // jy61 接收到角度時會執行的函式

void storage_out(bool let_out);  // 排料，let_out = true：排
void mission_e();  // 第一關定位到起點方框前
void mission_f();  // 第一關定位到起點方框後


void setup(){

    // Serial.begin(115200);
    // commu.attach_jetson_nano_Serial(&Serial1, &recieve_jetson_nano, &recieve_jetson_nano_action);
    // commu.attach_control_mega_Serail(&Serial2);
    // commu.attach_jy61_Serial(&Serial3, &recieve_jy61_now_angle);

    hand_servo.attach(hand_servos_pin, pump_pin);
    storage_hand_servo.attach(storage_hand_pin);

    falldetct.enable(true);

}

void loop(){

    // commu.read_serial_buffer();
    int a = analogRead(A0);
    int b = analogRead(A1);

    a = map(a, 0, 1024, 0, 255);
    b = map(b, 0, 1024, 0, 255);

    hand_servo.move_hand(a, b);


}

void recieve_jetson_nano(char dir_code, int speed, int target_angle, int servo0_angle, int servo1_angle){
    commu.send_motor_mega(now_angle, target_angle, dir_code, speed);

    int servo_angle[2] = {servo0_angle, servo1_angle};
    hand_servo.do_it(servo_angle, pump_active);
}

void recieve_jetson_nano_action(char mission_code){
    /**
     * 接收任務指令，做完後須回傳做得如何
     * commu.send_jetson_nano_mission(bool mission_success)
     */
    switch (mission_code)
    {
        case 'a':{
            // 吸盤吸
            hand_servo.active_pump(true);
            break;
        }
        
        case 'b':{
            // 吸盤放
            hand_servo.active_pump(false);

            break;
        }
        
        case 'c':{
            // 2F倒料 - on
            storage_out(true);
            break;
        }
        
        case 'd':{
            // 2F倒料 - off
            storage_out(false);
            break;
        }
        
        case 'e':{
            // 第一關定位到起點方框前
            
            mission_e();
            

            break;
        }
        
        case 'f':{
            // 第一關定位到起點方框後

            break;
        }
        
        case 'g':{
            // 第一關從起點方框 "前" 到終點方框，倒料後，正面面向終點方框

            break;
        }
        
        case 'h':{
            // 第一關從起點方框 "後" 到終點方框，倒料後，正面面向終點方框

            break;
        }
        
        case 'i':{
            // 第一關重新定位到終點方框

            break;
        }

        case 'j':{
            // 從第一關出發到第二關，並自動通過第二三關

            break;
        }
        
        case 'k':{
            // 從第二關重新開始

            break;
        }
        
        case 'l':{
            // 從第三關重新開始

            break;
        }
        
        
        default:
            break;
    }


    commu.send_jetson_nano_mission(true);

}

void recieve_jy61_now_angle(int now_angle_){
    now_angle = now_angle_;
}

void storage_out(bool let_out){
    // 排料
    storage_hand_servo.write(let_out ? 45 : 135);
}

void mission_e(){
    /**
     * @brief 
     * 第一關定位到起點方框前
     */

    // 快速左前
    unsigned long now_t = millis();
    while(now_t - millis() > 1000){
        commu.read_serial_buffer();
        commu.send_motor_mega(now_angle, target_angle, 'q', speed_range[2]);
    }
    
    // 慢速左前，直到到邊緣
    while(!falldetct.get_is_fall(fallpin.lmd)){
        commu.read_serial_buffer();
        commu.send_motor_mega(now_angle, target_angle, 'q', speed_range[0]);
    }

    // 中速前，直到到邊緣
    while(!falldetct.get_is_arrive(fallpin.mfd)){
        commu.read_serial_buffer();
        if (falldetct.get_is_fall(fallpin.lmd)){
            commu.send_motor_mega(now_angle, target_angle, 'e', speed_range[1]);
        }else{
            commu.send_motor_mega(now_angle, target_angle, 'w', speed_range[1]);
        }
    }

    // 慢速左，直到到邊緣
    while(!falldetct.get_is_fall(fallpin.lmd)){
        commu.read_serial_buffer();
        commu.send_motor_mega(now_angle, target_angle, 'a', speed_range[0]);
    }

    // 中速右
    now_t = millis();
    while(now_t - millis() > 1000){
        commu.read_serial_buffer();
        commu.send_motor_mega(now_angle, target_angle, 'd', speed_range[1]);
    }

    // 向後
    now_t = millis();
    while(now_t - millis() > 1000){
        commu.read_serial_buffer();
        commu.send_motor_mega(now_angle, target_angle, 's', speed_range[1]);
    }


    // finish
    
}