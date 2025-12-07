#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include "../include/algoritmos.h" // Asegúrate que coincida con tu archivo .h

// Configuración
#define MAX_TAM 200000        // Capacidad máxima de memoria
#define NUM_REP_MEDIANA 21    // Repeticiones para estadística
#define SEMILLA_MAESTRA 12345 // Para reproducibilidad

// Macros de tiempo
#define TS_TO_NS(ts) ((double)(ts).tv_sec * 1e9 + (double)(ts).tv_nsec)

void fijar_afinidad() 
{
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(0, &set);
    if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
        perror("Advertencia: No se pudo fijar afinidad");
    }
}

int comparar_doubles(const void *a, const void *b) 
{
    return (*(double*)a > *(double*)b) - (*(double*)a < *(double*)b);
}

int main() {
    fijar_afinidad();
    
    // 1. SEGURIDAD: Comprobación de malloc
    char *A_Base = malloc(MAX_TAM);
    char *A_Test = malloc(MAX_TAM);
    char *A_Ref  = malloc(MAX_TAM);

    if (!A_Base || !A_Test || !A_Ref) {
        fprintf(stderr, "Error Fatal: Fallo malloc (sin memoria).\n");
        return 1;
    }
    
    // Catálogo
    AlgoritmoInfo algos[] = {
//        {V1_Ingenuo, "V1_Ingenuo", "O(N*k) Lento"},
//        {V2_IngenuoOptimo, "V2_Optimo", "O(N*k) Direccional"},
        {V3_BufferCompleto_Memcpy, "V3_Memcpy", "O(N) Buffer Total"},
        {V4_BufferCompleto_Manual, "V4_Manual", "O(N) Buffer Total Manual"},
        {V5_BufferParcial_Memmove, "V5_Memmove", "O(k) Buffer Parcial"},
        {V6_BufferParcial_Manual, "V6_Manual", "O(k) Manual"},
        {V7_BufferOptimo_Memmove, "V7_Optimo", "O(min) Buffer Optimo"},
        {V9_Juggling_MCD, "V9_Juggling", "O(N) In-place"},
        {V11_Reversa, "V11_Reversa", "O(N) In-place 3-pass"},
        {V12_BufferEstatico, "V12_Static", "O(N) Sin Malloc"},
        {NULL, NULL, NULL} // CENTINELA VITAL
    };

    // 2. SEGURIDAD: Comprobación de archivo
    FILE *f = fopen("data/resultados/test_.csv", "w");
    if (!f) {
        perror("Error Fatal: No se pudo crear el archivo CSV");
        fprintf(stderr, "Sugerencia: Ejecuta 'mkdir -p data/resultados'\n");
        free(A_Base); free(A_Test); free(A_Ref);
        return 1;
    }
    // Agregamos columna 'Estado' para diferenciar fallos de lógica vs recursos
    fprintf(f, "Algoritmo,Tamano,Tiempo_ns,Estado\n");

    struct timespec start, end;
    double tiempos[NUM_REP_MEDIANA];

    // Calibrar Overhead
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double overhead = TS_TO_NS(end) - TS_TO_NS(start);
    printf("Calibracion: Overhead Timer %.2f ns\n", overhead);

    // Bucle de Tamaños
    for (size_t Tam = 10000; Tam <= 100000; Tam += 10000) {
        printf("\n--- Tamano: %zu ---\n", Tam);
        LlenarArreglo(A_Base, Tam);

        for (int i = 0; algos[i].func; i++) {
            printf("%-15s ", algos[i].nombre_corto);
            fflush(stdout);

            int estado_global = 1; // 1=OK, 0=Fallo Logica, -1=Fallo Recursos

            // MEDICIÓN ROBUSTA
            for (int r = 0; r < NUM_REP_MEDIANA; r++) {
                // NOTA DOCUMENTADA: Reiniciamos la semilla para que CADA algoritmo
                // enfrente exactamente la misma secuencia de 'k' aleatorios.
                // Esto garantiza una comparación justa caso por caso.
                srand(SEMILLA_MAESTRA + r); 
                size_t k = rand() % Tam;
                if (k == 0) k = 1;

                // Preparar
                memcpy(A_Test, A_Base, Tam);

                // Medir
                clock_gettime(CLOCK_MONOTONIC, &start);
                int resultado = algos[i].func(A_Test, Tam, k); // <--- 3. CHECK RETORNO
                clock_gettime(CLOCK_MONOTONIC, &end);

                // Truco anti-optimización
                volatile char dummy = A_Test[k % Tam];
                (void)dummy;

                if (resultado == NO) {
                    estado_global = -1; // Fallo de Recursos (ej. V12 buffer lleno)
                    tiempos[r] = 0;     // Tiempo inválido
                } else {
                    double t = (TS_TO_NS(end) - TS_TO_NS(start)) - overhead;
                    tiempos[r] = (t > 0) ? t : 0;

                    // 4. VALIDACIÓN MUESTRAL (Solo la primera repetición para no alentar todo)
                    if (r == 0) {
                        memcpy(A_Ref, A_Base, Tam);
                        V3_BufferCompleto_Memcpy(A_Ref, Tam, k); // Referencia confiable
                        if (memcmp(A_Test, A_Ref, Tam) != 0) {
                            estado_global = 0; // Fallo de Lógica (Bug en algoritmo)
                        }
                    }
                }
            }

            // Reporte y CSV
            if (estado_global == -1) {
                printf("[RECURSOS INSUF]\n");
                fprintf(f, "%s,%zu,0,Fallo_Recursos\n", algos[i].nombre_corto, Tam);
            } 
            else if (estado_global == 0) {
                printf("[ERROR LOGICO]\n");
                fprintf(f, "%s,%zu,0,Fallo_Logica\n", algos[i].nombre_corto, Tam);
            } 
            else {
                // Calcular mediana solo si fue exitoso
                qsort(tiempos, NUM_REP_MEDIANA, sizeof(double), comparar_doubles);
                double mediana = tiempos[NUM_REP_MEDIANA/2];
                printf("%.2f us\n", mediana / 1000.0);
                fprintf(f, "%s,%zu,%.2f,OK\n", algos[i].nombre_corto, Tam, mediana);
            }
        }
    }

    fclose(f);
    free(A_Base); free(A_Test); free(A_Ref);
    return 0;
}
