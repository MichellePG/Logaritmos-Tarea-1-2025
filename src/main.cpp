#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include "btree.hpp"
#include "bplustree.hpp"
#include <random>

struct Crono {
    std::chrono::high_resolution_clock::time_point t0;
    void iniciar() { t0 = std::chrono::high_resolution_clock::now(); }
    double ms() const {
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
};

// Estructura utilizada para leer argumentos de línea de comandos
struct Args {
    bool construir;
    bool consultar;
    bool usar_bplus;
    bool experimento;
    size_t N;
    std::string ruta_datos;
    std::string ruta_b;
    std::string ruta_bplus;
    int32_t L;
    int32_t U;

    Args()
    : construir(false), consultar(false), usar_bplus(false), experimento(false),
      N(0), ruta_datos(""),
      ruta_b("b_tree.bin"), ruta_bplus("bplus_tree.bin"),
      L(0), U(0) {}
};

static Args leer_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; i++) {
        std::string s = argv[i];
        if (s == "--build") a.construir = true;
        else if (s == "--query") a.consultar = true;
        else if (s == "--bplus") a.usar_bplus = true;
        else if (s == "--experimento") a.experimento = true;
        else if (s == "--N" && i + 1 < argc) a.N = std::stoull(argv[++i]);
        else if (s == "--datos" && i + 1 < argc) a.ruta_datos = argv[++i];
        else if (s == "--out-b" && i + 1 < argc) a.ruta_b = argv[++i];
        else if (s == "--out-bp" && i + 1 < argc) a.ruta_bplus = argv[++i];
        else if (s == "--l" && i + 1 < argc) a.L = std::stoi(argv[++i]);
        else if (s == "--u" && i + 1 < argc) a.U = std::stoi(argv[++i]);
    }
    return a;
}

