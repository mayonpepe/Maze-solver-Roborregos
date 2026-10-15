// ControladorMotores.h
#pragma once
#include <Arduino.h>
#include <Wire.h>

#define MOTOR_TYPE_ADDR               0x14
#define MOTOR_ENCODER_POLARITY_ADDR   0x15
#define MOTOR_FIXED_SPEED_ADDR        0x33
#define MOTOR_ENCODER_TOTAL_ADDR      0x3C

#define MOTOR_TYPE_WITHOUT_ENCODER        0
#define MOTOR_TYPE_TT                     1
#define MOTOR_TYPE_N20                    2
#define MOTOR_TYPE_JGB37_520_12V_110RPM   3

struct ControladorMotores {

    int DireccionI2C = 0x34;

    long PulsosPorVuelta = 700;
    float DiametroRuedaCm = 6.6;
    float DistanciaEntreRuedasCm = 12;
    int VelocidadFija = 50;

    uint8_t TipoMotor = MOTOR_TYPE_JGB37_520_12V_110RPM; // el mismo que usa tu ejemplo
    uint8_t PolaridadEncoder = 0;
    int Invertido[4] = {1, 1, 1, 1}; // pon -1 en los canales cuya rueda gire al reves

    bool WireWriteByte(uint8_t val){

        Wire.beginTransmission(DireccionI2C);
        Wire.write(val);
        if(Wire.endTransmission() != 0){
            return false;
        }
        return true;

    }

    bool WireWriteDataArray(uint8_t reg, uint8_t *val, unsigned int len){

        unsigned int i;

        Wire.beginTransmission(DireccionI2C);
        Wire.write(reg);
        for(i = 0; i < len; i++){
            Wire.write(val[i]);
        }
        if(Wire.endTransmission() != 0){
            return false;
        }
        return true;

    }

    int WireReadDataArray(uint8_t reg, uint8_t *val, unsigned int len){

        unsigned char i = 0;

        if(!WireWriteByte(reg)){
            return -1;
        }

        Wire.requestFrom((uint8_t)DireccionI2C, (uint8_t)len);
        while(Wire.available()){
            if(i >= len){
                return -1;
            }
            val[i] = Wire.read();
            i++;
        }

        return i;

    }

    // ---- Configuracion ----

    void Inicializar(){

        // Wire.begin() ya se hizo en setup()
        delay(200);
        WireWriteDataArray(MOTOR_TYPE_ADDR, &TipoMotor, 1);
        delay(5);
        WireWriteDataArray(MOTOR_ENCODER_POLARITY_ADDR, &PolaridadEncoder, 1);

    }

    // ---- Velocidad: las 4 juntas en una sola escritura, como en tu ejemplo ----

    void EnviarVelocidades(int V1, int V2, int V3, int V4){

        int V[4] = {V1, V2, V3, V4};
        int8_t Datos[4];

        for(int i = 0; i < 4; i++){
            Datos[i] = constrain(V[i] * Invertido[i], -127, 127);
        }

        WireWriteDataArray(MOTOR_FIXED_SPEED_ADDR, (uint8_t*)Datos, 4);

    }

    // Mueve un solo canal y deja los otros 3 en 0 (sirve para probar ruedas)
    void EnviarVelocidad(int Canal, int Velocidad){

        int V[4] = {0, 0, 0, 0};
        V[Canal - 1] = Velocidad;
        EnviarVelocidades(V[0], V[1], V[2], V[3]);

    }

    void Detener(){
        EnviarVelocidades(0, 0, 0, 0);
    }

    // ---- Encoders ----

    long LeerPulsos(int Canal){

        int32_t Pulsos[4];
        WireReadDataArray(MOTOR_ENCODER_TOTAL_ADDR, (uint8_t*)Pulsos, 16);

        return Pulsos[Canal - 1];

    }

    long PulsosAbs(int Canal){

        long Pulsos = LeerPulsos(Canal);
        if(Pulsos < 0){
            return -Pulsos;
        }
        return Pulsos;

    }

    // Pone en 0 los pulsos de los 4 canales
    void ReiniciarPulsos(){

        int8_t Reinicio[16] = {0};
        WireWriteDataArray(MOTOR_ENCODER_TOTAL_ADDR, (uint8_t*)Reinicio, 16);

    }

    long DistanciaAPulsos(float DistanciaCm){

        float VueltasNecesarias = DistanciaCm / (3.14159 * DiametroRuedaCm);
        return VueltasNecesarias * PulsosPorVuelta;

    }

    float PulsosADistanciaCm(long Pulsos){

        float Vueltas = (float)Pulsos / PulsosPorVuelta;
        return Vueltas * 3.14159 * DiametroRuedaCm;

    }

    // ---- Movimientos ----

    void MoverDistancia(int SignoFrontalIzq, int SignoFrontalDer, int SignoTraseraIzq, int SignoTraseraDer, float DistanciaCm){

        ReiniciarPulsos();

        long PulsosObjetivo = DistanciaAPulsos(DistanciaCm);

        EnviarVelocidades(SignoFrontalIzq * VelocidadFija, SignoFrontalDer * VelocidadFija, SignoTraseraIzq * VelocidadFija, SignoTraseraDer * VelocidadFija);

        while(PulsosAbs(1) < PulsosObjetivo){
        }

        Detener();

    }

    void Movimiento1(float DistanciaCm){
        MoverDistancia(1, 1, 1, 1, DistanciaCm);
    }

    void Movimiento2(float DistanciaCm){
        MoverDistancia(1, -1, -1, 1, DistanciaCm);
    }

    void Movimiento3(float DistanciaCm){
        MoverDistancia(-1, -1, -1, -1, DistanciaCm);
    }

    void Movimiento4(float DistanciaCm){
        MoverDistancia(-1, 1, 1, -1, DistanciaCm);
    }

    void Girar(float Grados){

        ReiniciarPulsos();

        float ArcoCm = (abs(Grados) / 360.0) * 3.14159 * DistanciaEntreRuedasCm;
        long PulsosObjetivo = DistanciaAPulsos(ArcoCm);

        int Sentido = 1; // positivo = horario, negativo = antihorario

        if(Grados < 0){
            Sentido = -1;
        }

        EnviarVelocidades(-Sentido * VelocidadFija, Sentido * VelocidadFija, -Sentido * VelocidadFija, Sentido * VelocidadFija);

        while(PulsosAbs(1) < PulsosObjetivo and PulsosAbs(2) < PulsosObjetivo){
            // espera para completar el giro
        }

        Detener();

    }

    void MoverContinuo(int SignoFrontalIzq, int SignoFrontalDer, int SignoTraseraIzq, int SignoTraseraDer, int Velocidad){
        EnviarVelocidades(SignoFrontalIzq * Velocidad, SignoFrontalDer * Velocidad, SignoTraseraIzq * Velocidad, SignoTraseraDer * Velocidad);
    }

};