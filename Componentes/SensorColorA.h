// SensorColorA.h
#pragma once
#include <Wire.h>
#include <Adafruit_TCS34725.h>

struct SensorColor {

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

        //Azul celeste
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 1;
        }

        //Amarillo
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 2;
        }

        //Naranja
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 3;
        }

        //Rosa
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 4;
        }

        //Rojo salida
        if(Rojo < 1 and Rojo > 0 and Verde < 1 and Verde > 0 and Azul < 1 and Azul > 0){
            return 5;
        }

        // aqui falta decidir como traducir R,G,B a cada color
        return 6;
    }

};