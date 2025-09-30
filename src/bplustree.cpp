#include "bplustree.hpp"

BPlusTree::BPlusTree() {
    // La raíz parte como HOJA vacía.
    Nodo raiz = crear_nodo(false);
    disco.push_sin_contar(raiz);
}

BPlusTree::ResultadoSplit BPlusTree::partir_nodo(const Nodo& lleno) {
    ResultadoSplit R;
    int mid = B / 2;

    R.izq = crear_nodo(lleno.es_interno == 1);
    R.der = crear_nodo(lleno.es_interno == 1);

    if (lleno.es_interno) {
        R.izq.k = mid;
        for (int i = 0; i < mid; i++) R.izq.pares[i] = lleno.pares[i];
        R.izq.es_interno = 1;
        R.der.es_interno = 1;

        R.mediana = lleno.pares[mid];

        R.der.k = B - (mid + 1);
        for (int i = 0; i < R.der.k; i++) R.der.pares[i] = lleno.pares[mid + 1 + i];

        for (int i = 0; i <= mid; i++) R.izq.hijos[i] = lleno.hijos[i];
        for (int i = 0; i <= R.der.k; i++) R.der.hijos[i] = lleno.hijos[mid + 1 + i];
    } else {
        // El mediano también queda en la izquierda
        R.izq.k = mid + 1;
        for (int i = 0; i <= mid; i++) R.izq.pares[i] = lleno.pares[i];
        R.mediana = lleno.pares[mid];

        R.der.k = B - (mid + 1);
        for (int i = 0; i < R.der.k; i++) R.der.pares[i] = lleno.pares[mid + 1 + i];
    }
    return R;
}

void BPlusTree::insertar(int32_t clave, float valor) {
    Nodo raiz = disco.obtener(0);

    if (raiz.k < B) {
        if (!raiz.es_interno) {
            insertar_ordenado(raiz, clave, valor);
            disco.fijar(0, raiz);
        } else {
            Nodo arriba = insertar_en_interno(0, raiz, clave, valor);
            disco.fijar(0, arriba);
        }
        return;
    }

    ResultadoSplit S = partir_nodo(raiz);
    size_t idxI = disco.anexar(S.izq);
    size_t idxD = disco.anexar(S.der);

    if (!raiz.es_interno) {
        // Si partimos en una hoja raíz, encadenar nodo izquierdo al derecho
        Nodo I = disco.obtener(idxI);
        Nodo D = disco.obtener(idxD);
        I.siguiente = (int)idxD;
        disco.fijar(idxI, I);
        disco.fijar(idxD, D);
    }

    Nodo nueva = crear_nodo(true);
    nueva.k = 1;
    nueva.pares[0] = S.mediana;
    nueva.hijos[0] = (int)idxI;
    nueva.hijos[1] = (int)idxD;

    if (clave <= S.mediana.clave) {
        Nodo I = disco.obtener(idxI);
        if (!I.es_interno) insertar_ordenado(I, clave, valor);
        else               I = insertar_en_interno(idxI, I, clave, valor);
        disco.fijar(idxI, I);
    } else {
        Nodo D = disco.obtener(idxD);
        if (!D.es_interno) insertar_ordenado(D, clave, valor);
        else               D = insertar_en_interno(idxD, D, clave, valor);
        disco.fijar(idxD, D);
    }
    disco.fijar(0, nueva);
}

void BPlusTree::serializar(const std::string& ruta) {
    FileStorage fs(ruta);
    fs.escribir_todos(disco.crudo());
}

std::vector<Par> BPlusTree::consulta_rango(const std::string& ruta, int32_t L, int32_t U, IOStats* out_io) {
    FileStorage fs(ruta);
    std::vector<Par> ans;
    size_t idx = 0; // raíz en 0

    while (true) {
        Nodo cur = fs.leer_en(idx);

        if (!cur.es_interno) {
            // Avanzar hasta que sea mayor o igual a L
            int i = 0;
            while (i < cur.k && cur.pares[i].clave < L) i++;

            // Agregar pares hasta superar U
            for (; i < cur.k; ++i) {
                int32_t c = cur.pares[i].clave;
                if (c > U) {
                    if (out_io) *out_io = fs.stats;
                    return ans;
                }
                ans.push_back(cur.pares[i]);
            }

            // Seguir por hojas encadenadas
            while (cur.siguiente != -1) {
                idx = (size_t)cur.siguiente;
                cur = fs.leer_en(idx);

                for (int j = 0; j < cur.k; j++) {
                    int32_t c = cur.pares[j].clave;
                    if (c > U) {
                        if (out_io) *out_io = fs.stats;
                        return ans;
                    }
                    if (c >= L) ans.push_back(cur.pares[j]);
                }
            }

            if (out_io) *out_io = fs.stats;
            return ans;
        } else {
            // Decidir hijo usando L
            int i = indice_hijo(cur, L);
            idx = (size_t)cur.hijos[i];
        }
    }
}

Nodo BPlusTree::insertar_en_interno(size_t /*idx*/, Nodo actual, int32_t clave, float valor) {
    int i = indice_hijo(actual, clave);
    int hijo_idx = actual.hijos[i];
    Nodo hijo = disco.obtener(hijo_idx);

    if (hijo.k == B) {
        // Partir hijo antes de bajar
        ResultadoSplit S = partir_nodo(hijo);
        size_t idxI = hijo_idx;
        size_t idxD = disco.anexar(S.der);

        if (!hijo.es_interno) {
            // Si partimos hoja, encadenar nodo izquierdo al derecho
            Nodo I = S.izq;
            Nodo D = disco.obtener(idxD);
            I.siguiente = (int)idxD;
            disco.fijar(idxI, I);
            disco.fijar(idxD, D);
        } else {
            disco.fijar(idxI, S.izq);
        }

        // Subir mediana al actual
        for (int j = actual.k; j > i; --j) {
            actual.pares[j] = actual.pares[j - 1];
            actual.hijos[j + 1] = actual.hijos[j];
        }
        actual.pares[i] = S.mediana;
        actual.k++;
        actual.hijos[i] = (int)idxI;
        actual.hijos[i + 1] = (int)idxD;

        // Elegir lado y continuar
        if (clave <= S.mediana.clave) {
            Nodo I = disco.obtener(idxI);
            if (!I.es_interno) insertar_ordenado(I, clave, valor);
            else               I = insertar_en_interno(idxI, I, clave, valor);
            disco.fijar(idxI, I);
        } else {
            Nodo D = disco.obtener(idxD);
            if (!D.es_interno) insertar_ordenado(D, clave, valor);
            else               D = insertar_en_interno(idxD, D, clave, valor);
            disco.fijar(idxD, D);
        }
        return actual;
    }

    // Hijo no lleno
    if (!hijo.es_interno) {
        insertar_ordenado(hijo, clave, valor);
        disco.fijar(hijo_idx, hijo);
        return actual;
    }

    hijo = insertar_en_interno(hijo_idx, hijo, clave, valor);
    disco.fijar(hijo_idx, hijo);
    return actual;
}