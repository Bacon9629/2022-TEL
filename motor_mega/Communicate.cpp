#include <Arduino.h>
#include "Communicate.h"
#include "Parameter.h"

Communicate::Communicate(){}
Communicate::~Communicate() {}
void Communicate::attach_Serial(HardwareSerial *s){
    _serial = s;
    _serial->begin(_MY_BAUD_);
    // _serial->print("abc");
    is_debug = false;
}

void Communicate::start_debug_with_Serial0()
{
    Serial.begin(_MY_BAUD_);
    Serial.println("=== Motor mega ***Communication***  start debug ===");
    is_debug = true;
}

void Communicate::read_code_buffer()
{
    #ifndef _MOVE_NO_STOP_ // 若沒啟動，若沒有傳送新訊息，馬達就會一直往上次傳輸的方向移動

    static int temp_count = _COMMU_DELAY_;
    if (!(_serial->available()))
    {
        if (!temp_count){
            (*recieve_stop_func)();
            return;
        }else{
            --temp_count;
            delay(1);
        }
    }else{
        temp_count = _COMMU_DELAY_;
    }

    #endif

    char code;
    int result_speed = -1;
    char result_code = -1;
    int count = 20;  // 若收到20個錯誤訊息則停止

    while (_serial->available())
    {
        // 如果Serial裡面還有東西則一直讀，讀到沒有為止，在讀的過程中若一直沒出現移動起始字元，則停止移動

        code = _serial->read();

        if (is_debug)
        {
            Serial.print("#");
            Serial.println(code);
        }

        switch (code)
        {
            case '<':{
                delay(1);

                int speed[4] = {0, 0, 0, 0};

                for(size_t i=0; i < 4; i++){

                    char dir = _serial->read();
                    if (dir == '-'){
                        speed[i] -= char2int(_serial);
                    }else{
                        speed[i] = char2int(_serial);
                    }

                }

                if (recieve_wheel_pwm)
                {
                    (*recieve_wheel_pwm)(speed[0], speed[1], speed[2], speed[3]);
                }
                break;
            }

            case '[':
            {
                delay(1);
                int data1 = char2int(_serial);
                int data2 = char2int(_serial);

                if (data1 > 180){
                    data1 -= 360;
                }else if(data1 < -180){
                    data1 += 360;
                }

                if (data2 > 180){
                    data2 -= 360;
                }else if(data2 < -180){
                    data2 += 360;
                }

                // data1 *= -1;
                // data2 *= -1;


                if (recieve_angel_func)
                {
                    (*recieve_angel_func)(data1, data2);
                }
                break;
            }
            case '!':{
                delay(1);
                int code2 = _serial->read();
                translate_segment2motor_code(code2, &result_code, &result_speed);
                count = 20;
                break;
            }

            case '~':{
                delay(1);
                int code3 = _serial->read();
                int data3 = char2int(_serial);
                translate_continue2motor_code(code3, data3, &result_code, &result_speed);
                if (is_debug)
                {
                    Serial.print("");
                    Serial.write(code3);
                    Serial.println(data3);
                }
                count = 20;
                break;
            }
            default:{
                if (is_debug){
                    Serial.println(count);
                }
                if (count)
                {
                    --count;
                }
                else
                {
                    if (recieve_stop_func)
                    {
                        (*recieve_stop_func)();
                    }
                }
                break;
            }
            
        }
    }

    if (recieve_move_func && result_speed != -1)
    {   
        (*recieve_move_func)(result_code, result_speed);
    }
}

void Communicate::register_recieve_wheel_pwm(void (*function)(int pwm0, int pwm1, int pwm2, int pwm3))
{
    recieve_wheel_pwm = function;
}

void Communicate::register_recieve_angle(void (*function)(int now_angle, int target_angle))
{
    recieve_angel_func = function;
}
void Communicate::register_recieve_move_code(void (*function)(char dir_code, int speed))
{
    recieve_move_func = function;
}
void Communicate::register_recieve_stop(void (*function)(void))
{
    recieve_stop_func = function;
}

int Communicate::char2int(HardwareSerial *Serial_temp)
{
    char data[3] = {
        Serial_temp->read(),
        Serial_temp->read(),
        Serial_temp->read()
        };

    return char2int(data);
}

int Communicate::char2int(char *data)
{
    char temp[3] = {
        data[0] - '0',
        data[1] - '0',
        data[2] - '0'
        };
    return int(temp[0]) * 100 + temp[1] * 10 + temp[2];
}

void Communicate::translate_segment2motor_code(char sengment_code, char *result_code, int *result_speed)
{
    // 這裡還沒打！！！！！！！！
}
void Communicate::translate_continue2motor_code(char continue_code, int speed, char *result_code, int *result_speed)
{
    if (continue_code == -1 && is_debug){
        Serial.println("translate_continue2motor_code BUG");
    }
    *result_code = continue_code;
    *result_speed = speed;
}

void Communicate::clear_serial_buffer(){
    while(_serial->available()){
        _serial->read();
    }
}