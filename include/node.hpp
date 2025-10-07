#pragma once
#include <cstdint>

// Capacidad de claves por nodo
static const int B = 340;
static const int MAX_HIJOS = B + 1;

// Estructura Par (clave, valor)
struct Par {
    int32_t clave;
    float valor;
};

// Nodo de 4096 bytes exactos
#pragma pack(push, 1)
struct Nodo {
    int32_t es_interno;
    int32_t k;
    Par pares[B];
    int32_t hijos[MAX_HIJOS];
    int32_t siguiente;
};
#pragma pack(pop)

// Verificar los tamaños en tiempo de compilación
static_assert(sizeof(Par) == 8, "Par debe medir 8 bytes");
static_assert(sizeof(Nodo) == 4096, "Nodo debe medir 4096 bytes");

// Crear un nodo nuevo vacío
inline Nodo crear_nodo(bool interno) {
    Nodo n;
    n.es_interno = interno ? 1 : 0;
    n.k = 0;
    n.siguiente = -1;
    for (int i = 0; i < MAX_HIJOS; i++) n.hijos[i] = -1;
    return n;
}

// Inserta un par en orden dentro de un nodo hoja
inline int insertar_ordenado(Nodo& nodo, int32_t clave, float valor) {
    int pos = 0;
    while (pos < nodo.k && nodo.pares[pos].clave < clave) pos++;
    for (int i = nodo.k; i > pos; --i) nodo.pares[i] = nodo.pares[i - 1];
    nodo.pares[pos] = Par{clave, valor};
    nodo.k++;
    return pos;
}

// Dado un nodo interno y una clave, retornar el índice del hijo por el que se debe descender
inline int indice_hijo(const Nodo& nodo, int32_t clave) {
    int i = 0;
    while (i < nodo.k && nodo.pares[i].clave < clave) i++;
    return i;
}