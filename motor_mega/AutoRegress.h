
#if !defined(_AUTO_REGRESS_H_)
#define _AUTO_REGRESS_H_

#include <Arduino.h>

class AutoRegress{

    private:
    int p_adjust, i_adjust;
    int i_error;  // 積分儲存用
    unsigned long last_time;  // 上次時間，用來積分時計算delta_time
    long count;  // 計算系統到現在跑了多少次


    public:
    AutoRegress();
    AutoRegress(int p_adjust, int i_adjust);
    ~AutoRegress();
    int calculate_w(int target_angle, int now_angle);  // 計算並回傳角度，回傳值是一個角度，可直接丟進motor_control裡面

    void start_loss_calculate();  // 啟動計算loss值，評斷PID參數好壞，若越高則越爛
    int get_loss();  // 取得loss值

};


#endif // _AUTO_REGRESS_H_

