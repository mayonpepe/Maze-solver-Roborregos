#include <iostream>
using namespace std;

//Coordenadas de inicio
int Inicio[2] = {4, 0};
//Coordenadas de final
int Final[2] =  {4, 4};

//Coordenadas actuales
int PosicionActual[2] = {4 , 0};

//Disponibilidad de cada movimiento, depende si es posible o no
int DisponibilidadMovimiento[4] = {0, 0, 0, 0};

//Movimiento actual por probar
int MovimientoActual = 1;

//Coordenada x del objetivo
int Ox = 0;
//Coordenada y del objetivo
int Oy = 0;

//Etiqueta del objetivo actual
int ObjetivoActual = 26;

//Movimientos hechos hasta el momento
int MovimientosTotales = 0;

//Factor para aumentar el peso de las veces pasadas en el calculo de la prioridad
float FactorPasados = 2.5;

//Factor para aumentar el peso de la distancia en el calculo de la prioridad
float FactorDistancia = 1;

float FactorX = 0;

//Mapa que guarda la etiqueta del objetivo de cada casilla
int Mapa[5][5] =       {{0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0}};

//Mapa que guarda la cantidad de veces que se ha pasado por cada casilla
float MapaRevisado[5][5]={{0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0}};

//Mapa que guarda la posicion de los obstaculos
int Obstaculos[5][5] = {{0, 0, 0, 1, 0},
                        {0, 0, 2, 0, 0},
                        {0, 0, 0, 0, 4},
                        {0, 0, 0, 0, 4},
                        {0, 1, 0, 0, 0}};

//Mapa que guarda la posicion de cada color (Simulacion)
int Colores[5][5] =    {{3, 0, 0, 0, 0},
                        {0, 2, 0, 0, 0},
                        {1, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 0, 4, 0, 0}};

//Mapa que guarda la posicion de cada pared vertical (Simulacion)
int ParedesX[6][5] =   {{1, 1, 1, 1, 1},
                        {1, 0, 0, 0, 0},
                        {0, 1, 0, 0, 0},
                        {0, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0},
                        {1, 1, 1, 1, 1}};
    
//Mapa que guarda la posicion de cada pared vertical (Simulacion)
int ParedesY[5][6] =   {{1, 0, 0, 0, 0, 1},
                        {1, 0, 1, 1, 1, 1},
                        {1, 1, 0, 1, 1, 1},
                        {1, 1, 0, 1, 1, 1},
                        {1, 0, 0, 0, 1, 1}};

//Condicion que sirve para mover la columna a mapear en casos especiales (Inicio y columna sin objetivos disponibles)
int CondicionColumna = 0;

//Considerar inicio a la derecha

//Funcion que mapea Mapa[][] con las etiquetas de objetivo de cada casilla, lo que sirve como una guia para el robot que
// le dice en que orden eligirá sus objetivos, lo hace columna a columna cuando termina todos los objetivos de una
void MapeadoObjetivos(){

    if(PosicionActual[0] <= 2){

        for(int i = 0; i <= 4; i++){
            Mapa[i][PosicionActual[1] + CondicionColumna] = (i + 1) + 5 * (PosicionActual[1] + CondicionColumna);
        }

    }
    else{
        for(int i = 4; i >= 0; i--){
            Mapa[i][PosicionActual[1] + CondicionColumna] = (5 - i) + 5 * (PosicionActual[1] + CondicionColumna);
        }

    }

    /*for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            cout << Mapa[i][j] << ", ";
        }
        cout << endl;

    }*/
}
//Contador que registra cuantas veces se cambio el objetivo (si es 0 es un caso especial para MapeadoObjetivos())
int ContadorObjetivosColumna = 0;
//Variable temporal que sirve para elegir la etiqueta de objetivo con menor valor
int ObjetivoTemporal = 0;

