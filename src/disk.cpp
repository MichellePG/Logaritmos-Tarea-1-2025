#include "disk.hpp"

void DiskArray::push_sin_contar(const Nodo& n) {
    v.push_back(n);
}

Nodo DiskArray::obtener(size_t i) {
    stats.lecturas++;
    return v[i];
}

void DiskArray::fijar(size_t i, const Nodo& n) {
    stats.escrituras++;
    v[i] = n;
}

size_t DiskArray::anexar(const Nodo& n) {
    stats.escrituras++;
    v.push_back(n);
    return v.size() - 1;
}

size_t DiskArray::cantidad() const {
    return v.size();
}

const std::vector<Nodo>& DiskArray::crudo() const {
    return v;
}

void FileStorage::escribir_todos(const std::vector<Nodo>& nodos) {
    std::ofstream out(ruta.c_str(), std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("No se pudo abrir " + ruta + " para escritura");

    for (size_t i = 0; i < nodos.size(); ++i) {
        const Nodo& n = nodos[i];
        out.write(reinterpret_cast<const char*>(&n), sizeof(Nodo));
        stats.escrituras++;
    }
}

Nodo FileStorage::leer_en(size_t i) {
    std::ifstream in(ruta.c_str(), std::ios::binary);
    if (!in) throw std::runtime_error("No se pudo abrir " + ruta + " para lectura");

    in.seekg((std::streamoff)i * (std::streamoff)sizeof(Nodo), std::ios::beg);

    Nodo n;
    in.read(reinterpret_cast<char*>(&n), sizeof(Nodo));
    if (!in) throw std::runtime_error("Lectura incompleta en índice " + std::to_string(i));

    stats.lecturas++;
    return n;
}