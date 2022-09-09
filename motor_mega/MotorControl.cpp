#include "MotorControl.h"
#include "Parameter.h"

MotorControl::MotorControl(byte *all_pwm_pin_, byte *all_dir_pin_)
{
    all_pwm_pin = all_pwm_pin_;
    all_dir_pin = all_dir_pin_;
}

MotorControl::~MotorControl()
{
}

void MotorControl::start_debug_with_Serial0(){
    is_debug = true;
    Serial.begin(_MY_BAUD_);
}

void MotorControl::move(char dir, int speed){
    move(dir, speed, 0);
}

void MotorControl::move(char dir, int speed, int angle_offset)
{
    static char last_dir = ' ';

    int reuslt_speeds[4];
    speed_adjust(dir, speed, angle_offset, reuslt_speeds);
    for (int i = 0; i < 4; i++)
    {
        one_wheel_move(i, reuslt_speeds[i]);
    }
    last_dir = dir;
    delay(5);
}

void MotorControl::speed_adjust(char dir, int target_speed, int w, int result_speeds[])
{
    /**
     * @brief
     * dir: 方向
     * target_speed: 目標速度
     * result_speeds: 使用者傳入一個空陣列，此函式會把每一個輪子的速度設定調整後放進result_speeds陣列內  *
     * w: now_angle - target_angle，越大則表示車子越往順時針偏移，越負則逆時針，會依此值回正
     * 參考麥克納姆倫運動學模型: https://mp.weixin.qq.com/s/qPBFqa_-ay4WZG1jWUvEfw
     */
    if (target_speed == 0 || dir == 'p')
    {
        result_speeds[0] = 0;
        result_speeds[1] = 0;
        result_speeds[2] = 0;
        result_speeds[3] = 0;
        return;
    }

    // int positive_strength = 1; // 回正強度，若越高、回正效果越強，基本上是1
    // 運動學模型矩陣
    int matrix[][3] = {
        {1, -1, -_POSITIVE_STRENGTH_}, // 左前
        {1, 1, -_POSITIVE_STRENGTH_},  // 左後
        {1, 1, _POSITIVE_STRENGTH_},   // 右前
        {1, -1, _POSITIVE_STRENGTH_}   // 右後
    };

    // 設定運動模型的目標角度值
    int _temp_dir = 90;
    if (dir == 'w')
    {
        _temp_dir = 90;
    }
    else if (dir == 's')
    {
        _temp_dir = -90;
    }
    else if (dir == 'a')
    {
        _temp_dir = 180;
    }
    else if (dir == 'd')
    {
        _temp_dir = 0;
    }
    else if (dir == 'p')
    {
        _temp_dir = 90;
    }
    else if (dir == 'q')
    {
        _temp_dir = 135;
    }
    else if (dir == 'e')
    {
        _temp_dir = 45;
    }
    else if (dir == 'z')
    {
        _temp_dir = 225;
    }
    else if (dir == 'c')
    {
        _temp_dir = -45;
    }
    else if (dir == 'n')
    {
        result_speeds[0] = -target_speed;
        result_speeds[1] = -target_speed;
        result_speeds[2] = target_speed;
        result_speeds[3] = target_speed;
        return;
    }
    else if (dir == 'm')
    {
        result_speeds[0] = target_speed;
        result_speeds[1] = target_speed;
        result_speeds[2] = -target_speed;
        result_speeds[3] = -target_speed;
        return;
    }
    else if (dir == 'z')
    {

        if (w > 2)
        {
            // 逆時針
            int temp = w > 10 ? 120 : 70;
            result_speeds[0] = -temp;
            result_speeds[1] = -temp;
            result_speeds[2] = temp;
            result_speeds[3] = temp;
        }

        if (w < -2)
        {
            // 順時針
            int temp = w < -10 ? 120 : 70;
            result_speeds[0] = temp;
            result_speeds[1] = temp;
            result_speeds[2] = -temp;
            result_speeds[3] = -temp;
        }
    }
    int vx = round(target_speed * sin(_temp_dir * PI / 180)); // y軸的向量
    int vy = round(target_speed * cos(_temp_dir * PI / 180)); // x軸的向量
    
    if (is_debug){
        Serial.print("vx, vx : ");
        Serial.print(target_speed);
        Serial.print(" ");
        Serial.print(vx);
        Serial.print(" ");
        Serial.println(vy);

    }

    int _matrix_2[4][1] = {{vx}, {vy}, {w}};
    bool is_over = false;

    // 把matrix 和 _matrix_2 做點積運算
    for (int i = 0; i < 4; i++)
    {
        int _result = 0;
        for (int j = 0; j < 3; j++)
        {
            _result += matrix[i][j] * _matrix_2[j][0];
        }
        result_speeds[i] = _result; // 回傳速度
        if (_result > 255 || _result < -255)
            is_over = true; // 如果有速度超過255、則要做某件事
    }

    // 如果有超過255、-255的值則要做，若超過則四個值等比例縮減，把最大值壓縮進255內
    if (is_over)
    {
        int max = 255;
        int min = -255;
        for (int i = 0; i < 4; i++)
        { // 找離0最遠的值
            if (result_speeds[i] > max){
                max = result_speeds[i];
            }else if (result_speeds[i] < min){
                min = result_speeds[i];
            }
        }
        // int reduction_ratio = 255 / (max > 0 ? max : -max); // 壓縮比，把最大值壓縮進255
        for (int i = 0; i < 4; i++)
        { // 等倍率縮小
            // result_speeds[i] *= reduction_ratio;
            result_speeds[i] = map(result_speeds[i], 255, -255, max, min);
        }
    }
}
void MotorControl::one_wheel_move(int which_wheel, int speed)
{
    /*
  which_wheel: 選擇哪個輪子
  int speed : 速度，若小於0則
  */

    if (is_debug){
        Serial.print(which_wheel);
        Serial.print(" ");
        Serial.println(speed);
    }

    analogWrite(all_pwm_pin[which_wheel], 255 - (speed < 0 ? -speed : speed));

    if (which_wheel == 0 || which_wheel == 1)
    {
        digitalWrite(all_dir_pin[which_wheel], (speed < 0 ? false : true));
    }
    else
    {
        digitalWrite(all_dir_pin[which_wheel], (speed < 0 ? true : false));
    }
}

void MotorControl::stop_little_time(){
    move('p', 0);
    delay(_LITTLE_TIME_);
}

void MotorControl::stop(){
    move('p', 0);
}