#ifndef NODO_HPP
#define NODO_HPP

#include <array>

#ifndef B
#define B 4 // B=4 para debug inicial.
#endif

// Par (llave, valor)
struct Par {
    int llave;
    float valor;
};

// Nodo de árbol B / B+
struct Nodo {
    int es_interno;                      // indica si el nodo es interno o un nodo externo (hoja)
    int k;                               // guarda la cantidad de pares llave-valor actualmente contenidas en el nodo
    std::array<Par, B> llaves_valores;   // pares llave-valor
    std::array<int, B + 1> hijos;        // posiciones de hijos (índices en disco)
    int siguiente;                       // solo para B+:  Guarda la posición en disco de la hoja siguiente a la actual

    Nodo() {
        es_interno = 0;
        k = 0;
        hijos.fill(-1);
        siguiente = -1;
    }
};

#endif
