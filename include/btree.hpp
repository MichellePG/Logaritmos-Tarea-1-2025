#ifndef BTREE_HPP
#define BTREE_HPP

#include "disco.hpp"
#include <vector>

// BTree (árbol B clásico)
// En hojas se almacenan pares (llave, valor)
// En internos se usan solo las llaves

class BTree {
public:
    explicit BTree(DiscoSimulado* d);

    // --- Búsqueda --- 
    // Retorna pares (llave, valor) encontrados en hojas.
    std::vector<Par> buscarRango(int l, int u) const;

    int raizIndex() const {
        return raiz;
    }
    void setRaiz(int idx) {
        raiz = idx;
    }

    // --- Inserción --- (TO DO)

    // --- Split --- (TO DO)


private:
    DiscoSimulado* disco {nullptr};
    int raiz {-1};

    // --- Búsqueda --- 
    void buscarRangoRec(int pos, int l, int u, std::vector<Par>& out) const;

    // Dado un nodo interno n y una llave x, retorna el índice de hijo por donde bajar.
    static int hijoPara(const Nodo& n, int x);

    // --- Inserción --- (TO DO)

    // --- Split --- (TO DO)

};

#endif
