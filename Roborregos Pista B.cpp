#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>

struct SensorColor {

    Adafruit_TCS34725 Sensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

    void Inicializar(){
        Sensor.begin();
    }

    int LeerColor(){
        uint16_t R, G, B, C;
        Sensor.getRawData(&R, &G, &B, &C);

        float Rojo = R / C;
        float Verde = G / C;
        float Azul = B / C;

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

struct ControladorMotores {

    int DireccionI2C = 0x34;

    long PulsosPorVuelta = 0;   // pendiente
    float DiametroRuedaCm = 0;  // pendiente
    float DistanciaEntreRuedasCm = 0; // pendiente

    void Inicializar(){
        // Pendiente
    }

    void EnviarVelocidad(int Canal, int Velocidad){
        // Canal pendiente
    }

    long LeerPulsos(int Canal){
        // Canal pendiente
        return 0;
    }

    void ReiniciarPulsos(int Canal){
        // Canal pendiente
    }

    long DistanciaAPulsos(float DistanciaCm){

        float VueltasNecesarias = DistanciaCm / (3.14159 * DiametroRuedaCm);
        return VueltasNecesarias * PulsosPorVuelta;

    }

    float PulsosADistanciaCm(long Pulsos){

        float Vueltas = (float)Pulsos / PulsosPorVuelta;
        return Vueltas * 3.14159 * DiametroRuedaCm;

    }

    void Detener(){

        EnviarVelocidad(1, 0);
        EnviarVelocidad(2, 0);
        EnviarVelocidad(3, 0);
        EnviarVelocidad(4, 0);

    }

    void MoverDistancia(int SignoFrontalIzq, int SignoFrontalDer, int SignoTraseraIzq, int SignoTraseraDer, float DistanciaCm){

        ReiniciarPulsos(1);

        long PulsosObjetivo = DistanciaAPulsos(DistanciaCm);

        EnviarVelocidad(1, SignoFrontalIzq * VelocidadFija);
        EnviarVelocidad(2, SignoFrontalDer * VelocidadFija);
        EnviarVelocidad(3, SignoTraseraIzq * VelocidadFija);
        EnviarVelocidad(4, SignoTraseraDer * VelocidadFija);

        while(abs(LeerPulsos(1)) < PulsosObjetivo){
        }

        Detener();

    }

    void Movimiento1(float DistanciaCm){
        MoverDistancia(1, 1, 1, 1, DistanciaCm);
    }

    void Movimiento3(float DistanciaCm){
        MoverDistancia(-1, -1, -1, -1, DistanciaCm);
    }

    void Movimiento2(float DistanciaCm){
        MoverDistancia(1, -1, -1, 1, DistanciaCm);
    }

    void Movimiento4(float DistanciaCm){
        MoverDistancia(-1, 1, 1, -1, DistanciaCm);
    }

    void Girar(float Grados){

        ReiniciarPulsos(1);
        ReiniciarPulsos(2);

        float ArcoCm = (abs(Grados) / 360.0) * 3.14159 * DistanciaEntreRuedasCm;
        long PulsosObjetivo = DistanciaAPulsos(ArcoCm);

        int Sentido = (Grados >= 0) ? 1 : -1; // positivo = horario, negativo = antihorario

        EnviarVelocidad(1, -Sentido * VelocidadFija); // frontal izquierda
        EnviarVelocidad(2, Sentido * VelocidadFija);  // frontal derecha
        EnviarVelocidad(3, -Sentido * VelocidadFija); // trasera izquierda
        EnviarVelocidad(4, Sentido * VelocidadFija);  // trasera derecha

        while(abs(LeerPulsos(1)) < PulsosObjetivo and abs(LeerPulsos(2)) < PulsosObjetivo){
            // espera activa hasta completar el giro
        }

        Detener();

    };

    void MoverContinuo(int SignoFrontalIzq, int SignoFrontalDer, int SignoTraseraIzq, int SignoTraseraDer, int Velocidad){

        EnviarVelocidad(1, SignoFrontalIzq * Velocidad);
        EnviarVelocidad(2, SignoFrontalDer * Velocidad);
        EnviarVelocidad(3, SignoTraseraIzq * Velocidad);
        EnviarVelocidad(4, SignoTraseraDer * Velocidad);

    }

};

struct IMU {

    int DireccionI2C;

    float LeerRumbo(){
        // lectura del compas integrado
        return 0.0;
    }

    void LeerAceleracion(float &Ax, float &Ay, float &Az){
        // lectura del acelerometro
    }

};

//Pendiente
struct CamaraRB {

    bool DetectarPelota(){
        return 0;
        return 1;
    }

}

struct ServoMotor {

    int PinServo;
    int AnguloActual = 90;
    Servo Motor;

    Servo(int Pin){
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

struct Robot {

    SensoresUltrasonicos Sensores;
    SensorColor SensorColor1;
    ServoMotor servo = Servo(13);
    ControladorMotores Motores;
    IMU Brujula;

    float DistanciaRecorrida = 0;

    int BuscarColor(int Direccion, int VelocidadLenta, int DistanciaMaximaCm){

        Motores.ReiniciarPulsos(1);
        ColorInicial = 7;

        switch(Direccion){
            //Adelante
            case 1: Motores.MoverContinuo(1, 1, 1, 1, VelocidadLenta); break;
            //Derecha
            case 2: Motores.MoverContinuo(1, -1, -1, 1, VelocidadLenta); break;
            //Izquierda
            case 3: Motores.MoverContinuo(-1, 1, 1, -1, VelocidadLenta); break;

        }

        int Color = ColorInicial;
        DistanciaRecorrida = 0;

        while(Color == ColorInicial and DistanciaRecorrida < DistanciaMaximaCm){

            Color = SensorColor1.LeerColor();
            DistanciaRecorrida = Motores.PulsosADistanciaCm(abs(Motores.LeerPulsos(1)));

        }

        Motores.Detener();

        return Color;

    }

    int PelotaNodo = 4;
    int CheckpointNodo = 4;

    void DeteccionPelotaYCheckpoint(int nodo, int SensorC, int SensorP){

        if(Sensores.SensorUS(SensorC) == 0;){
            CheckpointNodo = nodo;
        }

        if(Sensores.SensorUS(SensorP) == 0;){
            PelotaNodo = nodo;
        }

    }

    void Seccion1(){
        Motores.Movimiento1(30);

        if(Sensores.SensorUS(4) == 0 and Sensores.SensorUS(2) == 0){
            Motores.Movimiento4(30);
        }

        if(Sensores.SensorUS(2) == 1){
            Motores.Movimiento4(60);
        }

        Motores.Movimiento1(30);
        DeteccionPelotaYCheckpoint(0, 4, 2);
        Motores.Movimiento1(30);
        Motores.Movimiento2(30);
        DeteccionPelotaYCheckpoint(1, 1, 3);
        Motores.Movimiento2(30);
        Motores.Movimiento3(30);
        DeteccionPelotaYCheckpoint(2, 2, 4);
        Motores.Movimiento3(30);
        Motores.Movimiento4(30);
        DeteccionPelotaYCheckpoint(3, 3, 1);

        switch(PelotaNodo){

            case 3:
                Motores.Girar(180);

                servo.MoverA(120);
                Motores.Movimiento3(45);
                servo.MoverA(0);
                Motores.Movimiento1(45);

                Motores.Girar(180);
                break;

            case 2: 
                Motores.Movimiento2(30);
                Motores.Movimiento1(30);
                Motores.Girar(90);

                servo.MoverA(120);
                Motores.Movimiento3(45);
                servo.MoverA(0);
                Motores.Movimiento1(45);

                Motores.Girar(-90);
                Motores.Movimiento3(30);
                Motores.Movimiento4(30);
                break;
            
            case 1:
                Motores.Movimiento2(30);
                Motores.Movimiento1(60);
                Motores.Movimiento4(30);

                servo.MoverA(120);
                Motores.Movimiento3(45);
                servo.MoverA(0);
                Motores.Movimiento1(45);

                Motores.Movimiento2(30);
                Motores.Movimiento3(60);
                Motores.Movimiento4(30);
                break;

            case 0:
                Motores.Movimiento4(30);
                Motores.Movimiento1(30);
                Motores.Girar(-90);

                servo.MoverA(120);
                Motores.Movimiento3(45);
                servo.MoverA(0);
                Motores.Movimiento1(45);

                Motores.Girar(90);
                Motores.Movimiento3(30);
                Motores.Moviento2(30);
                break;

        }

        switch(CheckpointNodo){
        
            case 3:
                Motores.Girar(180);
                break;

            case 2: 
                Motores.Movimiento2(30);
                Motores.Movimiento1(30);
                Motores.Girar(90);
                break;
            
            case 1:
                Motores.Movimiento2(30);
                Motores.Movimiento1(60);
                Motores.Movimiento4(30);
                break;

            case 0:
                Motores.Movimiento4(30);
                Motores.Movimiento1(30);
                Motores.Girar(-90);
                break;
        }

        Motores.Movimiento1(30);
    }

    void Seccion2(){

        for(int i = 1; i <= 3; i++){

            Motores.Movimiento4(20);

            if(BuscarColor(1, 5, 20) == 0){

                BuscarColor(2, 5, 30);
                Motores.Movimiento2(15);
                Motores.Movimiento1(15);

                Motores.Movimiento4(DistanciaRecorrida - 5);

            }

            else{
        
                Motores.Movimiento3(20);
                Motores.Movimiento2(40);
                BuscarColor(1, 5, 20);

                BuscarColor(3, 5, 30);
                Motores.Movimiento4(15);
                Motores.Movimiento1(15);

                Motores.Movimiento2(DistanciaRecorrida - 5);

            }

        }
    }

    void Seccion3(){

        while(robot.SensorColor1.LeerColor() != 6){

            int n = robot.SensorColor1.LeerColor();

            switch(n){

                case 2:
                    robot.Motores.Movimiento1(30);
                    break;

                case 3:
                    robot.Motores.Movimiento4(30);
                    break;

                case 4:
                    robot.Motores.Movimiento2(30);
                    break;
            
                case 5:
                    robot.Motores.Movimiento3(30);
                    break;

                default: break;

            }

        }

    }

};


Robot robot;

void setup(){

    Wire.begin();

    robot.Sensores.Inicializar();
    robot.SensorColor1.Inicializar();
    robot.Motores.Inicializar();

}

void loop(){

    robot.Seccion1();

    robot.Motores.Movimiento1(30);

    robot.Seccion2();
    
    robot.Motores.Movimiento1(30);

    robot.Seccion3();







    

}