// ControladorMotores.h
#pragma once
#include <Arduino.h>

// ---------- Pines (cambialos aqui si cableas distinto) ----------

// L298N #1: motores 1 y 2
#define PIN_M1_EN   44
#define PIN_M1_IN1  22
#define PIN_M1_IN2  23


#define PIN_M2_EN   45
#define PIN_M2_IN1  24
#define PIN_M2_IN2  25

// L298N #2: motores 3 y 4
#define PIN_M3_EN   46
#define PIN_M3_IN1  26
#define PIN_M3_IN2  27
#define PIN_M4_EN   13
#define PIN_M4_IN1  28
#define PIN_M4_IN2  29

// Encoders de los canales 1 y 2 (la fase A va en un pin con interrupcion)
#define PIN_ENC1_A  18
#define PIN_ENC1_B  30
#define PIN_ENC2_A  19
#define PIN_ENC2_B  31

// ---------- Conteo de pulsos de los encoders ----------

volatile long ContadorEncoder[2] = {0, 0};

void InterrupcionEncoder1(){

    if(digitalRead(PIN_ENC1_B) == HIGH){
        ContadorEncoder[0]++;
    }
    else{
        ContadorEncoder[0]--;
    }

}

void InterrupcionEncoder2(){

    if(digitalRead(PIN_ENC2_B) == HIGH){
        ContadorEncoder[1]++;
    }
    else{
        ContadorEncoder[1]--;
    }

}

// ---------- Un motor conectado a un canal del L298N ----------

struct MotorL298N {

    int PinEN;
    int PinIN1;
    int PinIN2;

    MotorL298N(int EN, int IN1, int IN2){
        PinEN = EN;
        PinIN1 = IN1;
        PinIN2 = IN2;
    }

    void Inicializar(){

        pinMode(PinEN, OUTPUT);
        pinMode(PinIN1, OUTPUT);
        pinMode(PinIN2, OUTPUT);
        Detener();

    }

    // PWM de -255 a 255: el signo es el sentido de giro
    void Mover(int PWM){

        PWM = constrain(PWM, -255, 255);

        if(PWM > 0){
            digitalWrite(PinIN1, HIGH);
            digitalWrite(PinIN2, LOW);
            analogWrite(PinEN, PWM);
        }
        else if(PWM < 0){
            digitalWrite(PinIN1, LOW);
            digitalWrite(PinIN2, HIGH);
            analogWrite(PinEN, -PWM);
        }
        else{
            Detener();
        }

    }

    // Freno rapido: las dos entradas iguales con el enable en alto
    void Detener(){

        digitalWrite(PinIN1, LOW);
        digitalWrite(PinIN2, LOW);
        analogWrite(PinEN, 255);

    }

};

// ---------- Los 4 motores (dos L298N) con las mismas funciones de siempre ----------

struct ControladorMotores {

    long PulsosPorVuelta = 700;          // vuelve a medirlo con los encoders en el Mega (ver notas)
    float DiametroRuedaCm = 6.6;
    float DistanciaEntreRuedasCm = 12;
    int VelocidadFija = 50;
    float VelocidadMaxima = 17;          // valor de velocidad que equivale al 100% de PWM
    int Invertido[4] = {1, 1, 1, 1};     // pon -1 en los canales cuya rueda gire al reves

    unsigned long TiempoMaximoMs = 10000;   // si un movimiento por distancia tarda mas, se detiene
    bool TiempoAgotado = false;

    MotorL298N Motor1, Motor2, Motor3, Motor4;

    ControladorMotores() : Motor1(PIN_M1_EN, PIN_M1_IN1, PIN_M1_IN2),
                           Motor2(PIN_M2_EN, PIN_M2_IN1, PIN_M2_IN2),
                           Motor3(PIN_M3_EN, PIN_M3_IN1, PIN_M3_IN2),
                           Motor4(PIN_M4_EN, PIN_M4_IN1, PIN_M4_IN2) {}

    // ---- Configuracion ----

    void Inicializar(){

        Motor1.Inicializar();
        Motor2.Inicializar();
        Motor3.Inicializar();
        Motor4.Inicializar();

        pinMode(PIN_ENC1_A, INPUT_PULLUP);
        pinMode(PIN_ENC1_B, INPUT_PULLUP);
        pinMode(PIN_ENC2_A, INPUT_PULLUP);
        pinMode(PIN_ENC2_B, INPUT_PULLUP);

        attachInterrupt(digitalPinToInterrupt(PIN_ENC1_A), InterrupcionEncoder1, RISING);
        attachInterrupt(digitalPinToInterrupt(PIN_ENC2_A), InterrupcionEncoder2, RISING);

    }

