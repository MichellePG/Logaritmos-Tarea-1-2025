#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include "node.hpp"

// Contadores de operaciones de entrada/salida
struct IOStats {
    unsigned long long lecturas;
    unsigned long long escrituras;

    IOStats() : lecturas(0), escrituras(0) {}

    void reiniciar() { 
        lecturas = 0; escrituras = 0;
    }
};

// Simula el disco en RAM durante la construcción de arboles
struct DiskArray {
    std::vector<Nodo> v; // almacenamiento de nodos en RAM
    IOStats stats; // contadores de I/O

    DiskArray() { 
        v.reserve(1 << 16); 
    }

    void push_sin_contar(const Nodo& n);
    Nodo obtener(size_t i);
    void fijar(size_t i, const Nodo& n);
    size_t anexar(const Nodo& n);
    size_t cantidad() const; // total de nodos en RAM
    const std::vector<Nodo>& crudo() const;
};

// Clase para manejar el archivo binario de nodos
struct FileStorage {
    std::string ruta; // nombre del archivo binario
    IOStats stats; // contadores de I/O

    explicit FileStorage(const std::string& r) : ruta(r) {}

    void escribir_todos(const std::vector<Nodo>& nodos); // guardar todos los nodos
    Nodo leer_en(size_t i); // leer nodo en posición i
};