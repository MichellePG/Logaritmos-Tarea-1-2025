#pragma once
#include <vector>
#include <string>
#include "node.hpp"
#include "disk.hpp"

class BTree {
public:
    DiskArray disco; // almacenamiento en RAM

    BTree();

    // Resultado al dividir (split) un nodo lleno
    struct ResultadoSplit {
        Nodo izq;
        Nodo der;
        Par mediana;
    };

    static ResultadoSplit partir_nodo(const Nodo& lleno);

    // Inserta una clave en el árbol
    void insertar(int32_t clave, float valor);

    // Serializa el árbol completo a un archivo binario
    void serializar(const std::string& ruta);

    // Retorna todos los pares con clave entre [L, U]
    std::vector<Par> consulta_rango(const std::string& ruta, int32_t L, int32_t U, IOStats* out_io);

private:
    // Inserción en un nodo interno no lleno
    Nodo insertar_en_interno(size_t idx, Nodo actual, int32_t clave, float valor);
};