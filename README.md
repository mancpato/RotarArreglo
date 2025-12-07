# Benchmark de Rotación de Arreglos (C + Python)

Este proyecto es un laboratorio de **Arquitectura de Computadoras y Algoritmia** diseñado para comparar empíricamente el rendimiento de diversos algoritmos que resuelven un mismo problema: la rotación de un arreglo unidimensional.

El objetivo pedagógico es demostrar que la **Complejidad Asintótica (Big-O)** no es el único factor que determina el rendimiento en el mundo real. Factores como la **localidad de caché**, las **instrucciones vectoriales (SIMD)**, la **gestión de memoria dinámica (malloc)** y las **optimizaciones del compilador** juegan un papel crucial en sistemas modernos.

## Características

  * **Implementación Modular en C:** Código estructurado profesionalmente separando lógica, interfaz y *harness* de pruebas.
  * **10 Variantes Algorítmicas:** Desde enfoques ingenuos $O(N \cdot k)$ hasta optimizaciones de bajo nivel con `memcpy` y aritmética de punteros.
  * **Compilación Multi-Target:** Sistema `Makefile` robusto para generar ejecutables de **Release** (Optimizado -O3), **Debug** (Símbolos -O0) y **Profile** (Instrumentado -pg).
  * **Metodología Científica:**
      * Semillas aleatorias controladas para reproducibilidad.
      * Uso de la **mediana** estadística para filtrar ruido del SO.
      * Validación de corrección lógica automática.
  * **Visualización de Datos:** Scripts de Python (`pandas` + `seaborn`) para generar gráficas comparativas de alto impacto.

## Estructura del Proyecto

```text
.
├── src/                # Código fuente (.c y .py)
│   ├── algorithms.c    # Implementación de las 10 versiones
│   ├── main.c          # Harness de medición científica
│   └── visualize.py    # Script de generación de gráficas
├── include/            # Encabezados (.h)
├── bin/                # Ejecutables generados (release, debug, profile)
├── build/              # Archivos objeto intermedios (.o)
├── data/
│   └── resultados/     # Salida de los benchmarks (.csv)
├── figures/            # Gráficas generadas (.png)
└── Makefile            # Sistema de compilación automatizado
```

## Algoritmos Comparados

| ID | Nombre | Complejidad Tiempo | Complejidad Espacio | Descripción |
| :--- | :--- | :--- | :--- | :--- |
| **V1** | Ingenuo | $O(N \cdot k)$ | $O(1)$ | Rotación iterativa uno por uno. Lento. |
| **V2** | Ingenuo Óptimo | $O(N \cdot k)$ | $O(1)$ | Elige la dirección más corta (izq/der). |
| **V3** | Buffer Completo | $O(N)$ | $O(N)$ | Copia total usando `malloc` y `memcpy`. |
| **V4** | Buffer Manual | $O(N)$ | $O(N)$ | Igual que V3 pero con bucles `for` (sin SIMD). |
| **V5** | Buffer Parcial | $O(N)$ | $O(k)$ | Guarda solo el prefijo en memoria auxiliar. |
| **V6** | Parcial Manual | $O(N)$ | $O(k)$ | Versión manual de V5. |
| **V7** | Buffer Óptimo | $O(N)$ | $O(\min(k, N-k))$ | Usa la menor memoria posible. |
| **V9** | Juggling (MCD) | $O(N)$ | $O(1)$ | Algoritmo clásico de saltos (K\&R). Pobre localidad de caché. |
| **V11** | Reversa | $O(N)$ | $O(1)$ | Algoritmo de 3 inversiones. Amigable al caché. |
| **V12** | Buffer Estático | $O(N)$ | $O(N)$ | Usa memoria `.bss` (sin overhead de `malloc`). |

##  Instalación y Requisitos

### Prerrequisitos

  * **GCC** (Compilador de C)
  * **Make** (Sistema de construcción)
  * **Python 3.x** (Para visualización)

### Configuración del Entorno Python

Se recomienda usar un entorno virtual para las dependencias de visualización:

```bash
python3 -m venv .venv
source .venv/bin/activate  # En Windows: .venv\Scripts\activate
pip install -r requirements.txt
```

*(Contenido de requirements.txt: pandas, seaborn, matplotlib)*

## Uso

El proyecto utiliza un `Makefile` para gestionar todas las tareas.

### 1\. Compilar y Ejecutar Benchmark (Release)

Esta es la prueba principal de rendimiento. Genera el CSV en `data/resultados/`.

```bash
make run
```

### 2\. Generar Gráficas

Una vez ejecutado el benchmark, procesa los datos con Python:

```bash
python src/visualize.py
```

Las imágenes se guardarán en la carpeta `figures/`.

### 3\. Depuración y Perfilado (Opcional)

  * **Modo Debug:** Para usar con GDB. Sin optimizaciones.
    ```bash
    make debug
    gdb ./bin/rotar_debug
    ```
  * **Modo Profile:** Para análisis con `gprof`.
    ```bash
    make run-profile
    gprof bin/rotar_profile gmon.out > perfil.txt
    ```

## Autor

**Miguel Ángel Norzagaray Cosío**
*Universidad Autónoma de Baja California Sur (UABCS)*
Departamento de Sistemas Computacionales.

-----

*Este proyecto fue desarrollado como material didáctico para demostrar principios de optimización de software y arquitectura de computadoras.*
