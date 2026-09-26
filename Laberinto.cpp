#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

struct Direccion {
    int dy;
    int dx;
    char dir;
};

void tallar_caminos(int y, int x, int filas, int columnas,
                    vector<vector<int>>& H, vector<vector<int>>& V,
                    vector<vector<bool>>& visitado, mt19937& rng) {

    visitado[y][x] = true;

    vector<Direccion> direcciones = {
        {-1, 0, 'N'},
        {1, 0, 'S'},
        {0, 1, 'E'},
        {0, -1, 'W'}
    };

    shuffle(direcciones.begin(), direcciones.end(), rng);

    for (const auto& d : direcciones) {
        int ny = y + d.dy;
        int nx = x + d.dx;

        if (ny >= 0 && ny < filas && nx >= 0 && nx < columnas && !visitado[ny][nx]) {
            if (d.dir == 'N') H[y][x] = 0;
            else if (d.dir == 'S') H[y + 1][x] = 0;
            else if (d.dir == 'E') V[y][x + 1] = 0;
            else if (d.dir == 'W') V[y][x] = 0;

            tallar_caminos(ny, nx, filas, columnas, H, V, visitado, rng);
        }
    }
}

// NUEVO: despues de generar el laberinto "perfecto" (un unico camino entre
// cualquier par de casillas), tira abajo buena parte de las paredes INTERNAS
// que sobraron, con cierta probabilidad. Quitar una pared solo agrega mas
// conexiones, nunca rompe la conectividad, asi que el laberinto sigue siendo
// 100% recorrible, solo que mucho mas abierto.
void reducir_paredes(vector<vector<int>>& H, vector<vector<int>>& V,
                      int filas, int columnas, double probabilidadQuitar, mt19937& rng) {
    uniform_real_distribution<double> azar(0.0, 1.0);

    // paredes horizontales internas (la fila 0 y la 'filas' son el borde exterior, no se tocan)
    for (int r = 1; r < filas; r++)
        for (int c = 0; c < columnas; c++)
            if (H[r][c] == 1 && azar(rng) < probabilidadQuitar)
                H[r][c] = 0;

    // paredes verticales internas (la columna 0 y 'columnas' son el borde exterior, no se tocan)
    for (int r = 0; r < filas; r++)
        for (int c = 1; c < columnas; c++)
            if (V[r][c] == 1 && azar(rng) < probabilidadQuitar)
                V[r][c] = 0;
}

void dibujar_laberinto(const vector<vector<int>>& H, const vector<vector<int>>& V, int filas, int columnas) {
    cout << "\n--- LABERINTO ---\n";
    for (int r = 0; r < filas; ++r) {
        string linea_h = "+";
        for (int c = 0; c < columnas; ++c) {
            if (H[r][c] == 1) linea_h += "---+";
            else linea_h += "   +";
        }
        cout << linea_h << "\n";

        string linea_v = "";
        for (int c = 0; c < columnas; ++c) {
            if (V[r][c] == 1) linea_v += "|   ";
            else linea_v += "    ";
        }
        if (V[r][columnas] == 1) linea_v += "|";
        else linea_v += " ";
        cout << linea_v << "\n";
    }

    string linea_h_fin = "+";
    for (int c = 0; c < columnas; ++c) {
        if (H[filas][c] == 1) linea_h_fin += "---+";
        else linea_h_fin += "   +";
    }
    cout << linea_h_fin << "\n";
}

void imprimir_matriz(const vector<vector<int>>& matriz) {
    for (size_t i = 0; i < matriz.size(); ++i) {
        if (i == 0) cout << "{{";
        else cout << " {";

        for (size_t j = 0; j < matriz[i].size(); ++j) {
            cout << matriz[i][j];
            if (j < matriz[i].size() - 1) cout << ", ";
        }

        cout << "}";
        if (i < matriz.size() - 1) cout << ",\n";
        else cout << "};\n";
    }
}

int main() {
    int filas = 5;
    int columnas = 5;

    vector<vector<int>> H(filas + 1, vector<int>(columnas, 1));
    vector<vector<int>> V(filas, vector<int>(columnas + 1, 1));
    vector<vector<bool>> visitado(filas, vector<bool>(columnas, false));

    random_device rd;
    mt19937 rng(rd());

    tallar_caminos(0, 0, filas, columnas, H, V, visitado, rng);

    // AQUI ESTA EL CAMBIO: quita ~65% de las paredes internas que sobraron
    reducir_paredes(H, V, filas, columnas, 0.25, rng);

    dibujar_laberinto(H, V, filas, columnas);

    cout << "\n--- MATRIZ HORIZONTAL (H) 6x5 ---\n";
    imprimir_matriz(H);

    cout << "\n--- MATRIZ VERTICAL (V) 5x6 ---\n";
    imprimir_matriz(V);

    return 0;
}