//Define el objetivo actual, eligiendo la casilla con con menor etiqueta de objetivo y verificando que el robot no haya pasado por ahi
void CalcularObjetivo(){

    ObjetivoTemporal = 5 * (PosicionActual[1] + 1 + CondicionColumna);
    ContadorObjetivosColumna = 0;
    for(int i = 0; i < 5; i++){

        for(int j = 0; j < 5; j++){
            if(Mapa[i][j] != 0 and Mapa[i][j] <= ObjetivoTemporal and MapaRevisado[i][j] == 0){
                //cout << "Mi ObjetivoTemporal era " << ObjetivoTemporal << " y lo cambie a " << Mapa[i][j] << " en " << i << ", " << j << " con valor de Maparevisado de " << Maparevisado[i][j] << endl;
                Oy = i;
                Ox = j;
                ObjetivoTemporal = Mapa[i][j];
                ContadorObjetivosColumna++;
            }
        }
    }
    ObjetivoActual = ObjetivoTemporal;
    cout << "Mi nuevo objetivo es " << ObjetivoActual << " y esta en " << Oy << ", " << Ox << endl;
}

//Funcion que simula el sensor ultrasonico, accediendo a la matriz con las paredes guardadas
int Sensor(int n){
    switch(n){

        case 1:
            if(ParedesX[PosicionActual[0]][PosicionActual[1]] == 1){
                return 1;
            }
            else return 0;

        case 2:

            if(ParedesY[PosicionActual[0]][PosicionActual[1] + 1] == 1){
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

//Funcion que calcula la distancia entre la casilla a la que lleva el movimiento actual y el objetivo actual
int Distancia(int a, int b){

    //cout << "La distancia de " << a << ", " << b << " a " << Oy << ", " << Ox << " es " << abs(a - Oy) + abs(b - Ox) << endl;
    return(abs(a - Oy) + abs(b - Ox));

}

//Funcion para acceder a la cantidad de veces que has pasado por la casilla a la que te llevará el movimiento actual
int VecesPasadas(int n){

    switch(n){

        case 1:

            return MapaRevisado[PosicionActual[0] - 1][PosicionActual[1]];

        case 2:

            return MapaRevisado[PosicionActual[0]][PosicionActual[1] + 1];
        
        case 3:

            return MapaRevisado[PosicionActual[0] + 1][PosicionActual[1]];

        case 4:

            return MapaRevisado[PosicionActual[0]][PosicionActual[1] - 1];

        default:
            return -1;
    }
}

//Prioridada de la casilla a la que lleva cada movimiento
float PrioridadMovimiento[4] = {0, 0, 0, 0};

//Funcion que define los valores de la lista PrioridadMovimiento[], con base a Distancia() y VecesPasadas()
// con un peso definido por FactorDistancia y FactorPasados respectivamente 
void CalcularPrioridad(){

    if(PosicionActual[0] != 0){
        PrioridadMovimiento[0] = Distancia(PosicionActual[0] - 1, PosicionActual[1])*FactorDistancia + VecesPasadas(1)*FactorPasados;
    }
    else DisponibilidadMovimiento[0] = 1;
    
    if(PosicionActual[1] != 4){
        PrioridadMovimiento[1] = Distancia(PosicionActual[0], PosicionActual[1] + 1)*FactorDistancia + VecesPasadas(2)*FactorPasados;
    }
    else DisponibilidadMovimiento[1] = 1;

    if(PosicionActual[0] != 4){
        PrioridadMovimiento[2] = Distancia(PosicionActual[0] + 1, PosicionActual[1])*FactorDistancia + VecesPasadas(3)*FactorPasados;
    }
    else DisponibilidadMovimiento[2] = 1;

    if(PosicionActual[1] != 0){
        PrioridadMovimiento[3] = Distancia(PosicionActual[0], PosicionActual[1] - 1)*FactorDistancia + VecesPasadas(4)*FactorPasados;
    }
    else DisponibilidadMovimiento[3] = 1;

}

//Variable temporal que sirve para elegir el elemento de PrioridadMovimiento[] con menor valor
float MovientoTemporal = 0;

//Variable que indica si el movimiento probado fue posible (si habia pared) en caso de que haya pared, 
// MejorMovimiento() se ejecuta otra vez y selecciona un nuevo objetivo
int EstadoMovimiento = 1;

//Funcion que elige el mejor movimiento eligiendo el menor valor de PrioridadMovimiento[] y comprobando que el movimiento este 
// disponible verificando DisponibilidadMovimiento[]
void MejorMovimiento(){

    MovientoTemporal = 999999;

    for(int i = 0; i < 4; i++){
        
        if(PrioridadMovimiento[i] <= MovientoTemporal and DisponibilidadMovimiento[i] == 0){
            
            //cout << "El mejor movimiento seria " << i + 1 << " porque " << Mov[i] << " < " << Movi << endl;
            MovientoTemporal = PrioridadMovimiento[i];
            MovimientoActual = i + 1;

        }
       /* else if(Movdis[i] == 0){
            cout << "El movimiento no se cambio a " << i + 1 << " porque " << Mov[i] << " > " << Movi << endl;
        }
        else cout << "El movimiento no se cambio a " << i + 1 << " porque Movdis[i] = " << Movdis[i] << endl;   
        */
        
        

    }

    if(Sensor(MovimientoActual) == 1){
        DisponibilidadMovimiento[MovimientoActual - 1] = 1;
        EstadoMovimiento = 1;
    }

    else EstadoMovimiento = 0;

}

//Funcion que ejecuta el movimiento, una vez que EstadoMovimiento == 0, lo que significa que el movimiento es posible
void Movimiento(int n){

    switch(n){

        case 1:

            PosicionActual[0]--;
            break;
        case 2:

            PosicionActual[1]++;
            break;
        case 3:

            PosicionActual[0]++;
            break;
        case 4:

            PosicionActual[1]--;
            break;
        
    }

    MovimientosTotales++;
    MapaRevisado[PosicionActual[0]][PosicionActual[1]] += 1;
    /*
    if(MapaRevisado[Posicion[0]][Posicion[1]] > 10){
        indicelab++;
    }*/

    EstadoMovimiento = 1;

    for(int i = 0; i < 4; i++){
        DisponibilidadMovimiento[i] = 0;
    }

    //cout << "Me movi a " << Posicion[0] << ", " << Posicion[1] << endl;
    cout << "Me movi hacia " << MovimientoActual << endl;
    //cout << "Ahora Maparevisado " << Posicion[0] << ", " << Posicion[1] << " tiene un valor de " << MapaRevisado[Posicion[0]][Posicion[1]] << endl;
}

void Calibracion(){

}



//Variable que te dice si ya lograste todos los objetivos
int EstadoObjetivos = 0;
//Variable que te dice si ya lograste todos los objetivos y llegaste a la salida
int EstadoLaberinto = 0;
//Contador que registra cuantos objetivos ya se lograron de una columna
int ContadorColumna = 0;

int main()
{
    MapaRevisado[PosicionActual[0]][PosicionActual[1]] = 1;

    MapeadoObjetivos();
    CondicionColumna++;
    CalcularObjetivo();

    while(EstadoLaberinto == 0){

        CalcularPrioridad();

        while(EstadoMovimiento == 1){
            
            MejorMovimiento();

        }

        Movimiento(MovimientoActual);

        

        if(PosicionActual[0] == Oy and PosicionActual[1] == Ox and EstadoObjetivos == 0){

            for(int i = 0; i < 5; i++){

                if(MapaRevisado[i][PosicionActual[1]] > 0){
                    ContadorColumna++;
                }

            }

            if(ContadorColumna == 5){

                MapeadoObjetivos();
                ContadorColumna = 0;

            }

            CalcularObjetivo();

            if(ContadorObjetivosColumna == 0){
                CondicionColumna++;
                MapeadoObjetivos();
                CalcularObjetivo();
                CondicionColumna--;

            }

        }

        if(MapaRevisado[0][4] > 0 and MapaRevisado[4][4] > 0 and EstadoObjetivos == 0){

            EstadoObjetivos++;
            Ox = Final[1];
            Oy = Final[0];

            if(PosicionActual[0] = Oy and PosicionActual[1] == Ox){
                EstadoLaberinto++;
                EstadoObjetivos--;
            }

        }

        if(EstadoObjetivos > 0){
            
            while(EstadoMovimiento == 1){
            
                MejorMovimiento();

            }

            Movimiento(MovimientoActual);

            if(PosicionActual[0] = Oy and PosicionActual[1] == Ox){
                EstadoLaberinto++;
            }

        }

    }
    cout << "El total de movimientos fue de " << MovimientosTotales << endl;
    return 0;
}