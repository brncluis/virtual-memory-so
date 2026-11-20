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