// Lee N pares (clave, valor) desde un binario plano (int32, float).
static std::vector<Par> leer_pares_bin(const std::string& ruta, size_t N) {
    std::vector<Par> v;
    v.reserve(N);

    std::ifstream in(ruta.c_str(), std::ios::binary);
    if (!in) throw std::runtime_error("No pude abrir datos.bin: " + ruta);

    for (size_t i = 0; i < N; i++) {
        Par p;
        in.read(reinterpret_cast<char*>(&p.clave), 4);
        in.read(reinterpret_cast<char*>(&p.valor), 4);
        if (!in) break; // por si el archivo es más corto que N
        v.push_back(p);
    }
    return v;
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Args args = leer_args(argc, argv);

    try {
        if (args.construir) {
            if (args.N == 0 || args.ruta_datos.empty()) {
                std::cerr << "Uso: --build --N <num> --datos <ruta_datos.bin> "
                             "[--out-b <b_tree.bin>] [--out-bp <bplus_tree.bin>]\n";
                return 1;
            }

            // Cargar datos
            std::vector<Par> pares = leer_pares_bin(args.ruta_datos, args.N);

            // Construir Árbol B en RAM
            BTree arbolB;
            Crono t1; t1.iniciar();
            for (size_t i = 0; i < pares.size(); ++i)
                arbolB.insertar(pares[i].clave, pares[i].valor);
            double tiempoB = t1.ms();
            IOStats ioB = arbolB.disco.stats;

            // Serializar Árbol B a archivo
            arbolB.serializar(args.ruta_b);

            std::cerr << "B:  tiempo(ms)=" << tiempoB
                      << "  IOsRAM L/E=" << ioB.lecturas << "/" << ioB.escrituras
                      << "  nodos=" << arbolB.disco.cantidad() << "\n";

            // Construir Árbol B+ en RAM
            BPlusTree arbolBP;
            Crono t2; t2.iniciar();
            for (size_t i = 0; i < pares.size(); ++i)
                arbolBP.insertar(pares[i].clave, pares[i].valor);
            double tiempoBP = t2.ms();
            IOStats ioBP = arbolBP.disco.stats;

            // Serializar Árbol B+ a archivo
            arbolBP.serializar(args.ruta_bplus);

            std::cerr << "B+: tiempo(ms)=" << tiempoBP
                      << "  IOsRAM L/E=" << ioBP.lecturas << "/" << ioBP.escrituras
                      << "  nodos=" << arbolBP.disco.cantidad() << "\n";

            return 0;
        }

        if (args.consultar) {
            if (args.U == 0) {
                std::cerr << "Uso: --query --l <L> --u <U> [--bplus] "
                             "[--out-b <b_tree.bin>] [--out-bp <bplus_tree.bin>]\n";
                return 1;
            }

            if (args.usar_bplus) {
                // Consultar en B+ leyendo desde disco
                IOStats ios;
                Crono t; t.iniciar();
                std::vector<Par> res = BPlusTree().consulta_rango(args.ruta_bplus, args.L, args.U, &ios);
                double tiempo = t.ms();
                std::cout << "#res=" << res.size()
                          << "  IOs(lec)=" << ios.lecturas
                          << "  t(ms)=" << tiempo << "\n";
            } else {
                // Consultar en B leyendo desde disco
                IOStats ios;
                Crono t; t.iniciar();
                std::vector<Par> res = BTree().consulta_rango(args.ruta_b, args.L, args.U, &ios);
                double tiempo = t.ms();
                std::cout << "#res=" << res.size()
                          << "  IOs(lec)=" << ios.lecturas
                          << "  t(ms)=" << tiempo << "\n";
            }
            return 0;
        }

        if (args.experimento) {
            if (args.ruta_datos.empty()) {
                std::cerr << "Uso: --experimento --datos <ruta_datos.bin>\n";
                return 1;
            }

            std::cout << "N,arbol,nodos,bytes,t_creacion_ms,io_crea_lec,io_crea_esc,t_busq_ms_prom,io_busq_lec_prom\n";

            std::mt19937_64 rng(std::random_device{}());
            std::uniform_int_distribution<int> dist(1546300800, 1754006400);

            for (int e = 15; e <= 26; e++) {
                size_t N = 1ull << e;
                std::vector<Par> pares = leer_pares_bin(args.ruta_datos, N);
                if (pares.empty()) break;

                // B
                BTree arbolB;
                Crono t1; t1.iniciar();
                for (auto& p : pares) arbolB.insertar(p.clave, p.valor);
                double tiempoB = t1.ms();
                IOStats ioB = arbolB.disco.stats;
                arbolB.serializar("b_tree.bin");

                // 50 queries
                double suma_ms_B=0; double suma_io_B=0;
                for (int q=0;q<50;q++){
                    int L = dist(rng);
                    int U = L+604800;
                    IOStats ios;
                    Crono tq; tq.iniciar();
                    auto res = BTree().consulta_rango("b_tree.bin",L,U,&ios);
                    (void)res;
                    suma_ms_B += tq.ms();
                    suma_io_B += ios.lecturas;
                }

                std::cout << N << ",B," << arbolB.disco.cantidad() << "," << arbolB.disco.cantidad()*sizeof(Nodo)
                          << "," << tiempoB << "," << ioB.lecturas << "," << ioB.escrituras
                          << "," << (suma_ms_B/50.0) << "," << (suma_io_B/50.0) << "\n";

                // B+
                BPlusTree arbolBP;
                Crono t2; t2.iniciar();
                for (auto& p : pares) arbolBP.insertar(p.clave, p.valor);
                double tiempoBP = t2.ms();
                IOStats ioBP = arbolBP.disco.stats;
                arbolBP.serializar("bplus_tree.bin");

                double suma_ms_BP=0; double suma_io_BP=0;
                for (int q=0;q<50;q++){
                    int L = dist(rng);
                    int U = L+604800;
                    IOStats ios;
                    Crono tq; tq.iniciar();
                    auto res = BPlusTree().consulta_rango("bplus_tree.bin",L,U,&ios);
                    (void)res;
                    suma_ms_BP += tq.ms();
                    suma_io_BP += ios.lecturas;
                }

                std::cout << N << ",B+," << arbolBP.disco.cantidad() << "," << arbolBP.disco.cantidad()*sizeof(Nodo)
                          << "," << tiempoBP << "," << ioBP.lecturas << "," << ioBP.escrituras
                          << "," << (suma_ms_BP/50.0) << "," << (suma_io_BP/50.0) << "\n";
            }
            return 0;
        }

        std::cerr << "Ejemplos:\n"
                  << "  ./tarea1 --build --N 32768 --datos datos/datos.bin\n"
                  << "  ./tarea1 --query --l 1546300800 --u 1546905600 --bplus\n"
                  << "  ./tarea1 --experimento --datos datos/datos.bin > resultados.csv\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
}
