# Tarea 1 – Árboles B y B+ en Disco
Implementación de árboles B y B+ en disco para la Tarea 1 de CC4102 – Diseño y Análisis de Algoritmos (2025-2).

## Integrantes
- Ivo Fuenzalida 
- Michelle Pérez
- Diego Sánchez

## Descripción
Este proyecto implementa árboles **B** y **B+** simulando memoria externa.  
Se soportan operaciones de inserción de pares `(llave, valor)` y búsqueda por rango `[ℓ, u]`.  
Los datos de entrada corresponden a temperaturas reales de la estación Quinta Normal, , en formato texto (`.txt`) o binario (`.bin`).


## Estructura del Proyecto

```
Logaritmos-Tarea-1-2025/
│
├── include/ # Archivos de cabecera (.hpp)
│ ├── nodo.hpp # Definición de la estructura Nodo
│ ├── disco.hpp # Disco simulado con contadores de I/O
│ ├── btree.hpp # B-Tree clásico (inserción (TO DO), split (TO DO) y búsqueda (IN REVIEW))
│ └── bplustree.hpp # B+ Tree (inserción (TO DO), split (TO DO) y búsqueda (IN REVIEW))
│
├── src/ # Código fuente (.cpp)
│ ├── main.cpp # programa principal
│ ├── disco.cpp # Implementación del disco simulado
│ ├── btree.cpp # B-Tree clásico
│ └── bplustree.cpp # B+ Tree
│
├── datos/ # Datos de entrada
│ └── datos_sample.txt # Sample de 50 pares llave-valor en texto
│
├── Makefile # Instrucciones de compilación y ejecución
└── README.md # Este archivo
```

## Compilación

Para compilar los programas, ejecute:

```bash
make
```

## Uso

Para ejecutar con datos de datos_sample.txt:

```bash
make run
```

Para ejecutar con datos binarios (con 2 millones de pares de ejemplo):

```bash
./tarea1 datos/datos.bin 2000000
```

Nota: datos.bin es muy pesado, por lo que debe descargarse aparte y colocarse en la carpeta datos/.

## Limpieza

Para eliminar binarios y objetos compilados:

```bash
make clean
```