    // ---- Velocidad ----

    void EnviarVelocidades(int V1, int V2, int V3, int V4){

        int V[4] = {V1, V2, V3, V4};
        int PWM[4];

        for(int i = 0; i < 4; i++){
            PWM[i] = V[i] * Invertido[i] * 255.0 / VelocidadMaxima;
        }

        Motor1.Mover(PWM[0]);
        Motor2.Mover(PWM[1]);
        Motor3.Mover(PWM[2]);
        Motor4.Mover(PWM[3]);

    }

    // Mueve un solo canal y deja los otros 3 detenidos (sirve para probar ruedas)
    void EnviarVelocidad(int Canal, int Velocidad){

        int V[4] = {0, 0, 0, 0};
        V[Canal - 1] = Velocidad;
        EnviarVelocidades(V[0], V[1], V[2], V[3]);

    }

    void Detener(){

        Motor1.Detener();
        Motor2.Detener();
        Motor3.Detener();
        Motor4.Detener();

    }

    // ---- Encoders ----

    long LeerPulsos(int Canal){

        if(Canal < 1 or Canal > 2){
            return 0;   // solo los canales 1 y 2 tienen encoder conectado
        }

        noInterrupts();
        long Valor = ContadorEncoder[Canal - 1];
        interrupts();

        return Valor;

    }

    long PulsosAbs(int Canal){

        long Pulsos = LeerPulsos(Canal);
        if(Pulsos < 0){
            return -Pulsos;
        }
        return Pulsos;

    }

    void ReiniciarPulsos(){

        noInterrupts();
        ContadorEncoder[0] = 0;
        ContadorEncoder[1] = 0;
        interrupts();

    }

    long DistanciaAPulsos(float DistanciaCm){

        float VueltasNecesarias = DistanciaCm / (3.14159 * DiametroRuedaCm);
        return VueltasNecesarias * PulsosPorVuelta;

    }

    float PulsosADistanciaCm(long Pulsos){

        float Vueltas = (float)Pulsos / PulsosPorVuelta;
        return Vueltas * 3.14159 * DiametroRuedaCm;

    }

    float DistanciaRecorridaCm(){
        return PulsosADistanciaCm(PulsosAbs(1));
    }

    // ---- Movimientos ----

    void MoverDistancia(int SignoFrontalIzq, int SignoFrontalDer, int SignoTraseraIzq, int SignoTraseraDer, float DistanciaCm){

        ReiniciarPulsos();
        TiempoAgotado = false;

        long PulsosObjetivo = DistanciaAPulsos(DistanciaCm);

        EnviarVelocidades(SignoFrontalIzq * VelocidadFija, SignoFrontalDer * VelocidadFija, SignoTraseraIzq * VelocidadFija, SignoTraseraDer * VelocidadFija);

        unsigned long Inicio = millis();

        while(PulsosAbs(1) < PulsosObjetivo and millis() - Inicio < TiempoMaximoMs){
        }

        if(PulsosAbs(1) < PulsosObjetivo){
            TiempoAgotado = true;
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
        TiempoAgotado = false;

        float ArcoCm = (abs(Grados) / 360.0) * 3.14159 * DistanciaEntreRuedasCm;
        long PulsosObjetivo = DistanciaAPulsos(ArcoCm);

        int Sentido = 1; // positivo = horario, negativo = antihorario

        if(Grados < 0){
            Sentido = -1;
        }

        EnviarVelocidades(-Sentido * VelocidadFija, Sentido * VelocidadFija, -Sentido * VelocidadFija, Sentido * VelocidadFija);

        unsigned long Inicio = millis();

        while(PulsosAbs(1) < PulsosObjetivo and PulsosAbs(2) < PulsosObjetivo and millis() - Inicio < TiempoMaximoMs){
            // espera para completar el giro
        }

        if(PulsosAbs(1) < PulsosObjetivo and PulsosAbs(2) < PulsosObjetivo){
            TiempoAgotado = true;
        }

        Detener();

    }

    void MoverContinuo(int SignoFrontalIzq, int SignoFrontalDer, int SignoTraseraIzq, int SignoTraseraDer, int Velocidad){

        ReiniciarPulsos();   // la cuenta de distancia empieza en 0 cada vez que arranca

        EnviarVelocidades(SignoFrontalIzq * Velocidad, SignoFrontalDer * Velocidad, SignoTraseraIzq * Velocidad, SignoTraseraDer * Velocidad);

    }

};