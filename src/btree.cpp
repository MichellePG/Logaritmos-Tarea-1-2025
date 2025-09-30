#include "btree.hpp"

BTree::BTree() {
    // La raíz parte como una hoja vacia
    Nodo raiz = crear_nodo(false);
    disco.push_sin_contar(raiz);
}

BTree::ResultadoSplit BTree::partir_nodo(const Nodo& lleno) {
    ResultadoSplit R;
    int mid = B / 2;

    R.izq = crear_nodo(lleno.es_interno == 1);
    R.der = crear_nodo(lleno.es_interno == 1);

    // Izquierda
    R.izq.k = mid;
    for (int i = 0; i < mid; i++) {
        R.izq.pares[i] = lleno.pares[i];
    }
    R.izq.es_interno = lleno.es_interno;

    // Mediana
    R.mediana = lleno.pares[mid];

    // Derecha
    R.der.k = B - (mid + 1);
    for (int i = 0; i < R.der.k; i++) {
        R.der.pares[i] = lleno.pares[mid + 1 + i];
    }
    R.der.es_interno = lleno.es_interno;

    // Si es interno, repartir punteros en hijos
    if (lleno.es_interno) {
        // Hijos de la izquierda
        for (int i = 0; i <= mid; i++) {
            R.izq.hijos[i] = lleno.hijos[i];
        }
        // Hijos de la derecha
        for (int i = 0; i <= R.der.k; i++) {
            R.der.hijos[i] = lleno.hijos[mid + 1 + i];
        }
    }
    return R;
}

void BTree::insertar(int32_t clave, float valor) {
    Nodo raiz = disco.obtener(0);

    if (raiz.k < B) {
        // Raíz no llena
        if (!raiz.es_interno) {
            insertar_ordenado(raiz, clave, valor);
            disco.fijar(0, raiz);
        } else {
            Nodo arriba = insertar_en_interno(0, raiz, clave, valor);
            disco.fijar(0, arriba);
        }
        return;
    }

    // Partir y crear nueva raíz interna
    ResultadoSplit S = partir_nodo(raiz);
    size_t idxI = disco.anexar(S.izq);
    size_t idxD = disco.anexar(S.der);

    Nodo nueva = crear_nodo(true);
    nueva.k = 1;
    nueva.pares[0] = S.mediana;
    nueva.hijos[0] = (int)idxI;
    nueva.hijos[1] = (int)idxD;

    // Insertar en el hijo correcto
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

void BTree::serializar(const std::string& ruta) {
    FileStorage fs(ruta);
    fs.escribir_todos(disco.crudo());
}

std::vector<Par> BTree::consulta_rango(const std::string& ruta, int32_t L, int32_t U, IOStats* out_io) {
    FileStorage fs(ruta);
    std::vector<Par> ans;

    // Pila de índices de nodos a visitar
    std::vector<size_t> pila;
    pila.push_back(0);

    while (!pila.empty()) {
        size_t idx = pila.back();
        pila.pop_back();

        Nodo cur = fs.leer_en(idx);

        if (!cur.es_interno) {
            // Hoja
            for (int i = 0; i < cur.k; i++) {
                int32_t c = cur.pares[i].clave;
                if (c < L) continue;
                if (c > U) break;
                ans.push_back(cur.pares[i]);
            }
        } else {
            // Nodo Interno
            for (int i = 0; i < cur.k; i++) {
                int32_t c = cur.pares[i].clave;
                if (c >= L && c <= U) ans.push_back(cur.pares[i]);
            }

            // Determinar hijos a visitar; para no visitar todos, tomamos una ventana acotada por L y U
            int i_low = 0;
            while (i_low < cur.k && cur.pares[i_low].clave < L) i_low++;

            int i_high = cur.k - 1;
            while (i_high >= 0 && cur.pares[i_high].clave > U) i_high--;

            int desde = (i_low > 0 ? i_low - 1 : 0);
            int hasta = (i_high + 1 < cur.k ? i_high + 1 : cur.k);

            for (int i = desde; i <= hasta; i++) {
                if (cur.hijos[i] != -1) pila.push_back((size_t)cur.hijos[i]);
            }
        }
    }

    if (out_io) *out_io = fs.stats;
    return ans;
}

Nodo BTree::insertar_en_interno(size_t /*idx*/, Nodo actual, int32_t clave, float valor) {
    int i = indice_hijo(actual, clave);
    int hijo_idx = actual.hijos[i];
    Nodo hijo = disco.obtener(hijo_idx);

    if (hijo.k == B) {
        // Si el hijo está lleno, dividirlo antes de bajar
        ResultadoSplit S = partir_nodo(hijo);
        size_t idxI = hijo_idx;
        size_t idxD = disco.anexar(S.der);
        disco.fijar(idxI, S.izq);

        // Subir la mediana al actual, corriendo hijos hacia la derecha
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
        // Bajar a hoja
        insertar_ordenado(hijo, clave, valor);
        disco.fijar(hijo_idx, hijo);
        return actual;
    }

    // Bajar a interno
    hijo = insertar_en_interno(hijo_idx, hijo, clave, valor);
    disco.fijar(hijo_idx, hijo);
    return actual;
}