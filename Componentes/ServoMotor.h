#pragma once
#include <Arduino.h>
#include <Servo.h>

struct ServoMotor {

    int PinServo;
    int AnguloActual = 90;
    Servo Motor;

    ServoMotor(int Pin){
        PinServo = Pin;
    }

    void Inicializar(){
        Motor.attach(PinServo);
        Motor.write(AnguloActual);
    }

    void MoverA(int Angulo){
        AnguloActual = constrain(Angulo, 0, 180);
        Motor.write(AnguloActual);
    }

    int LeerAngulo(){
        return AnguloActual;
    }

};