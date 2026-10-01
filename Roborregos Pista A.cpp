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

        float Rojo = R / C;
        float Verde = G / C;
        float Azul = B / C;

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

struct LedRGB {

    int PinRojo;
    int PinVerde;
    int PinAzul;

    LedRGB(int Rojo, int Verde, int Azul){
        PinRojo = Rojo;
        PinVerde = Verde;
        PinAzul = Azul;
    }

    void Inicializar(){
        pinMode(PinRojo, OUTPUT);
        pinMode(PinVerde, OUTPUT);
        pinMode(PinAzul, OUTPUT);
    }

    void Encender(int R, int G, int B){
        analogWrite(PinRojo, R);
        analogWrite(PinVerde, G);
        analogWrite(PinAzul, B);
    }

    void Apagar(){
        Encender(0, 0, 0);
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
    int VelocidadFija = 50;

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

struct Mapa {

    //Mapa que guarda la etiqueta del objetivo de cada casilla
    int MapaObjetivos[5][5] = {{0}};

    //Mapa que guarda la cantidad de veces que se ha visitado una casilla
    int MapaVisitadas[5][5] = {{0}};

    int ListaRecientes[5][3] = {
    {0, 0, 5},
    {0, 0, 4},
    {0, 0, 3},
    {0, 0, 2},
    {0, 0, 1}};

    //Condicion que sirve para mover la columna a mapear en casos especiales (Inicio y columna sin objetivos disponibles)
    int CondicionColumna = 0;

    //Funcion que mapea Mapa[][] con las etiquetas de objetivo de cada casilla, lo que sirve como una guia para el robot que le dice en que orden eligirá sus objetivos, lo hace columna a columna cuando termina todos los objetivos de una
    void MapearObjetivos(int PosicionActual[2]){

        if(PosicionActual[0] >= 2){

            for(int i = 4; i >= 0 ; i--){
                MapaObjetivos[i][PosicionActual[1] + CondicionColumna] = (5 - i) + 5 * (PosicionActual[1] + CondicionColumna);
            }

        }
        else{
            for(int i = 0; i <= 4; i++){
                MapaObjetivos[i][PosicionActual[1] + CondicionColumna] = (i + 1) + 5 * (PosicionActual[1] + CondicionColumna);
            }

        }
    }

    //Funcion que actualiza ListaRecientes[] cada movimiento con la nueva casilla visitada
    void ActualizarListaRecientes(int PosicionActual[2]){

        for(int i = 0; i < 4; i++){
            ListaRecientes[i][0] = ListaRecientes[i + 1][0];
            ListaRecientes[i][1] = ListaRecientes[i + 1][1];
        }

        ListaRecientes[4][0] = PosicionActual[0];
        ListaRecientes[4][1] = PosicionActual[1];

    }

    int MovimientosRecientes(int y, int x){

        for(int i = 0; i < 5; i++){

            if(ListaRecientes[i][0] == y and ListaRecientes[i][1] == x){
                return ListaRecientes[i][2];
            }
        }
        return 0;

    }

    //Funcion para acceder a la cantidad de veces que se ha visitado la casilla a la que te llevará el movimiento actual
    int VecesVisitadas(int MovimientoActual, int PosicionActual[2]){

        switch(MovimientoActual){

            case 1: return MapaVisitadas[PosicionActual[0] - 1][PosicionActual[1]];
            case 2: return MapaVisitadas[PosicionActual[0]][PosicionActual[1] + 1];
            case 3: return MapaVisitadas[PosicionActual[0] + 1][PosicionActual[1]];
            case 4: return MapaVisitadas[PosicionActual[0]][PosicionActual[1] - 1];
            default: return -1;

        }

    }

    void BuscarObjetivo(int &ObjetivoTemporal, int &Oy, int &Ox, int &ContadorObjetivosColumna){

        ContadorObjetivosColumna = 0;
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 5; j++){

                if(MapaObjetivos[i][j] != 0 and MapaObjetivos[i][j] <= ObjetivoTemporal and MapaVisitadas[i][j] == 0){

                    Oy = i;
                    Ox = j;
                    ObjetivoTemporal = MapaObjetivos[i][j];
                    ContadorObjetivosColumna++;

                }
            }
        }
    }
    
};

