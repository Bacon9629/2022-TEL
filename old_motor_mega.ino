#include <Arduino.h>

bool auto_regress_active = false;  // 移動校正系統設定，true為啟動

const int lf_pwm_pin = 9;
const int lb_pwm_pin = 6;
const int rf_pwm_pin = 5;
const int rb_pwm_pin = 3;

const int lf_dir_pin = 36;
const int lb_dir_pin = 34;
const int rf_dir_pin = 32;
const int rb_dir_pin = 30;

const int all_pwm_pin[4] = {lf_pwm_pin, lb_pwm_pin, rf_pwm_pin, rb_pwm_pin};
const int all_dir_pin[4] = {lf_dir_pin, lb_dir_pin, rf_dir_pin, rb_dir_pin};

const int speed_range[] = {80, 150, 200, 255};


int offset_angle = 0;
int target_angle = 0;
int speed = 0;
char dir_code = ' '; // 可能隨著不同type編碼方式不同，此變數僅是暫存用的而已
int count = 0;

void continue_control(int code);  // 無段控制
void segment_control(int code);  // 有段控制程式
void move(char dir, int speed);  // 控制輪子的主函式
void one_wheel_move(int which_wheel, int speed);  // 控制單輪
void stop_little_ms();  // 停止車子50ms，急變方向前需使用此function
bool check_motor_reverse(char last_dir, char now_dir);  // 確認車輪是否即將反轉

