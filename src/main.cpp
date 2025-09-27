#include "nodo.hpp"
#include "disco.hpp"
#include "bplustree.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdio>


// --- Auxiliar ---
// Construye hojas a partir de 'pares' ordenados por llave
// Fija 'idx_primera_hoja' al índice de la primera hoja creada
static bool construir_hojas_desde_pares(DiscoSimulado& disco, const std::vector<Par>& pares, int& idx_primera_hoja) {
    if (pares.empty()) return false;

    idx_primera_hoja = -1;
    size_t i = 0;

    int  prev_idx   = -1;
    Nodo prev_hoja{};
    bool tengo_prev = false;

    while (i < pares.size()) {
        Nodo hoja{};
        hoja.es_interno = 0;
        hoja.k = 0;
        hoja.siguiente = -1;

        for (int j = 0; j < B && i < pares.size(); ++j, ++i)
            hoja.llaves_valores[hoja.k++] = pares[i];

        int idx = disco.append(hoja);
        if (idx_primera_hoja == -1) idx_primera_hoja = idx;

        if (tengo_prev) {
            prev_hoja.siguiente = idx;
            disco.escribir(prev_idx, prev_hoja); // no lee del disco
        }
        prev_idx   = idx;
        prev_hoja  = hoja;   // copia en RAM
        tengo_prev = true;
    }
    return true;
}

// --- Auxiliar ---
// Carga desde texto (sample). 
// Ordena por llave y construye hojas.
static bool cargar_sample_a_hojas(DiscoSimulado& disco,
                                  const std::string& path,
                                  int& idx_primera_hoja)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ERROR] No puede abrir " << path << ". Verificar la ruta.\n";
        return false;
    }

    std::vector<Par> pares;
    pares.reserve(1024);

    std::string line;
    size_t linea_num = 0;
    while (std::getline(file, line)) {
        linea_num++;

        auto notspace = [](int c){
            return !std::isspace(c); 
        };
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), notspace));
        line.erase(std::find_if(line.rbegin(), line.rend(), notspace).base(), line.end());
        if (line.empty()) continue;

        if (!std::isdigit((unsigned char)line[0]) && line[0] != '-') { // Ignorar líneas que no comienzan con dígito o '-'
            continue;
        }

        std::replace(line.begin(), line.end(), ',', '.'); // Reemplazar ',' por '.' en caso de que el float use coma decimal

        std::istringstream iss(line);
        long llaveL; double valorD;
        if (!(iss >> llaveL >> valorD)) {
            std::cerr << "[WARNING] Línea " << linea_num << " no se pudo parsear: \"" << line << "\"\n";
            continue;
        }
        Par p;
        p.llave = (int)llaveL;
        p.valor = (float)valorD;
        pares.push_back(p);
    }
    file.close();

    if (pares.empty()) {
        std::cerr << "[ERROR] El archivo " << path << " está vacío o no se pudo parsear.\n";
        return false;
    }

    std::sort(pares.begin(), pares.end(), [](const Par& a, const Par& b){
        return a.llave < b.llave; 
    });

    return construir_hojas_desde_pares(disco, pares, idx_primera_hoja);
}

// --- Auxiliar ---
// Carga desde binario (pares int-float)
// Ordena por llave y construye hojas   
static bool cargar_bin_a_hojas(DiscoSimulado& disco, const std::string& path_bin, int& idx_primera_hoja, size_t max_pares = 1'000'000) {
    FILE* f = std::fopen(path_bin.c_str(), "rb");
    if (!f) {
        std::cerr << "[ERROR] No puede abrir " << path_bin << " en binario.\n";
        return false;
    }

    std::vector<Par> pares;
    pares.reserve(max_pares);

    while (pares.size() < max_pares) {
        int   llave;
        float valor;
        size_t r1 = std::fread(&llave, sizeof(int),   1, f);
        size_t r2 = std::fread(&valor, sizeof(float), 1, f);
        if (r1 != 1 || r2 != 1) break; // fin de archivo o error
        pares.push_back({llave, valor}); 
    }
    std::fclose(f);

    if (pares.empty()) {
        std::cerr << "[ERROR] No se leyeron pares desde " << path_bin << " (¿ruta correcta?).\n";
        return false;
    }

    std::sort(pares.begin(), pares.end(), [](const Par& a, const Par& b){
        return a.llave < b.llave;
    });

    return construir_hojas_desde_pares(disco, pares, idx_primera_hoja);
}

// --- Programa principal ---

int main(int argc, char** argv) {
    DiscoSimulado disco;

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <ruta_a_datos_sample.txt | ruta_a_datos.bin> [MAX_PARES]\n";
        return 1;
    }

    int idx_primera_hoja = -1;
    std::string ruta = argv[1];
    size_t max_pares = (argc >= 3) ? std::stoull(argv[2]) : 1'000'000;

    bool ok = false;
    if (ruta.size() >= 4 && ruta.substr(ruta.size()-4) == ".bin") {
        ok = cargar_bin_a_hojas(disco, ruta, idx_primera_hoja, max_pares);
        if (ok) std::cout << "[OK] Leído binario desde: " << ruta << " (hasta " << max_pares << " pares)\n";
    }
    else {
        ok = cargar_sample_a_hojas(disco, ruta, idx_primera_hoja);
        if (ok) std::cout << "[OK] Leído sample desde: " << ruta << "\n";
    }
    if (!ok) return 1;

    // Construir B+ y fijar raíz (primera hoja en este caso base)
    BPlusTree bpt(&disco);
    bpt.setRaiz(idx_primera_hoja);

    // Descubrir [ℓ, u] recorriendo la lista de hojas
    Nodo h = disco.leer(idx_primera_hoja);
    int primera_llave = h.llaves_valores[0].llave;
    int ultima_llave  = h.llaves_valores[h.k - 1].llave;
    int pos = h.siguiente;

    while (pos != -1) {
        h = disco.leer(pos);
        if (h.k > 0) ultima_llave = h.llaves_valores[h.k - 1].llave;
        pos = h.siguiente;
    }


    std::cout << "Buscando rango [" << primera_llave << ", " << ultima_llave << "]\n";
    auto res = bpt.buscarRango(primera_llave, ultima_llave);
    std::cout << "Total resultados: " << res.size() << "\n";
    for (size_t i = 0; i < res.size() && i < 10; ++i)
        std::cout << res[i].llave << " -> " << res[i].valor << "\n";
    if (res.size() > 10) std::cout << "(... " << (res.size()-10) << " más)\n";

    std::cout << "Lecturas simuladas: " << disco.lecturas
              << ", Escrituras simuladas: " << disco.escrituras << "\n";
    return 0;
}
