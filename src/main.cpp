#include <iostream>
#include "nodo.hpp"

int main() {
    // Nodo de ejemplo/prueba
    Nodo n;
    n.llaves_valores[0] = {10, 20.5};
    n.llaves_valores[1] = {30, 25.0};
    n.k = 2;

    std::cout << "Nodo creado con " << n.k << " pares.\n";
    for (int i = 0; i < n.k; i++) {
        std::cout << "  " << n.llaves_valores[i].llave
                  << " -> " << n.llaves_valores[i].valor << "\n";
    }

    return 0;
}
