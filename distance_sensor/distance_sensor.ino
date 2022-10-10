#include <Arduino.h>
#include "FallDetect.h"
#include "pin.h"
byte sensor[] = {A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};

FallDetect *fallDetect;
FallPin pins;

void setup(){
    fallDetect = new FallDetect();
    Serial.begin(115200);
    Serial.println("start");
}

void loop(){

    // for (int i=0;i<12;i++){
    //     float v = analogRead(sensor[i]) * 0.0048828152;
    //     float distance = 13 * pow(v, -1);
    //     delay(100);
    //     Serial.print("pin ");
    //     Serial.print("A");
    //     Serial.print(i + 4);
    //     Serial.print(" : ");
    //     if (distance <= 30){
    //         // Serial.println(v);
    //         Serial.print(distance);
    //     }else{
    //         Serial.print("out");
    //     }
    //         Serial.print(" ; ");
    // }
    // Serial.println();

    // Serial.print("lff  ");
    // Serial.println(fallDetect->get_distance(pins.lff));
    Serial.println(13 * pow((analogRead(pins.rmf) * 0.0048828125), -1));
    delay(300);



}