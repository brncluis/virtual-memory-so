#include "vm.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ler_politica(char *texto, Politica *destino) {
    if (strcmp(texto, "fifo") == 0) {
        *destino = POLITICA_FIFO;
        return 1;
    }
    if (strcmp(texto, "lru") == 0) {
        *destino = POLITICA_LRU;
        return 1;
    }
    return 0;
}

int parsear_endereco_linha(char *linha, int *endereco_out) {
    char *p = linha;
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }
    if (*p == '\0') {
        return 0;
    }

    char *fim = NULL;
    errno = 0;
    long valor = strtol(p, &fim, 10);

    if (errno != 0 || fim == p || valor < 0 || valor > INT_MAX) {
        return 0;
    }

    while (*fim != '\0' && isspace((unsigned char)*fim)) {
        fim++;
    }
    if (*fim != '\0') {
        return 0;
    }

    *endereco_out = (int)valor;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Erro: numero invalido de argumentos.\n");
        fprintf(stderr, "Uso: %s <arquivo_enderecos> <fifo|lru> <fifo|lru>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Politica pol_pagina, pol_tlb;
    if (!ler_politica(argv[2], &pol_pagina)) {
        fprintf(stderr, "Erro: algoritmo de pagina invalido '%s'. Use 'fifo' ou 'lru'.\n", argv[2]);
        return EXIT_FAILURE;
    }
    if (!ler_politica(argv[3], &pol_tlb)) {
        fprintf(stderr, "Erro: algoritmo de TLB invalido '%s'. Use 'fifo' ou 'lru'.\n", argv[3]);
        return EXIT_FAILURE;
    }

    FILE *arq_enderecos = fopen(argv[1], "r");
    if (!arq_enderecos) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

    FILE *backing_store = fopen("BACKING_STORE.bin", "rb");
    if (!backing_store) {
        fprintf(stderr, "Erro: nao foi possivel abrir 'BACKING_STORE.bin'.\n");
        fclose(arq_enderecos);
        return EXIT_FAILURE;
    }

    FILE *saida = fopen("correct.txt", "w");
    if (!saida) {
        fprintf(stderr, "Erro: nao foi possivel criar 'correct.txt'.\n");
        fclose(arq_enderecos);
        fclose(backing_store);
        return EXIT_FAILURE;
    }

    VM vm;
    inicializar_vm(&vm, pol_pagina, pol_tlb);

    char linha[256];
    int numero_linha = 0;
    int endereco_logico = 0;

    while (fgets(linha, sizeof(linha), arq_enderecos) != NULL) {
        numero_linha++;
        if (!parsear_endereco_linha(linha, &endereco_logico)) {
            fprintf(stderr, "Erro: formato invalido na linha %d do arquivo '%s'.\n",
                    numero_linha, argv[1]);
            fclose(arq_enderecos);
            fclose(backing_store);
            fclose(saida);
            return EXIT_FAILURE;
        }

        int endereco_fisico = 0;
        signed char valor = 0;
        int indice_tlb = 0;

        if (!traduzir_endereco(&vm, endereco_logico, backing_store, &endereco_fisico, &valor, &indice_tlb)) {
            fprintf(stderr, "Erro: falha ao ler 'BACKING_STORE.bin' na linha %d.\n", numero_linha);
            fclose(arq_enderecos);
            fclose(backing_store);
            fclose(saida);
            return EXIT_FAILURE;
        }

        fprintf(saida, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n",
        endereco_logico, indice_tlb, endereco_fisico, valor);
    }

    if (vm.total_enderecos == 0) {
        fprintf(stderr, "Erro: o arquivo '%s' esta vazio.\n", argv[1]);
        fclose(arq_enderecos);
        fclose(backing_store);
        fclose(saida);
        return EXIT_FAILURE;
    }

    fprintf(saida, "Number of Translated Addresses = %d\n", vm.total_enderecos);
    fprintf(saida, "Page Faults = %d\n", vm.faltas_pagina);
    fprintf(saida, "Page Fault Rate = %.3f\n", (double)vm.faltas_pagina / vm.total_enderecos);
    fprintf(saida, "TLB Hits = %d\n", vm.acertos_tlb);
    fprintf(saida, "TLB Hit Rate = %.3f\n", (double)vm.acertos_tlb / vm.total_enderecos);

    fclose(arq_enderecos);
    fclose(backing_store);
    fclose(saida);
    return EXIT_SUCCESS;
}