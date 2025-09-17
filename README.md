# Tarea 1 – Árboles B y B+ en Disco
Implementación de árboles B y B+ en disco para la Tarea 1 de CC4102 – Diseño y Análisis de Algoritmos (2025-2).

## Integrantes
- Ivo Fuenzalida 
- Michelle Pérez
- Diego Sánchez

## Descripción
Este proyecto implementa árboles **B** y **B+** simulando memoria externa.  
Se soportan operaciones de inserción de pares `(llave, valor)` y búsqueda por rango `[ℓ, u]`.  
Los datos de entrada corresponden a temperaturas reales de la estación Quinta Normal. 

## Estructura del Proyecto

```
Logaritmos-Tarea-1-2025/
│
├── include/ # Archivos de cabecera (.hpp)
│ └── nodo.hpp # Definición de la estructura Nodo
│
├── src/ # Código fuente (.cpp)
│ ├── main.cpp # Programa para ...
│
├── datos/ # Datos para experimentar
│ ├── datos_sample.txt # Primeros 50 pares llave-valor en forma de texto
│ └── datos.bin # Datos de temperatura (67 millones de pares)
│
├── Makefile # Instrucciones simples de compilación
└── README.md # Este archivo
```

## Compilación

Para compilar los programas, ejecute:

```bash
make
```

## Uso



## Limpieza

Para limpiar los archivos generados:

```bash
make clean
```
