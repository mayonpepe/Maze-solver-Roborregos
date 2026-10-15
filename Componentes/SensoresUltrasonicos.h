#pragma once
#include <Arduino.h>

struct SensorUltrasonico {

    int PinTrigger;
    int PinEcho;

    SensorUltrasonico(int Trigger, int Echo){
        PinTrigger = Trigger;
        PinEcho = Echo;
    }

    void Inicializar(){
        pinMode(PinTrigger, OUTPUT);
        pinMode(PinEcho, INPUT);
    }

    float LeerDistancia(){
        digitalWrite(PinTrigger, LOW);
        delayMicroseconds(2);
        digitalWrite(PinTrigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(PinTrigger, LOW);

        long Duracion = pulseIn(PinEcho, HIGH);
        return Duracion * 0.0343 / 2;
    }

    bool HayPared(float Umbral){
        return LeerDistancia() < Umbral;
    }

};

struct SensoresUltrasonicos {

    SensorUltrasonico Frontal, Derecho, Trasero, Izquierdo;
    float UmbralPared = 15.0;

    SensoresUltrasonicos() : Frontal(2, 3), Derecho(4, 5), Trasero(6, 7), Izquierdo(8, 9) {}

    void Inicializar(){
        Frontal.Inicializar();
        Derecho.Inicializar();
        Trasero.Inicializar();
        Izquierdo.Inicializar();
    }

    int SensorUS(int n){
        switch(n){
            case 1:
                if(Frontal.HayPared(UmbralPared)){
                    return 1;
                }
                else return 0;

            case 2:
                if(Derecho.HayPared(UmbralPared)){
                    return 1;
                }
                else return 0;

            case 3:
                if(Trasero.HayPared(UmbralPared)){
                    return 1;
                }
                else return 0;

            case 4:
                if(Izquierdo.HayPared(UmbralPared)){
                    return 1;
                }
                else return 0;

            default:
                return -1;
        }
    }

};