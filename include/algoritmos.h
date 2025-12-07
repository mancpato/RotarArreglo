#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include <stddef.h> // size_t

// Definiciones
#define SI 1
#define NO 0
#define STATIC_BUFFER_SIZE 100000 

typedef int (*RotadorFunc)(char[], size_t, size_t);

typedef struct {
    RotadorFunc func;
    const char* nombre_corto;
    const char* descripcion;
} AlgoritmoInfo;

// ---------------------------------------------------------------------------
// Prototipos (Todos estandarizados a ROTACIÓN IZQUIERDA)
// ---------------------------------------------------------------------------

// Familia Ingenua
int V1_Ingenuo(char A[], size_t Tam, size_t k);
int V2_IngenuoOptimo(char A[], size_t Tam, size_t k);

// Familia Buffer (O(N) memoria)
int V3_BufferCompleto_Memcpy(char A[], size_t Tam, size_t k);
int V4_BufferCompleto_Manual(char A[], size_t Tam, size_t k); // Antes V3b

// Familia Buffer Parcial (O(k) o O(N-k))
int V5_BufferParcial_Memmove(char A[], size_t Tam, size_t k); // Corregido a memmove [cite: 120]
int V6_BufferParcial_Manual(char A[], size_t Tam, size_t k);

// Familia Buffer Óptimo
int V7_BufferOptimo_Memmove(char A[], size_t Tam, size_t k); // Corregido a memmove [cite: 134]

// Familia Juggling (O(1) memoria)
int V9_Juggling_MCD(char A[], size_t Tam, size_t k);

// Familia Reversa
int V11_Reversa(char A[], size_t Tam, size_t k);

// Buffer Estático
int V12_BufferEstatico(char A[], size_t Tam, size_t k);

// Auxiliares
void LlenarArreglo(char A[], size_t Tam);
int ValidarResultado(const char A_Procesado[], const char A_Referencia[], size_t Tam);

#endif
