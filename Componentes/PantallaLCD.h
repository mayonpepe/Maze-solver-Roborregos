// PantallaLCD.h
#pragma once
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

struct PantallaLCD {

    LiquidCrystal_I2C Lcd = LiquidCrystal_I2C(0x27, 16, 2);

    void Inicializar(){
        Lcd.init();
        Lcd.backlight();
    }

    void Limpiar(){
        Lcd.clear();
    }

    void ImprimirText(int Linea, const char* Texto){
        Lcd.setCursor(0, Linea);
        Lcd.print(Texto);
    }

    void ImprimirNum(int Linea, int Numero){
        Lcd.setCursor(0, Linea);
        Lcd.print(Numero);
    }

};