int char2int(char *data);  // 把char[3]轉成百位int
int char2int(HardwareSerial _serial);  // 自動讀取Serial的三個字元，轉籌int

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  
  for (int i=0;i<4;i++){
    pinMode(all_dir_pin[i], OUTPUT);
    digitalWrite(all_dir_pin[i], 1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  static int count = 0;
  static char code = 'p';

  // // receive_data();
  if (Serial.available()){
    code = Serial.read();
    Serial.println(code);
    count = 0;
  }

  if (count > 1000){
    Serial.println("auto stop");
    code = 'p';
    move('p', 0);
  }else{
    move(code, 100);
    count += 15;
  }

  
  
}

char receive_data(){
  /**
   * @brief 
   * 接收資料用，順便執行
   */


  // 若暫存器存超過30個字元，則清空到10個
  if (Serial2.available() > 30){
    while(Serial2.available() > 10){
      Serial2.read();
    }
  }

  char data;  // 存起始字元

  while(Serial2.available()){
    // 如果Serial2裡面還有東西則一直讀，讀到沒有為止，在讀的過程中若一直沒出現移動起始字元，則停止移動

    data = Serial2.read();
    switch(data){
      // 讀取資料用的switch

      case '[': {
        // get offset_angle
        offset_angle = char2int(Serial2);
        // count = 20;
        break;
      }

      case '!': {
        // segment control
        dir_code = Serial2.read();
        count = 20;
        break;
      }

      case '~': {
        // continue control
        dir_code = Serial2.read();
        speed = char2int(Serial2);
        count = 20;
        break;
      }

      default:{
        // 若讀的過程中一直沒出現移動起始字元則停止移動
        if (count){
          --count;
        }else{
          speed = 0;
          dir_code = 'p';
        }

      }

    }
  }  

  switch (data)
  {
    // 執行用的switch

    case '!':{
      segment_control(dir_code);
      break;
    }

    case '~':{
      continue_control(dir_code, speed);
      break;
    }
  }
  
}

void continue_control(char dir, int speed){
  /*
  無段控制程式，code編碼詳見PDF
  */

  move(dir, speed);

}

void segment_control(int code){
  /*
  有段控制程式，code編碼詳見PDF
  */

  if (code == 'Z'){  // 對正回正角度
    move('z', 0);
  }else if (code == 'p'){  // 停止
    move('p', 0);
  }else if(code >= 'A' && code <= 'D'){  // 前
    move('w', code - 'A');
  }else if(code >= 'E' && code <= 'H'){  // 後
    move('s', code - 'E');
  }else if(code >= 'I' && code <= 'L'){  // 左
    move('a', code - 'I');
  }else if(code >= 'M' && code <= 'P'){  // 右
    move('d', code - 'M');
  }else if(code >= 'a' && code <= 'd'){  // 左前
    move('q', code - 'a');
  }else if(code >= 'i' && code <= 'l'){  // 右前
    move('e', code - 'i');
  }else if(code >= 'm' && code <= 'p'){  // 左後
    move('z', code - 'm');
  }else if(code >= 'e' && code <= 'h'){  // 右後
    move('c', code - 'e');
  }

}

void move(char dir, int speed){
  /*
  dir : 角度
    w:前
    a:左
    s:後
    d:右
    p:停止
    q:左前
    e:右前
    z:左後
    c:右後
    n:逆時鐘轉
    m:順時鐘轉

    z:對正回正角度、使用回正指令期間須不斷傳送角度和移動編碼

  speed : 速度 [0, 255]
  */
  static char last_dir = ' ';


  // if (check_motor_reverse(last_dir, dir)){
  //   stop_little_ms();
  // }

  int reuslt_speeds[4];
  speed_control(dir, speed, reuslt_speeds);
  for (int i=0;i<4;i++){
    one_wheel_move(i, reuslt_speeds[i]);
  }

  last_dir = dir;
  delay(15);

}


void speed_control(char dir, int target_speed, int result_speeds[]){
  /**
   * @brief 
   * dir: 方向
   * target_speed: 目標速度
   * result_speeds: 使用者傳入一個空陣列，此函式會把每一個輪子的速度設定調整後放進result_speeds陣列內  * 
   * 
   * 參考麥克納姆倫運動學模型: https://mp.weixin.qq.com/s/qPBFqa_-ay4WZG1jWUvEfw
   */
  if (target_speed == 0 || dir == 'p'){
    result_speeds[0] = 0;
    result_speeds[1] = 0;
    result_speeds[2] = 0;
    result_speeds[3] = 0;
    return;
  }

  int positive_strength = 1;  // 回正強度，若越高、回正效果越強，基本上是1
  int w = 0;
  // 運動學模型矩陣
  int matrix[][3] = {
    {1, -1, -positive_strength}, // 左前
    {1, 1, -positive_strength},  // 左後
    {1, 1, positive_strength},   // 右前
    {1, -1, positive_strength}   // 右後
  };

  // 設定運動模型的目標角度值
  int _temp_dir = 90;
  if (dir == 'w'){
    _temp_dir = 90;
  }else if(dir == 's'){
    _temp_dir = -90;
  }else if(dir == 'a'){
    _temp_dir = 180;
  }else if(dir == 'd'){
    _temp_dir = 0;
  }else if(dir == 'p'){
    _temp_dir = 90;
  }else if(dir == 'q'){
    _temp_dir = 135;
  }else if(dir == 'e'){
    _temp_dir = 45;
  }else if(dir == 'z'){
    _temp_dir = 225;
  }else if(dir == 'c'){
    _temp_dir = -45;
  }else if(dir == 'n'){
    result_speeds[0] = -target_speed;
    result_speeds[1] = -target_speed;
    result_speeds[2] = target_speed;
    result_speeds[3] = target_speed;
    return;
  }else if(dir == 'm'){
    result_speeds[0] = target_speed;
    result_speeds[1] = target_speed;
    result_speeds[2] = -target_speed;
    result_speeds[3] = -target_speed;
    return;
  }else if(dir == 'z'){
    
    if(offset_angle > 2){
      // 逆時針
      int temp = offset_angle > 10 ? 120 : 70;
      result_speeds[0] = -temp;
      result_speeds[1] = -temp;
      result_speeds[2] = temp;
      result_speeds[3] = temp;
    }

    if(offset_angle < -2){  
      // 順時針
      int temp = offset_angle < -10 ? 120 : 70;
      result_speeds[0] = temp;
      result_speeds[1] = temp;
      result_speeds[2] = -temp;
      result_speeds[3] = -temp;
    }

  }
  int vx = round(target_speed * sin(_temp_dir * PI / 180));// y軸的向量
  int vy= round(target_speed * cos(_temp_dir * PI / 180));// x軸的向量

  int _matrix_2[4][1] = {{vx}, {vy}, {w}};
  bool is_over = false;

  // 把matrix 和 _matrix_2 做點積運算
  for (int i=0;i<4;i++){
    int _result = 0;
    for (int j=0;j<3;j++){
      _result += matrix[i][j] * _matrix_2[j][0];
    }
    result_speeds[i] = _result;  // 回傳速度
    if (_result > 255 || _result < -255) is_over = true;  // 如果有速度超過255、則要做某件事
    
  }

  // 如果有超過255、-255的值則要做，若超過則四個值等比例縮減，把最大值壓縮進255內
  if (is_over){
    int max = -500;
    for (int i=0;i<4;i++){  // 找離0最遠的值
      if (result_speeds[i] > max || result_speeds[i] < -max){
        max = result_speeds[i];
      }
    }
    int reduction_ratio = 255 / (max > 0 ? max : -max);  // 壓縮比，把最大值壓縮進255
    for (int i=0;i<4;i++){  // 等倍率縮小
      result_speeds[i] *= reduction_ratio;
    }
  }

}

void one_wheel_move(int which_wheel, int speed){
  /*
  which_wheel: 選擇哪個輪子
  int speed : 速度，若小於0則
  */

  analogWrite(all_pwm_pin[which_wheel], 255 - (speed < 0 ? -speed : speed));

  if (which_wheel < 2){
    digitalWrite(all_dir_pin[which_wheel], (speed < 0 ? false : true));
  }else{
    digitalWrite(all_dir_pin[which_wheel], (speed < 0 ? true : false));
  }

}

int char2int(char *data){
  char temp[3] = {
    data[0] - '0',
    data[1] - '0',
    data[2] - '0'
  };
  return int(temp[0]) * 100 + temp[1] * 10 + temp[2];
}

int char2int(HardwareSerial Serial_temp){
    char data[3] = {
        Serial_temp.read(),
        Serial_temp.read(),
        Serial_temp.read()
        };

    return char2int(data);
}

void stop_little_ms(){
  move('p', 0);
  delay(100);
}

bool check_motor_reverse(char last_dir, char now_dir){
  /*
  確認馬達是否反轉
  */

  // char *reverse_combination[2] = {
  //   "ws", "ad", 
  // }

  if (last_dir == ' '){
    return false;
  }
  return last_dir != now_dir;
  

}