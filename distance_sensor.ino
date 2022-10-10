#include <Arduino.h>
byte sensor = A6;

void setup(){
    Serial.begin(115200);
    Serial.println("start");
}

void loop(){
    float v = analogRead(sensor);
    int distance = 13 * pow(v, -1);

    // if (distance <= 30){
        // Serial.println(v);
        Serial.println(distance);
    delay(300);
    // }

}