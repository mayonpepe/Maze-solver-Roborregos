// IMU.h
#pragma once
#include <Arduino.h>
#include <Wire.h>

#define MPU_WHO_AM_I     0x75
#define MPU_PWR_MGMT_1   0x6B
#define MPU_GYRO_CONFIG  0x1B
#define MPU_GYRO_ZOUT_H  0x47

struct IMU {

    int DireccionI2C = 0x68;

    float OffsetGiroZ = 0;          // grados/s que marca el giroscopio con el robot quieto
    float Rumbo = 0;                // grados acumulados desde la calibracion
    unsigned long UltimoTiempo = 0;

    void EscribirRegistro(uint8_t Registro, uint8_t Valor){

        Wire.beginTransmission(DireccionI2C);
        Wire.write(Registro);
        Wire.write(Valor);
        Wire.endTransmission();

    }

    int LeerRegistros(uint8_t Registro, uint8_t *Datos, uint8_t Cantidad){

        Wire.beginTransmission(DireccionI2C);
        Wire.write(Registro);
        if(Wire.endTransmission(false) != 0){
            return -1;
        }

        Wire.requestFrom((uint8_t)DireccionI2C, (uint8_t)Cantidad);

        uint8_t i = 0;
        while(Wire.available() and i < Cantidad){
            Datos[i] = Wire.read();
            i++;
        }
        return i;

    }

    uint8_t LeerID(){

        uint8_t ID = 0;
        LeerRegistros(MPU_WHO_AM_I, &ID, 1);
        return ID;

    }

    void Inicializar(){

        // Wire.begin() ya se hizo en setup()
        EscribirRegistro(MPU_PWR_MGMT_1, 0x00);   // despertar el sensor
        delay(100);
        EscribirRegistro(MPU_GYRO_CONFIG, 0x00);  // rango de +-250 grados/s

    }

    // Velocidad de giro en el eje Z, en grados por segundo
    float LeerGiroZ(){

        uint8_t Datos[2];
        LeerRegistros(MPU_GYRO_ZOUT_H, Datos, 2);

        int16_t Crudo = (int16_t)((Datos[0] << 8) | Datos[1]);
        return Crudo / 131.0;

    }

    // Llamar con el robot totalmente quieto
    void Calibrar(){

        float Suma = 0;

        for(int i = 0; i < 500; i++){
            Suma += LeerGiroZ();
            delay(2);
        }

        OffsetGiroZ = Suma / 500.0;
        Rumbo = 0;
        UltimoTiempo = micros();

    }

    // Hay que llamarla seguido: suma lo que giro el robot desde la ultima vez
    void Actualizar(){

        unsigned long Ahora = micros();
        float Segundos = (Ahora - UltimoTiempo) / 1000000.0;
        UltimoTiempo = Ahora;

        Rumbo += (LeerGiroZ() - OffsetGiroZ) * Segundos;

    }

    // Rumbo de 0 a 360 grados, relativo a la posicion donde se calibro
    float LeerRumbo(){

        Actualizar();

        float R = Rumbo;

        while(R < 0){
            R += 360;
        }
        while(R >= 360){
            R -= 360;
        }

        return R;

    }

};