#include <Arduino.h>
#include "Tool.h"
#include "Communicate.h"
#include "HandServo.h"
#include "pin.h"
#include "FallDetect.h"

int speed_range[3] = {40, 80, 140};

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
FallDetect falldetect(hand_servo);

Servo storage_hand_servo;

// 監聽函式
void recieve_jetson_nano(char dir_code, int speed, int target_angle, int servo0_angle, int servo1_angle);
void recieve_jetson_nano_action(char mission_code);  // 接收到jetson nano任務資料時會執行的
void recieve_jy61_now_angle(int now_angle_);  // jy61 接收到角度時會執行的函式

void storage_out(bool let_out);  // 排料，let_out = true：排
void mission_e();  // 第一關定位到起點方框前
void mission_f();  // 第一關定位到起點方框後


void setup(){

    Serial.begin(115200);

    commu.attach_jetson_nano_Serial(&Serial2, &recieve_jetson_nano, &recieve_jetson_nano_action);
    commu.attach_control_mega_Serail(&Serial1);
    commu.attach_jy61_Serial(&Serial3, &recieve_jy61_now_angle);

    hand_servo.attach(hand_servos_pin, pump_pin, relay_pin);
    hand_servo.move_hand(115, 110);
    hand_servo.active_pump(false);

    storage_hand_servo.attach(storage_hand_pin);

    delay(300);
    commu.reset_angle_offset();

    // falldetect.enable(true);

    Serial.println("gogo");

    storage_out(false);

}

void loop(){

    commu.read_serial_buffer();

}

void recieve_jetson_nano(char dir_code, int speed, int _target_angle, int servo0_angle, int servo1_angle){
    commu.send_motor_mega(now_angle, _target_angle, dir_code, speed);

    hand_servo.move_hand(servo0_angle, servo1_angle);

    move_dir_code = dir_code;
    move_speed = speed;
    target_angle = _target_angle;
    hand_motor_angle[0] = servo0_angle;
    hand_motor_angle[1] = servo1_angle;

    char temp[30];
    sprintf(temp, "%c, %03d, %03d, %03d, %03d", dir_code, speed, _target_angle, servo0_angle, servo1_angle);
    Serial.println(temp);
    Serial.print("now angle ");
    Serial.println(now_angle);
}

