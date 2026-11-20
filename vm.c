#include "vm.h"
#include <string.h>

void inicializar_vm(VM *vm, Politica pol_pagina, Politica pol_tlb) {
    memset(vm->ram, 0, sizeof(vm->ram));

    for (int i = 0; i < NUM_PAGINAS; i++) {
        vm->tabela_paginas[i].frame = -1;
        vm->tabela_paginas[i].valido = 0;
    }

    for (int i = 0; i < NUM_FRAMES; i++) {
        vm->frames[i].pagina = -1;
        vm->frames[i].carregado_em = 0;
        vm->frames[i].ultimo_uso = 0;
    }

    for (int i = 0; i < TAM_TLB; i++) {
        vm->tlb[i].pagina = -1;
        vm->tlb[i].frame = -1;
        vm->tlb[i].valido = 0;
        vm->tlb[i].carregado_em = 0;
        vm->tlb[i].ultimo_uso = 0;
    }

    vm->proximo_frame_livre = 0;
    vm->relogio = 0;
    vm->politica_pagina = pol_pagina;
    vm->politica_tlb = pol_tlb;
    vm->total_enderecos = 0;
    vm->faltas_pagina = 0;
    vm->acertos_tlb = 0;
}

void extrair_pagina_deslocamento(int endereco_logico, int *pagina, int *deslocamento) {
    int endereco_16bits = endereco_logico % 65536;
    *pagina = endereco_16bits / TAM_FRAME;
    *deslocamento = endereco_16bits % TAM_FRAME;
}

int escolher_frame(VM *vm) {

    if (vm->proximo_frame_livre < NUM_FRAMES) {
        return vm->proximo_frame_livre++;
    }

    int vitima = 0;
    for (int i = 0; i < NUM_FRAMES; i++) {

        if (vm->politica_pagina == POLITICA_FIFO) {
            if (vm->frames[i].carregado_em < vm->frames[vitima].carregado_em) {
                vitima = i;
            }
        }

        else {
            if (vm->frames[i].ultimo_uso < vm->frames[vitima].ultimo_uso) {
                vitima = i;
            }
        }
    }

    return vitima;

}

int carregar_pagina_do_disco(VM *vm, int pagina, int frame, FILE *backing_store) {

    long posicao = (long)pagina * TAM_FRAME;

    if (fseek(backing_store, posicao, SEEK_SET) != 0) {
        return 0; 
    }

    size_t lidos = fread(vm->ram[frame], sizeof(signed char), TAM_FRAME, backing_store);

    if (lidos != TAM_FRAME) {
        return 0; 
    }

    return 1;

}

int traduzir_endereco(VM *vm, int endereco_logico, FILE *backing_store,
                        int *endereco_fisico, signed char *valor){

    int pagina;
    int deslocamento;
    extrair_pagina_deslocamento(endereco_logico, &pagina, &deslocamento);
    
    vm->relogio++;
    vm->total_enderecos++;

    int frame;

    if (vm->tabela_paginas[pagina].valido) {
        
        frame = vm->tabela_paginas[pagina].frame;
        vm->frames[frame].ultimo_uso = vm->relogio;

    }

    else {

        vm->faltas_pagina++;

        frame = escolher_frame(vm);

        int pagina_antiga = vm->frames[frame].pagina;

        if (pagina_antiga != -1) {
            vm->tabela_paginas[pagina_antiga].valido = 0;
            vm->tabela_paginas[pagina_antiga].frame = -1;
        }

        if (!carregar_pagina_do_disco(vm, pagina, frame, backing_store)) {
            return 0; 
        }

        vm->tabela_paginas[pagina].frame = frame;
        vm->tabela_paginas[pagina].valido = 1;

        vm->frames[frame].pagina = pagina;
        vm->frames[frame].carregado_em = vm->relogio; 
        vm->frames[frame].ultimo_uso = vm->relogio;

    }

    *endereco_fisico = frame * TAM_FRAME + deslocamento;
    *valor = vm->ram[frame][deslocamento];
    return 1;

}