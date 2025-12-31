#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/algoritmos.h"

// ===========================================================================
// Helpers (Funciones auxiliares internas)
// ===========================================================================

static void RotarUnoIzq(char A[], size_t Tam)
{
    if (Tam < 2)
        return;
    
    char t = A[0];
    for (size_t i = 0; i < Tam - 1; i++) 
        A[i] = A[i + 1];
    A[Tam - 1] = t;
}

static void RotarUnoDer(char A[], size_t Tam)
{
    if (Tam < 2)
        return;

    char t = A[Tam - 1];
    for (size_t i = Tam - 1; i > 0; i--) 
        A[i] = A[i - 1];
    A[0] = t;
}

static size_t mcd(size_t a, size_t b)
{
    while (b) {
        a %= b;
        size_t t = a;
        a = b;
        b = t;
    }
    return a;
}

static void Reversar(char A[], size_t ini, size_t fin)
{
    while (ini < fin) {
        char t = A[ini];
        A[ini] = A[fin];
        A[fin] = t;
        ini++;
        fin--;
    }
}

// ===========================================================================
// Implementaciones de Algoritmos (Rotación a la IZQUIERDA estandarizada)
// ===========================================================================

int V1_Ingenuo(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    // Rotación uno por uno hacia la izquierda
    for (size_t i = 0; i < k; i++)
        RotarUnoIzq(A, Tam);
    return SI;
}

int V2_IngenuoOptimo(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    // Optimización: Si k > Tam/2, conviene rotar al otro lado
    // Rotar k a la Izquierda es equivalente a rotar (Tam - k) a la Derecha
    if (k > Tam / 2) {
        size_t saltos_der = Tam - k;
        for (size_t i = 0; i < saltos_der; i++)
            RotarUnoDer(A, Tam);
    } else
        for (size_t i = 0; i < k; i++)
            RotarUnoIzq(A, Tam);
    return SI;
}

int V3_BufferCompleto_Memcpy(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    char *Buf = malloc(Tam);
    if (!Buf)
        return NO;

    // Lógica: [k...Final] va al inicio, [0...k] va al final
    memcpy(Buf, A + k, Tam - k);
    memcpy(Buf + (Tam - k), A, k);
    memcpy(A, Buf, Tam);

    free(Buf);
    return SI;
}

int V4_BufferCompleto_Manual(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    char *Buf = malloc(Tam);
    if (!Buf)
        return NO;

    // Copia manual al buffer auxiliar
    for (size_t i = 0; i < Tam - k; i++)
        Buf[i] = A[k + i];
    for (size_t i = 0; i < k; i++)
        Buf[Tam - k + i] = A[i];
    
    // Copia manual de regreso a A
    for (size_t i = 0; i < Tam; i++)
        A[i] = Buf[i];

    free(Buf);
    return SI;
}

int V5_BufferParcial_Memmove(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    // Estrategia segura: Guardar los primeros 'k' elementos (prefijo)
    char *Buf = malloc(k);
    if (!Buf)
        return NO;

    memcpy(Buf, A, k);              // 1. Respaldar el prefijo
    memmove(A, A + k, Tam - k);     // 2. Desplazar el resto al inicio (memmove maneja solapamiento)
    memcpy(A + (Tam - k), Buf, k);  // 3. Pegar el prefijo al final

    free(Buf);
    return SI;
}

int V6_BufferParcial_Manual(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    char *Buf = malloc(k);
    if (!Buf)
        return NO;

    // 1. Respaldar prefijo manualmente
    for (size_t i = 0; i < k; i++)
        Buf[i] = A[i];

    // 2. Desplazar resto a la izquierda manualmente
    for (size_t i = 0; i < Tam - k; i++)
        A[i] = A[i + k];

    // 3. Restaurar prefijo al final
    for (size_t i = 0; i < k; i++)
        A[Tam - k + i] = Buf[i];

    free(Buf);
    return SI;
}

int V7_BufferOptimo_Memmove(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    size_t L = Tam - k;
    char *Buf;

    // Usamos la menor cantidad de memoria posible: min(k, L)
    if (k < L) {
        // El bloque a mover (k) es menor que el bloque que se queda (L)
        Buf = malloc(k);
        if (!Buf)
            return NO;

        memcpy(Buf, A, k);
        memmove(A, A + k, L);
        memcpy(A + L, Buf, k);
    } else {
        // El bloque que se queda (L) es menor, usamos ese buffer
        Buf = malloc(L);
        if (!Buf)
            return NO;

        memcpy(Buf, A + k, L);
        memmove(A + L, A, k);
        memcpy(A, Buf, L);
    }

    free(Buf);
    return SI;
}

int V9_Juggling_MCD(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    size_t m = mcd(Tam, k);
    for (size_t i = 0; i < m; i++) {
        char temp = A[i];
        size_t j = i;
        while (1) {
            // Calculamos el índice "siguiente" para rotación a la izquierda
            // Queremos traer el elemento que está 'k' posiciones adelante
            size_t siguiente = (j + k);
            if (siguiente >= Tam)
                siguiente %= Tam;

            if (siguiente == i)
                break;

            A[j] = A[siguiente];
            j = siguiente;
        }
        A[j] = temp;
    }
    return SI;
}

int V11_Reversa(char A[], size_t Tam, size_t k)
{
    if (Tam == 0)
        return NO;

    k %= Tam;
    if (k == 0)
        return SI;

    // Algoritmo de 3 reversas para rotación a la Izquierda:
    // [A (k elementos)] [B (resto)] -> Queremos [B] [A]
    
    Reversar(A, 0, k - 1);       // Reversar A
    Reversar(A, k, Tam - 1);     // Reversar B
    Reversar(A, 0, Tam - 1);     // Reversar Todo

    return SI;
}

/** Esta versión no es thread-safe */
int V12_BufferEstatico(char A[], size_t Tam, size_t k)
{
    // Buffer en segmento de datos (no heap)
    static char StaticBuf[STATIC_BUFFER_SIZE];

    if (Tam == 0)
        return NO;
    
    // Verificación de seguridad
    if (Tam > STATIC_BUFFER_SIZE)
        return NO; 

    k %= Tam;
    if (k == 0)
        return SI;

    // Lógica segura de buffer parcial usando el buffer estático
    memcpy(StaticBuf, A, k);
    memmove(A, A + k, Tam - k);
    memcpy(A + (Tam - k), StaticBuf, k);

    return SI;
}

// ===========================================================================
// Implementación de Auxiliares Públicas
// ===========================================================================

void LlenarArreglo(char A[], size_t Tam)
{
    for (size_t i = 0; i < Tam; i++)
        A[i] = (char)('A' + (i % 26));
}

int ValidarResultado(const char A_Procesado[], const char A_Referencia[], size_t Tam)
{
    return (memcmp(A_Procesado, A_Referencia, Tam) == 0);
}
