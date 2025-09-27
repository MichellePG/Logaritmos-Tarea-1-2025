#ifndef BPLUSTREE_HPP
#define BPLUSTREE_HPP

#include "disco.hpp"
#include <vector>

// BTree+
// Valores solo en hojas, hojas enlazadas via 'siguiente'

class BPlusTree {
public:
    explicit BPlusTree(DiscoSimulado* d);

    // --- Búsqueda ---
    // Retorna pares (llave,valor) con l <= llave <= u
    std::vector<Par> buscarRango(int l, int u) const;

    int  raizIndex() const { return raiz; }
    void setRaiz(int idx)  { raiz = idx; }

    // --- Inserción --- (TO DO)
    
private:
    DiscoSimulado* disco {nullptr};
    int raiz {-1};

    // Retorna el índice de la hoja donde debería estar x
    int bajarAHojaPara(int x) const;

    // --- Split --- (TO DO)
};

#endif
