#include <Arduino.h>
byte sensor = A4;

void setup(){
    Serial.begin(115200);
    Serial.println("start");
}

void loop(){
    float v = 65 * pow((analogRead(sensor) * 0.0048828125), -1.10);
    // int distance = 13 * pow(v, -1);

    // if (distance <= 30){
        // Serial.println(v);
        Serial.println(v);
        delay(300);
    // }

}