#if !defined(_COMMUNICATION_H_)
#define _COMMUNICATION_H_

#include <Arduino.h>
#include <HardwareSerial.h>

class Communicate
{
private:
    void (*recieve_wheel_pwm)(int pwm0, int pwm1, int pwm2, int pwm3);
    void (*recieve_angel_func)(int now_angle, int target_angle);
    void (*recieve_move_func)(char dir_code, int speed);
    void (*recieve_stop_func)(void);
    HardwareSerial *_serial;
    bool is_debug; // 啟動debug訊息
    int char2int(HardwareSerial *Serial_temp);
    int char2int(char *data);
    void translate_segment2motor_code(char sengment_code, char *result_code, int *result_speed); // 把segment指令轉成馬達使用的指令
    void translate_continue2motor_code(char continue_code, int speed, char *result_code, int *result_speed);        // 把continue指令轉成馬達使用的指令

public:
    // Communication();
    Communicate();
    ~Communicate();
    void attach_Serial(HardwareSerial *s);
    void start_debug_with_Serial0(); // 程式啟動時執行

    void read_code_buffer(); // 放進 loop 迴圈 裡面一直跑

    void register_recieve_wheel_pwm(void (*function)(int pwm0, int pwm1, int pwm2, int pwm3));                // 當收到四顆馬達資料時執行
    void register_recieve_angle(void (*function)(int now_angle, int target_angle));                // 當收到角度資料時執行
    void register_recieve_move_code(void (*function)(char dir_code, int speed)); // 當收到continue資料時執行
    void register_recieve_stop(void (*function)(void));

    void recieve_setup_code();

    void clear_serial_buffer();
};

#endif // _COMMUNICATION_H_
