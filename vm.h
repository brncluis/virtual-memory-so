#ifndef VM_H
#define VM_H

#include <stdio.h>

#define NUM_PAGINAS 256
#define NUM_FRAMES 128
#define TAM_FRAME 256
#define TAM_TLB 16

typedef enum {
    POLITICA_FIFO,
    POLITICA_LRU
} Politica;

typedef struct {
    int frame;
    int valido;
} EntradaTabelaPaginas;

typedef struct {
    int pagina;
    unsigned long carregado_em;
    unsigned long ultimo_uso;
} InfoFrame;

typedef struct {
    int pagina;
    int frame;
    int valido;
    unsigned long carregado_em;
    unsigned long ultimo_uso;
} EntradaTLB;

typedef struct {
    signed char ram[NUM_FRAMES][TAM_FRAME];
    EntradaTabelaPaginas tabela_paginas[NUM_PAGINAS];
    InfoFrame frames[NUM_FRAMES];
    EntradaTLB tlb[TAM_TLB];

    int proximo_frame_livre;
    unsigned long relogio;

    Politica politica_pagina;
    Politica politica_tlb;

    int total_enderecos;
    int faltas_pagina;
    int acertos_tlb;
} VM;

#endif