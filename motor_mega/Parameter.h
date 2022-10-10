#define _MY_BAUD_ 115200
#define _LITTLE_TIME_ 200
#define _POSITIVE_STRENGTH_ 2  // 回正強度，若越高、回正效果越強，基本上是1
#define _PID_P_ 3
#define _PID_I_ 3
#define _COMMU_DELAY_ 25  // 值越高，則跑動後等待時間越久，_COMMU_DELAY_ * 5ms
#define _MOVE_NO_STOP_ // 啟動後，若沒有傳送新訊息，馬達就會一直往上次傳輸的方向移動