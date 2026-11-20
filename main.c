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

    while (*p && isspace((unsigned char)*p) != '\0') {
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

    while (*fim && isspace((unsigned char)*fim)) {
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

    Politica pol_pagina;
    Politica pol_tlb;

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
            return EXIT_FAILURE;
        }

        int pagina, deslocamento;
        extrair_pagina_deslocamento(endereco_logico, &pagina, &deslocamento);

        if (vm.total_enderecos < 5) {
            printf("Linha %d | Logico: %d -> Pagina: %d, Deslocamento: %d\n",
                   numero_linha, endereco_logico, pagina, deslocamento);
        }
        vm.total_enderecos++;
    }

    if (vm.total_enderecos == 0) {
        fprintf(stderr, "Erro: o arquivo '%s' esta vazio.\n", argv[1]);
        fclose(arq_enderecos);
        fclose(backing_store);
        return EXIT_FAILURE;
    }

    printf("Total de enderecos lidos com sucesso: %d\n", vm.total_enderecos);

    fclose(arq_enderecos);
    fclose(backing_store);
    return EXIT_SUCCESS;
}