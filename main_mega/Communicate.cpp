#include <Arduino.h>
#include <JY901.h>
#include "Communicate.h"
#include "Tool.h"
#include "Parameter.h"

Communicate::Communicate(){}

Communicate::~Communicate(){};

void Communicate::attach_jetson_nano_Serial(
    HardwareSerial *jetson_nano_Serial,
    void (*recieve_jetson_nano)(char dir_code, int speed, int target_angle, int servo0_angle, int servo1_angle),
    void (*recieve_jetson_nano_action)(char mission_code)
    ){

    this->jetson_nano_Serial = jetson_nano_Serial;
    (*jetson_nano_Serial).begin(_MY_BAUD_);

    this->recieve_jetson_nano = recieve_jetson_nano;
    this->recieve_jetson_nano_action = recieve_jetson_nano_action;
}

void Communicate::attach_control_mega_Serail(HardwareSerial *control_mega_Serail){
    this->control_mega_Serail = control_mega_Serail;
    (*control_mega_Serail).begin(_MY_BAUD_);
}

void Communicate::attach_jy61_Serial(
    HardwareSerial *jy61_Serail, 
    void (*recieve_jy61_now_angle)(int now_angle)
    ){
    // this->jy61_Serail = jy61_Serail;

    (*jy61_Serail).begin(115200);
    JY901.attach(*jy61_Serail);

    first_angle = jy61_serial_read();

    this->recieve_jy61_now_angle = recieve_jy61_now_angle;
}

void Communicate::read_serial_buffer(){
    jetson_nano_serial_read();
    jy61_serial_read();
    delay(_READ_DELAY_);
}

void Communicate::send_jetson_nano_mission(bool mission_success){
    if (!jetson_nano_Serial){
        return;
    }
    (*jetson_nano_Serial).print(mission_success ? 'a' : 'b');
}

void Communicate::send_motor_mega(int now_angle, int target_angle, char dir_code, int speed){
    if (!control_mega_Serail){
        return;
    }
    char temp[5];
    sprintf(temp, "~%c%03d", dir_code, speed);
    (*control_mega_Serail).print(temp);

    char temp2[7];
    sprintf(temp, "[%03d%03d", now_angle, target_angle);
    (*control_mega_Serail).print(temp2);
};

void Communicate::jetson_nano_serial_read(){
    /*
    使用Serial1
    取得jetson nano傳過來的資料，資料分成兩部分，詳見編碼表
    1. <行動>
    2. [方向、速度、自走車目標角度、手臂馬達1、手臂馬達2]
    */

    // // test start
    // while((*jetson_nano_Serial).available()){
    //     Serial.write((*jetson_nano_Serial).read());
    // }
    // return;
    // // test end

    if (!jetson_nano_Serial){
        return;
    }

    if (!(*jetson_nano_Serial).available()){
        return;
    }

    char type = (*jetson_nano_Serial).read();

    while((*jetson_nano_Serial).available()){

        switch (type)
        {
            case '<':{
                delay(1);
                char action_code = (*jetson_nano_Serial).read();
                if (recieve_jetson_nano_action){
                    (*recieve_jetson_nano_action)(action_code);
                }
                break;
            }
            
            case '[':{
                delay(1);
                char dir_code = (*jetson_nano_Serial).read();
                int speed = tool.char2int(*jetson_nano_Serial);
                int target_angle = tool.char2int(*jetson_nano_Serial);
                int servo0 = tool.char2int(*jetson_nano_Serial);;
                int servo1 = tool.char2int(*jetson_nano_Serial);;

                if (recieve_jetson_nano){
                    (*recieve_jetson_nano)(dir_code, speed, target_angle, servo0, servo1);
                }
                break;
            }
        }
    }

};


int Communicate::jy61_serial_read(){
    JY901.receiveSerialData();
    int now_angle = JY901.getPitch();
    if (recieve_jy61_now_angle){
        (*recieve_jy61_now_angle)(now_angle);
    }
    return now_angle;

    // Serial.print("Angle:");
	// Serial.print(JY901.getRoll());
	// Serial.print(" ");
	// Serial.print(JY901.getPitch());
	// Serial.print(" ");
	// Serial.print(JY901.getYaw());
	// Serial.print("\n");
};

