# Tarea 1 – Árboles B y B+ en Disco
Implementación de árboles **B** y **B+** en disco para la Tarea 1 de CC4102 – Diseño y Análisis de Algoritmos (2025-2).

## Integrantes
- Ivo Fuenzalida  
- Michelle Pérez  
- Diego Sánchez  

## Descripción
Este proyecto implementa árboles **B** y **B+** simulando memoria externa mediante nodos de **4096 bytes**.  

Se soportan las siguientes operaciones:
- **Inserción** de pares `(clave:int32, valor:float)` en RAM (simulada con `DiskArray`).
- **Serialización** de la estructura a un archivo binario (simulación de disco).
- **Consulta por rango** `[ℓ, u]`, leyendo únicamente desde disco (`FileStorage`).

Los datos de entrada corresponden a registros de temperaturas reales de la estación **Quinta Normal**.

## Estructura del Proyecto

```
Logaritmos-Tarea-1-2025/
│
├── include/ # Archivos de cabecera (.hpp)
│ ├── node.hpp # Definición de Par y Nodo
│ ├── disk.hpp # Simulación de disco en RAM y acceso a archivo
│ ├── btree.hpp # Definición del Árbol B
│ └── bplustree.hpp # Definición del Árbol B+
│
├── src/ # Código fuente (.cpp)
│ ├── main.cpp # CLI principal (--build / --query)
│ ├── disk.cpp # Implementación de DiskArray y FileStorage
│ ├── btree.cpp # Implementación del Árbol B
│ └── bplustree.cpp # Implementación del Árbol B+
│
├── datos/ # Carpeta de datos para pruebas
│ ├── datos_sample.txt # Sample de 50 pares llave-valor en texto
│ └── datos.bin # Archivo binario de pares (clave, valor) no incluido en este repositorio
│
├── Makefile # Instrucciones de compilación y ejecución
└── README.md # Este archivo
```

## Compilación

Para compilar los programas, ejecute:

```bash
make
```

Esto generará un ejecutable tarea1.

Para recompilar desde cero:

```bash
make clean && make
```

## Uso

### Construcción de los árboles

Ejemplo con N = 32768 pares:

```bash
./tarea1 --build --N 32768 --datos datos/datos.bin
```

Esto construye ambos árboles (B y B+) en RAM, los serializa a disco (b_tree.bin y bplus_tree.bin), y reporta:

- Tiempo total en milisegundos.
- Cantidad de operaciones de I/O simuladas en RAM.
- Número de nodos generados.

### Consulta por rango

Ejemplo de consulta en Árbol B+:

```bash
./tarea1 --query --l 1546300800 --u 1546905600 --bplus
```

Salida típica:

```bash
#res=1024  IOs(lec)=58  t(ms)=0.42
```

Donde:
- #res = cantidad de resultados en el rango.
- IOs(lec) = lecturas desde disco simuladas.
- t(ms) = tiempo total en milisegundos.

### Experimentación completa

Para correr automáticamente la experimentación pedida en el enunciado (N = 2^15 … 2^26, 50 consultas aleatorias por cada N), ejecute:

```bash
make run-experimento
```
Esto genera un archivo resultados.csv con el siguiente formato:

N,arbol,nodos,bytes,t_creacion_ms,io_crea_lec,io_crea_esc,t_busq_ms_prom,io_busq_lec_prom
32768,B,132,540672,43.6,130777,33159,1.03,2.46
32768,B+,132,540672,37.8,130777,33159,1.11,3.42
...

## Parámetros principales
- --build : construye árboles desde un archivo binario de datos.
- --query : ejecuta una consulta por rango [ℓ, u].
- --bplus : indica que la consulta se hace en Árbol B+ (si no se pasa, usa Árbol B).
- --experimento : corre la experimentación completa y genera un CSV.
- --N <num> : número de pares a cargar desde el archivo.
- --datos <ruta> : archivo binario de entrada (datos.bin).
- --out-b <ruta> : archivo de salida del Árbol B (default b_tree.bin).
- --out-bp <ruta> : archivo de salida del Árbol B+ (default bplus_tree.bin).

## Limpieza

Para eliminar binarios y objetos compilados:

```bash
make clean
```