struct Navegacion {

    float FactorPasados = 3;
    float FactorDistancia = 1;
    float FactorRecientes = 0;

    int Ox = 0;
    int Oy = 0;
    int ObjetivoActual = 26;
    int ContadorObjetivosColumna = 0;

    int DisponibilidadMovimiento[4] = {0, 0, 0, 0};
    float PrioridadMovimiento[4] = {0, 0, 0, 0};

    int MovimientoActual = 1;
    float MovientoTemporal = 0;
    int EstadoMovimiento = 1;

    void CalcularObjetivo(Mapa &mapa){

        ObjetivoActual = 26;
        mapa.BuscarObjetivo(ObjetivoActual, Oy, Ox, ContadorObjetivosColumna);

    }

    int Distancia(int y, int x){

        return(abs(y - Oy) + abs(x - Ox));

    }

    void CalcularPrioridad(int PosicionActual[2], Mapa &mapa){

        if(PosicionActual[0] != 0){
            PrioridadMovimiento[0] = Distancia(PosicionActual[0] - 1, PosicionActual[1])*FactorDistancia + mapa.VecesVisitadas(1, PosicionActual)*FactorPasados + mapa.MovimientosRecientes(PosicionActual[0] - 1, PosicionActual[1])*FactorRecientes;
        }
        else DisponibilidadMovimiento[0] = 1;

        if(PosicionActual[1] != 4){
            PrioridadMovimiento[1] = Distancia(PosicionActual[0], PosicionActual[1] + 1)*FactorDistancia + mapa.VecesVisitadas(2, PosicionActual)*FactorPasados + mapa.MovimientosRecientes(PosicionActual[0], PosicionActual[1] + 1)*FactorRecientes;
        }
        else DisponibilidadMovimiento[1] = 1;

        if(PosicionActual[0] != 4){
            PrioridadMovimiento[2] = Distancia(PosicionActual[0] + 1, PosicionActual[1])*FactorDistancia + mapa.VecesVisitadas(3, PosicionActual)*FactorPasados + mapa.MovimientosRecientes(PosicionActual[0] + 1, PosicionActual[1])*FactorRecientes;
        }
        else DisponibilidadMovimiento[2] = 1;

        if(PosicionActual[1] != 0){
            PrioridadMovimiento[3] = Distancia(PosicionActual[0], PosicionActual[1] - 1)*FactorDistancia + mapa.VecesVisitadas(4, PosicionActual)*FactorPasados + mapa.MovimientosRecientes(PosicionActual[0], PosicionActual[1] - 1)*FactorRecientes;
        }
        else DisponibilidadMovimiento[3] = 1;

    }

    void MejorMovimiento(int PosicionActual[2], SensoresUltrasonicos &sensor){

        MovientoTemporal = 200;

        for(int i = 0; i < 4; i++){

            if(PrioridadMovimiento[i] <= MovientoTemporal and DisponibilidadMovimiento[i] == 0){
                MovientoTemporal = PrioridadMovimiento[i];
                MovimientoActual = i + 1;
            }

        }

        if(sensor.SensorUS(MovimientoActual) == 1){
            DisponibilidadMovimiento[MovimientoActual - 1] = 1;
            EstadoMovimiento = 1;
        }

        else EstadoMovimiento = 0;

    }

    void ReiniciarMovimiento(){

        EstadoMovimiento = 1;

        for(int i = 0; i < 4; i++){
            DisponibilidadMovimiento[i] = 0;
        }
    }

};

struct Robot {

    int Final[2] = {4, 4};
    int PosicionActual[2] = {4, 0};

    int MovimientosTotales = 0;
    int ContadorObjetivos = 0;
    int EstadoObjetivos = 0;
    int EstadoLaberinto = 0;
    int ContadorColumna = 0;

    Mapa mapa;
    Navegacion navegacion;

    SensoresUltrasonicos Sensores;
    SensorColor SensorColor1;
    LedRGB Led = LedRGB(10, 11, 12);
    ControladorMotores Motores;
    IMU Brujula;

