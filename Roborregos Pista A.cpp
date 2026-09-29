#include <iostream>
using namespace std;

//Simulacion
struct Sensor {

    //Mapa que guarda la posicion de cada pared horizontal (Simulacion)
    int ParedesX[6][5] =      
    {{1, 1, 1, 1, 1},
    {0, 1, 0, 1, 0},
    {1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 1, 0, 0},
    {1, 1, 1, 1, 1}};
    
    //Mapa que guarda la posicion de cada pared vertical (Simulacion)
    int ParedesY[5][6] =   
    {{1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 1}};

    //Funcion que simula el sensor ultrasonico, accediendo a la matriz con las paredes guardadas
    int SensorParedes(int MovimientoActual, int PosicionActual[2]){

        switch(MovimientoActual){

            case 1:

                if(ParedesX[PosicionActual[0]][PosicionActual[1]] == 1){
                return 1;
            }
            else return 0;

        case 2:

            if(ParedesY[PosicionActual[0]][PosicionActual[1] + 1] == 1){
                //cout << "Habia una pared en " << PosicionActual[0] << ", " << PosicionActual[1] + 1 << endl;
                //cout << ParedesY[PosicionActual[0]][PosicionActual[1] + 1] << endl;
                return 1;
            }
            else return 0;
        
        case 3:

            if(ParedesX[PosicionActual[0] + 1][PosicionActual[1]] == 1){
                return 1;
            }
            else return 0;
        
        case 4:

            if(ParedesY[PosicionActual[0]][PosicionActual[1]] == 1){
                return 1;
            }
            else return 0;

        default:
            return -1;
    }

    
    
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
        cout << "Mi nuevo objetivo es " << Oy << ", " << Ox << endl;

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

    void MejorMovimiento(int PosicionActual[2], Sensor sensor){

        MovientoTemporal = 200;

        for(int i = 0; i < 4; i++){

            if(PrioridadMovimiento[i] <= MovientoTemporal and DisponibilidadMovimiento[i] == 0){
                MovientoTemporal = PrioridadMovimiento[i];
                MovimientoActual = i + 1;
            }

        }

        if(sensor.SensorParedes(MovimientoActual, PosicionActual) == 1){
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
    Sensor sensor;
    Navegacion navegacion;

    void Movimiento(int MovimientoActual){
        switch(MovimientoActual){
            case 1: PosicionActual[0]--; break;
            case 2: PosicionActual[1]++; break;
            case 3: PosicionActual[0]++; break;
            case 4: PosicionActual[1]--; break;
        }

        MovimientosTotales++;
        mapa.MapaVisitadas[PosicionActual[0]][PosicionActual[1]] += 1;

        navegacion.ReiniciarMovimiento();

        cout << "Me movi hacia " << MovimientoActual << endl;

    }


};

int main(){

    Sensor sensor;
    Robot robot;
    robot.mapa.MapaVisitadas[robot.PosicionActual[0]][robot.PosicionActual[1]] = 1;

    robot.mapa.MapearObjetivos(robot.PosicionActual);
    robot.mapa.CondicionColumna++;

    robot.navegacion.CalcularObjetivo(robot.mapa);

    while(robot.EstadoLaberinto == 0){

        robot.navegacion.CalcularPrioridad(robot.PosicionActual, robot.mapa);

        while(robot.navegacion.EstadoMovimiento == 1){

            robot.navegacion.MejorMovimiento(robot.PosicionActual, sensor);

        }

        robot.Movimiento(robot.navegacion.MovimientoActual);
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

    cout << "El total de movimientos fue de " << robot.MovimientosTotales << endl;
    return 0;
}

