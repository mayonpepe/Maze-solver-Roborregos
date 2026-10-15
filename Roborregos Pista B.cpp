#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>
#include "Componentes/SensorColorB.h"
#include "Componentes/SensoresUltrasonicos.h"
#include "Componentes/ControladorMotores.h"
#include "Componentes/IMU.h"
#include "Componentes/ServoMotor.h"

//Pendiente
struct CamaraRB {

    bool DetectarPelota(){
        return 0;
        return 1;
    }

}

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