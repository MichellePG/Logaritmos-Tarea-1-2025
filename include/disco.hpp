#ifndef DISCO_HPP
#define DISCO_HPP

#include "nodo.hpp"
#include <vector>
#include <stdexcept>

// Disco simulado: almacena nodos en un vector
// Cuenta lecturas y escrituras
class DiscoSimulado {
public:
    DiscoSimulado() : lecturas(0), escrituras(0) {}

    Nodo leer(int pos); // Lee el nodo en pos (1 lectura)
    void escribir(int pos, const Nodo& nodo); // Escribe 'nodo' en pos (1 escritura)
    int append(const Nodo& nodo); // Agrega 'nodo' al final (1 escritura) y retorna su índice
    int size() const { // Número de nodos almacenados
        return (int)nodos.size(); 
    }
    const Nodo& at(int pos) const { 
        return nodos.at(pos); 
    }

    long long lecturas;
    long long escrituras;

private:
    std::vector<Nodo> nodos; // almacenamiento en RAM
};

#endif