void recieve_jetson_nano_action(char mission_code){
    /**
     * 接收任務指令，做完後須回傳做得如何
     * commu.send_jetson_nano_mission(bool mission_success)
     */

    bool good_done = true;
    Serial.print("action code: ");
    Serial.println(mission_code);

    switch (mission_code)
    {
        case 'a':{
            // 吸盤吸
            hand_servo.active_pump(true);
            // Serial.println("A");
            break;
        }
        
        case 'b':{
            // 吸盤放
            hand_servo.active_pump(false);
            // Serial.println("B");
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
        
        case 'm':{
            // 回傳是唪落下，若要落下則回傳 失敗

            break;
        }
        
        case 'n':{
            // 更新初始角度，會依此角隊做回正
            commu.reset_angle_offset();
            break;
        }
        
        case 'o':{
            // 第一關重新定位到方框前
            first_mission_go_front_side();
            break;
        }
        
        case 'p':{
            // 第一關重新定位到方框右側

            break;
        }
        
        case 'q':{
            // 第一關重新定位到方框後側偵測

            break;
        }

        case 'r':{
            // 回傳各感測器是否偵測到
            if (falldetect.get_is_arrive(fallpin.mfd) || falldetect.get_is_fall(fallpin.lmd) || falldetect.get_is_fall(fallpin.rmd)){
                good_done = false;
            }
            break;
        }
        
        case 's':{
            // 關閉回正
            commu.set_jy61_enable(false);
            break;
        }
        
        case 't':{
            // 開啟回正
            commu.set_jy61_enable(true);
            break;
        }

        case 'z':{
            // 回正
            toward_target_angle();
            break;
        }
        
        
        default:
            break;
    }


    commu.send_jetson_nano_mission(good_done);

}

void recieve_jy61_now_angle(int now_angle_){
    now_angle = now_angle_;
}

void storage_out(bool let_out){
    // 排料
    storage_hand_servo.write(let_out ? 180 : 78);
}

void mission_e(){
    /**
     * @brief 
     * 第一關定位到起點方框前
     */

    // 快速左前
    move('q', speed_range[2], 2000);
    
    // 慢速左前，直到到邊緣
    goto_edge(fallpin.lmd, 'q', speed_range[1]);

    move('d', speed_range[1], 500);

    while(!falldetect.get_is_arrive(fallpin.mfd)){
        move('w', speed_range[1], 50);
    }


    
    move('p', speed_range[1], 300);
    move('s', speed_range[1], 300);
    move('d', speed_range[1], 2000);
    move('p', speed_range[1], 500);
    // finish
    
}

/**
 * @brief 第一關「重新」定位到方框前
 * 
 */
void first_mission_go_front_side(){
    goto_edge(fallpin.lmd, 'a', speed_range[1]);
    move('d', speed_range[1], 300);
    while (!falldetect.get_is_arrive(fallpin.mfd)){
        move('w', speed_range[0], 50);
    }
    move('s', speed_range[0], 300);
    move('d', speed_range[1], 1000);  // 定位車子到正中間的程式碼，主要調整這邊
    move('p', speed_range[1], 500);
}

/**
 * @brief 第二關程式
 * 
 */
void second_mission(){
    goto_edge(fallpin.rmd, 'd', speed_range[1]);
    move('a', speed_range[1], 500);
    goto_until_detect(fallpin.rmf, 'w',speed_range[1], 15);
    move('a', speed_range[1], 1000);
    move('w', speed_range[1], 1000);
    stay_center();
    move('w', speed_range[1], 1000);

    // 前往下一個區域
    // 往前直到真測到左方有東西，往後一點在往左走，直到沒偵測到木條後往前走
    move('p', speed_range[1], 300);
    goto_until_detect(fallpin.lff, 'w', speed_range[1], 20);
    move('s', speed_range[1], 500);
    move('p', speed_range[1], 300);
    goto_until_detect(fallpin.rmf, 'a', speed_range[1], 20);  // 平移植到映前偵測碰到東西
    move('a', speed_range[1], 200);
    goto_until_no_detect(fallpin.rmf, 'a', speed_range[1], 20);  // 平移植到映前偵測碰到東西
    move('a', speed_range[1], 1000);
    move('w', speed_range[1], 1000);
    stay_center();
    move('w', speed_range[1], 1000);

    // 前往下一個區域
    // 往前直到真測到右方有東西，往後一點在往右走，直到沒偵測到木條後往前走
    goto_until_detect(fallpin.rff, 'w', speed_range[1], 20);
    move('p', speed_range[1], 300);
    move('s', speed_range[1], 500);
    move('p', speed_range[1], 300);
    goto_until_detect(fallpin.lmf, 'd', speed_range[1], 20);  // 平移植到映前偵測碰到東西
    move('d', speed_range[1], 200);
    goto_until_no_detect(fallpin.lmf, 'd', speed_range[1], 20);  // 平移植到映前偵測碰到東西
    move('d', speed_range[1], 1000);
    move('w', speed_range[1], 1000);
    stay_center();

    // 通過第二關
    move('w', speed_range[1], 5000);



}

/**
 * @brief 第二關的，讓車子保持在兩木條的中間
 * 
 */
void stay_center(){
    int left_distance = falldetect.get_distance(fallpin.lff);
    int right_distance = falldetect.get_distance(fallpin.rff);
    int error_distance = abs(left_distance - right_distance);

    while(left_distance == 100 || right_distance == 100){


        target_angle = (right_distance - left_distance) * 2;
        move('w', speed_range[1], 50);

        // if (error_distance > 3){ // 如果兩距離感測偵測到差了 3 以上，則執行偏移，否則直走
        //     if (left_distance > right_distance){  // 要向左偏移
        //         move('q', speed_range[1], 50);
        //     }else{  // 要向右偏移
        //         move('e', speed_range[1], 50);
        //     }
        // }else{
        //     move('w', speed_range[1], 50);
        // }

        left_distance = falldetect.get_distance(fallpin.lff);
        right_distance = falldetect.get_distance(fallpin.rff);
        error_distance = abs(left_distance - right_distance);
        now_angle = commu.get_angle();

    }

    target_angle = 0;
    
}

/**
 * @brief 在main_mega程式內才能呼叫的移動函式
 * 
 * @param direction 移動指令編碼
 * @param speed 速度
 * @param delay_time 要移動多久時間
 */
void move(char direction, int speed, unsigned long delay_time){
    for (size_t i = 0; i < delay_time; i += 20){
        now_angle = commu.get_angle();
        commu.send_motor_mega(now_angle, target_angle, direction, speed);
    }
    delay(delay_time);
}

/**
 * @brief 自動走到邊邊界，輸入要用哪個sensor來當作邊界感測，感測到掉落時即到達
 * 
 * @param sensor sensor的PIN
 * @param direction 要移動的方向
 */
void goto_edge(byte sensor, char direction, int speed){
    while(!falldetect.get_is_fall(sensor)){
        move(direction, speed, 40);
        // commu.send_motor_mega(now_angle, target_angle, direction, speed);
    }
}

/**
 * @brief 自動走到某個東西前
 * 
 * @param sensor 要偵測的sensor
 * @param direction 要往哪邊走
 * @param speed 速度
 * @param distance 要偵測的距離，在小於這個距離錢都會不斷往前走
 */
void goto_until_detect(byte sensor, char direction, int speed, int distance){
    while(falldetect.get_distance(sensor) >= distance){
        move(direction, speed, 40);
        // commu.send_motor_mega(now_angle, target_angle, 'direction', speed);
    }
}


/**
 * @brief 自動走，直到離開某個東西
 * 
 * @param sensor 要偵測的sensor
 * @param direction 要往哪邊走
 * @param speed 速度
 * @param distance 要偵測的距離，在大於這個距離錢都會不斷往前走
 */
void goto_until_no_detect(byte sensor, char direction, int speed, int distance){
    while(falldetect.get_distance(sensor) <= distance){
        move(direction, speed, 40);
    }
}

void toward_target_angle(){
    char temp = Serial.read();
    Serial.println(temp);
    now_angle = commu.get_angle();
    int angle = now_angle - target_angle;
    int angle_unsigned = abs(angle);
    // Serial.println(angle);

    while (angle_unsigned > 0){
        while (angle_unsigned > 0){
            commu.send_motor_mega(
                now_angle, target_angle, 
                angle > 0 ? 'n' : 'm', 
                map(angle_unsigned, 0, 180, 20, 100)
            );
            delay(40);

            now_angle = commu.get_angle();
            angle = now_angle - target_angle;
            angle_unsigned = abs(angle);
            // Serial.println(angle);
        }
        commu.send_motor_mega(now_angle, target_angle, 'p', 0);
        delay(500);

        now_angle = commu.get_angle();
        angle = now_angle - target_angle;
        angle_unsigned = abs(angle);
        // Serial.println(angle);
    }
}