// SensorColorB.h
#pragma once
#include <Wire.h>
#include <Adafruit_TCS34725.h>

struct SensorColorB {

    Adafruit_TCS34725 Sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

    void Inicializar(){
        Sensor.begin();
    }

    int LeerColor(){
        uint16_t R, G, B, C;
        
        Sensor.getRawData(&R, &G, &B, &C);

        float R1 = R;
        float G1 = G;
        float B1 = B;
        float C1 = C;

        float Rojo = R1 / C1;
        float Verde = G1 / C1;
        float Azul = B1 / C1;

        //Blanco
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 0;
        }

        //Rojo checkpoint
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 1;
        }

        //Naranja B
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 2;
        }

        //Amarillo B
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 3;
        }

        //Azul B
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 4;
        }

        //Rosa B
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 5;
        }

        //Verde inicio y fin
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 6;
        }

        //Verde Seccion 2
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 7;
        }

        // aqui falta decidir como traducir R,G,B a cada color
        return 8;
    }

};