// IMU.h
#pragma once
#include <Arduino.h>
#include <Wire.h>

#define MPU_WHO_AM_I      0x75
#define MPU_PWR_MGMT_1    0x6B
#define MPU_GYRO_CONFIG   0x1B
#define MPU_GYRO_ZOUT_H   0x47
#define MPU_SMPLRT_DIV    0x19
#define MPU_CONFIG        0x1A
#define MPU_FIFO_EN       0x23
#define MPU_USER_CTRL     0x6A
#define MPU_FIFO_COUNT_H  0x72
#define MPU_FIFO_R_W      0x74

struct IMU {

    int DireccionI2C = 0x69;

    float OffsetGiroZ = 0;            // grados/s que marca el giroscopio con el robot quieto
    float Rumbo = 0;                  // grados acumulados desde la calibracion
    float PeriodoMuestra = 0.01;      // 100 muestras por segundo
    bool FIFODesbordada = false;      // true si se perdieron muestras

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

    void IniciarFIFO(){

        EscribirRegistro(MPU_USER_CTRL, 0x04);  // vaciar la FIFO
        delay(10);
        EscribirRegistro(MPU_FIFO_EN, 0x10);    // guardar solo el giroscopio Z
        EscribirRegistro(MPU_USER_CTRL, 0x40);  // encender la FIFO

    }

    void Inicializar(){

        // Wire.begin() ya se hizo en setup()
        EscribirRegistro(MPU_PWR_MGMT_1, 0x00);   // despertar el sensor
        delay(100);
        EscribirRegistro(MPU_GYRO_CONFIG, 0x00);  // rango de +-250 grados/s
        EscribirRegistro(MPU_CONFIG, 0x03);       // filtro interno, el giroscopio muestrea a 1 kHz
        EscribirRegistro(MPU_SMPLRT_DIV, 9);      // 1000 / (1 + 9) = 100 muestras por segundo

        IniciarFIFO();

    }

    // Velocidad de giro en el eje Z, en grados por segundo (lectura directa, solo para calibrar)
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
        FIFODesbordada = false;

        IniciarFIFO();   // descartar lo que se junto durante la calibracion

    }

    // Lee todas las muestras que la FIFO guardo desde la ultima vez y las suma al rumbo
    void Actualizar(){

        uint8_t Conteo[2];
        LeerRegistros(MPU_FIFO_COUNT_H, Conteo, 2);
        int Bytes = (Conteo[0] << 8) | Conteo[1];

        if(Bytes >= 1024){          // la FIFO se lleno: se perdieron muestras
            IniciarFIFO();
            FIFODesbordada = true;
            return;
        }

        Bytes = Bytes - (Bytes % 2);   // solo muestras completas (2 bytes cada una)

        while(Bytes > 0){

            int Leer = Bytes;
            if(Leer > 30){
                Leer = 30;
            }

            uint8_t Datos[30];
            int Leidos = LeerRegistros(MPU_FIFO_R_W, Datos, Leer);
            if(Leidos != Leer){
                return;
            }

            for(int i = 0; i < Leer; i += 2){
                int16_t Crudo = (int16_t)((Datos[i] << 8) | Datos[i + 1]);
                Rumbo += (Crudo / 131.0 - OffsetGiroZ) * PeriodoMuestra;
            }

            Bytes -= Leer;

        }

    }

    // Rumbo de 0 a 360 grados, relativo a la posicion donde se calibro
    float LeerRumbo(){

        Actualizar();

        float R = Rumbo;
        Serial.print("La brujula detectó ");
        Serial.println(Rumbo);

        while(R < 0){
            R += 360;
        }
        while(R >= 360){
            R -= 360;
        }

        return R;

    }

};