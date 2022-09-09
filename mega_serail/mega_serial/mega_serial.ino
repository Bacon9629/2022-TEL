#include <Arduino.h>
#include "HandServo.h"
#include "pin.h"
#include <Servo.h>

HandServo hand_servo;

Servo sa, sb;

void setup(){
    Serial.begin(115200);

    hand_servo.attach(hand_servos_pin, pump_pin);
    pinMode(44, INPUT_PULLUP);
    pinMode(46, INPUT_PULLUP);
    pinMode(50, INPUT_PULLUP);
    pinMode(52, INPUT_PULLUP);

    // sa.attach(8);
    // sb.attach(9);
}

void loop(){

    // int a = analogRead(A0);
    // int b = analogRead(A1);

    // a = map(a, 0, 1024, 0, 180);
    // b = map(b, 0, 1024, 0, 180);

    static int a = 130;
    static int b = 60;


    if(digitalRead(44)){
        a += 1;
    }

    if(digitalRead(46)){
        a -= 1;
    }

    if(digitalRead(50)){
        b += 1;
    }

    if(digitalRead(52)){
        b -= 1;
    }


    char code = Serial.read();
    delay(1);
    if (Serial.available() && code == '<'){
        a = char2int(&Serial);
        Serial.read();
        b = char2int(&Serial);
    }

    if (code == 10){
        char temp[10];
        sprintf(temp, "%03d, %03d", a, b);
        Serial.println(temp);
    

    }
    hand_servo.move_hand(a, b);



    // char code = Serial.read();
    // delay(1);
    // if (Serial.available() && code == '<'){
    //     a = char2int(&Serial);
    //     Serial.read();
    //     b = char2int(&Serial);
    // }
    // char temp[10];
    // sprintf(temp, "%03d, %03d", a, b);
    // Serial.println(temp);

    // sa.write(a);
    // sb.write(b);


    delay(50);
}

int char2int(HardwareSerial *Serial_temp){
    char data[3] = {
        Serial_temp->read(),
        Serial_temp->read(),
        Serial_temp->read()
        };

    Serial.write(data[0]);
    Serial.write(data[1]);
    Serial.write(data[2]);
    Serial.println();

    return char2int(data);
}

int char2int(char *data){
    char temp[3] = {
        data[0] - '0',
        data[1] - '0',
        data[2] - '0'
    };
    return int(temp[0]) * 100 + temp[1] * 10 + temp[2];
}