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