    // Pendiente corregir logica por deteccion de salida
    void Movimiento(int MovimientoActual){
        switch(MovimientoActual){
            case 1: 
            PosicionActual[0]--; 
            Motores.Movimiento1(30);
            break;

            case 2: 
            PosicionActual[1]++;
            Motores.Movimiento2(30);
            break;

            case 3: 
            PosicionActual[0]++;
            Motores.Movimiento3(30);
            break;

            case 4: 
            PosicionActual[1]--;
            Motores.Movimiento4(30);
            break;
        }

        MovimientosTotales++;
        mapa.MapaVisitadas[PosicionActual[0]][PosicionActual[1]] += 1;

        navegacion.ReiniciarMovimiento();

    }

    void MostrarColor(){
        int Color = SensorColor1.LeerColor();

        switch(Color){
            case 1: Led.Encender(92, 225, 230); break;
            case 2: Led.Encender(255, 222, 89); break;
            case 3: Led.Encender(255, 145, 77); break;
            case 4: Led.Encender(255, 102, 196); break;
            default: break;
        }
        
    }


};

Robot robot;

void setup(){

    Wire.begin();

    robot.Sensores.Inicializar();
    robot.SensorColor1.Inicializar();
    robot.Led.Inicializar();
    robot.Motores.Inicializar();

    robot.mapa.MapaVisitadas[robot.PosicionActual[0]][robot.PosicionActual[1]] = 1;

    robot.mapa.MapearObjetivos(robot.PosicionActual);
    robot.mapa.CondicionColumna++;

    robot.navegacion.CalcularObjetivo(robot.mapa);

}

void loop(){

    if(robot.EstadoLaberinto == 0){

        robot.navegacion.CalcularPrioridad(robot.PosicionActual, robot.mapa);

        while(robot.navegacion.EstadoMovimiento == 1){

            robot.navegacion.MejorMovimiento(robot.PosicionActual, robot.Sensores);

        }

        robot.Led.Apagar();

        robot.Movimiento(robot.navegacion.MovimientoActual);

        robot.MostrarColor();

        robot.mapa.ActualizarListaRecientes(robot.PosicionActual);

        if(robot.PosicionActual[0] == robot.navegacion.Oy and robot.PosicionActual[1] == robot.navegacion.Ox and robot.EstadoObjetivos == 0){

            for(int i = 0; i < 5; i++){

                if(robot.mapa.MapaVisitadas[i][robot.PosicionActual[1]] > 0){
                    robot.ContadorColumna++;
                }

            }

            if(robot.ContadorColumna == 5){

                robot.mapa.MapearObjetivos(robot.PosicionActual);

            }
            robot.ContadorColumna = 0;

            robot.navegacion.CalcularObjetivo(robot.mapa);

            if(robot.navegacion.ContadorObjetivosColumna == 0){

                robot.mapa.CondicionColumna++;
                robot.mapa.MapearObjetivos(robot.PosicionActual);
                robot.navegacion.CalcularObjetivo(robot.mapa);
                robot.mapa.CondicionColumna--;

            }

        }

        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 5; j++){
                if(robot.mapa.MapaVisitadas[i][j] > 0){
                    robot.ContadorObjetivos++;
                }
            }
        }

        if(robot.ContadorObjetivos == 25 and robot.EstadoObjetivos == 0){

            robot.EstadoObjetivos++;
            robot.navegacion.Ox = robot.Final[1];
            robot.navegacion.Oy = robot.Final[0];

            if(robot.PosicionActual[0] == robot.navegacion.Oy and robot.PosicionActual[1] == robot.navegacion.Ox){
                robot.EstadoLaberinto++;
                robot.EstadoObjetivos--;
            }

        }

        robot.ContadorObjetivos = 0;

        if(robot.EstadoObjetivos > 0){

            if(robot.PosicionActual[0] == robot.navegacion.Oy and robot.PosicionActual[1] == robot.navegacion.Ox){
                robot.EstadoLaberinto++;
            }

        }

    }

}
