#include "btree.hpp"
#include <algorithm>

// BTree (árbol B clásico)
// En hojas se almacenan pares (llave, valor)
// En internos se usan solo las llaves
BTree::BTree(DiscoSimulado* d) : disco(d), raiz(-1) {}

// --- Auxiliar ---
// Retorna el índice de hijo por donde bajar en un nodo interno n para la llave x
int BTree::hijoPara(const Nodo& n, int x) {
    int i = 0;
    while (i < n.k && x > n.llaves_valores[i].llave) ++i;
    return i;
}

// --- Búsqueda ---
// Agrega a 'out' los pares en [l,u] del subárbol con raíz 'pos'
void BTree::buscarRangoRec(int pos, int l, int u, std::vector<Par>& out) const {
    Nodo n = disco->leer(pos);

    if (n.es_interno == 0) {
        // Hoja: devolver solo pares en [l,u]
        for (int i = 0; i < n.k; ++i) {
            int key = n.llaves_valores[i].llave;
            if (key >= l && key <= u) out.push_back(n.llaves_valores[i]);
        }
        return;
    }

    if (n.k == 0) {
        if (n.hijos[0] != -1) buscarRangoRec(n.hijos[0], l, u, out);
        return;
    }

    int iL = 0; while (iL < n.k && l > n.llaves_valores[iL].llave) ++iL;
    int iU = 0; while (iU < n.k && u >= n.llaves_valores[iU].llave) ++iU;

    for (int ci = iL; ci <= iU; ++ci) {
        int h = n.hijos[ci];
        if (h != -1) buscarRangoRec(h, l, u, out);
    }
}

// Retorna los pares en [l,u] ordenados por llave
std::vector<Par> BTree::buscarRango(int l, int u) const {
    std::vector<Par> out;
    if (raiz != -1) buscarRangoRec(raiz, l, u, out);
    return out;
}
