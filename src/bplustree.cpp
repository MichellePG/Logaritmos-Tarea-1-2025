#include "bplustree.hpp"

// BTree+
// Valores solo en hojas, hojas enlazadas via 'siguiente'

BPlusTree::BPlusTree(DiscoSimulado* d) : disco(d), raiz(-1) {}

// --- Auxiliar ---
// Retorna el índice de la hoja donde debería caer x.
int BPlusTree::bajarAHojaPara(int x) const {
    if (raiz == -1) return -1;
    int pos = raiz;
    while (true) {
        Nodo n = disco->leer(pos);
        if (n.es_interno == 0) return pos;
        int i = 0;
        while (i < n.k && x > n.llaves_valores[i].llave) ++i;
        if (n.hijos[i] == -1) return pos;    // sin hijo válido, retornar el último alcanzado
        pos = n.hijos[i];
    }
}

// --- Búsqueda ---
// Retorna pares (llave,valor) con l <= llave <= u.
std::vector<Par> BPlusTree::buscarRango(int l, int u) const {
    std::vector<Par> out;
    if (raiz == -1) return out;

    int hoja = bajarAHojaPara(l);
    if (hoja == -1) return out;

    // Recorre hojas enlazadas desde 'hoja' hacia la derecha.
    int pos = hoja;
    while (pos != -1) {
        Nodo n = disco->leer(pos);
        if (n.es_interno != 0) break; 

        for (int i = 0; i < n.k; ++i) {
            const Par& p = n.llaves_valores[i];
            if (p.llave > u) return out;   
            if (p.llave >= l) out.push_back(p);
        }
        pos = n.siguiente;
    }
    return out;
}

// --- Inserción --- (TO DO)

// --- Split --- (TO DO)
