#include "disco.hpp"
#include <string>

// Implementación de disco

Nodo DiscoSimulado::leer(int pos) {
    if (pos < 0 || pos >= static_cast<int>(nodos.size())) {
        throw std::out_of_range(
            "leer: posicion invalida (" + std::to_string(pos) + "), size=" + std::to_string(nodos.size())
        );
    }
    lecturas++;                 // cuenta 1 lectura
    return nodos[pos];          // se devuelve COPIA
}

void DiscoSimulado::escribir(int pos, const Nodo& nodo) {
    if (pos < 0 || pos >= static_cast<int>(nodos.size())) {
        throw std::out_of_range(
            "escribir: posicion invalida (" + std::to_string(pos) + "), size=" + std::to_string(nodos.size())
        );
    }
    escrituras++;               // cuenta 1 escritura
    nodos[pos] = nodo;          // se soobreescribe “bloque”
}

int DiscoSimulado::append(const Nodo& nodo) {
    escrituras++;               // 1 escritura
    nodos.push_back(nodo);      // nuevo bloque al final
    return static_cast<int>(nodos.size()) - 1;
}
