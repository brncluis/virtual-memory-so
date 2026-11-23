# Virtual Memory Manager

Gerenciador de memória virtual em C (Designing a Virtual Memory Manager, página P-51 do livro Operating System Concepts, Silberschatz, 10ª edição), com 128 frames, substituição de página e da TLB por FIFO ou LRU e busca na TLB com uma thread por entrada.
Disciplina Infraestrutura de Software (CESAR School).
Desenvolvido por Luis Alves

## Arquivos utilizados

- `vm.h` — constantes (`NUM_PAGINAS`, `NUM_FRAMES`, `TAM_FRAME`, `TAM_TLB`), o enum `Politica` e as structs da tabela de páginas, dos frames, da TLB e da `VM`, além dos protótipos.
- `vm.c` — lógica da memória virtual: inicialização da VM, extração de página e deslocamento, escolha do frame (FIFO ou LRU), leitura da página no `BACKING_STORE.bin`, busca na TLB com threads (`buscar_tlb` e `buscar_entrada_da_tlb`), invalidação e inserção na TLB e tradução do endereço (`traduzir_endereco`).
- `main.c` — validação dos argumentos e do arquivo de endereços, chamada da tradução para cada endereço, escrita do `correct.txt` com as estatísticas finais e o `main`.
- `Makefile` — regra de compilação do executável `vm` e limpeza dos arquivos gerados.
- `evidencias.log` — registro do terminal com a compilação e as execuções de teste.

## Sistema operacional

Desenvolvido e testado em **Ubuntu Linux**, arquitetura `x86_64`.

## Como compilar

```
make
```

Isso gera o executável `vm` a partir de `main.c` e `vm.c`.

Para recompilar do zero:

```
make clean
make
```

## Como executar

```
./vm <arquivo_enderecos> <fifo|lru> <fifo|lru>
```

Exemplo:

```
./vm adresses.txt lru fifo
```

O primeiro argumento é o arquivo de endereços lógicos (um inteiro por linha), o segundo é o algoritmo de substituição da página e o terceiro o algoritmo de substituição da TLB. O `BACKING_STORE.bin` precisa estar no diretório atual. A execução gera o arquivo `correct.txt`, com uma linha por endereço (`Virtual address`, índice da `TLB`, `Physical address` e `Value`) e, no final, o número de endereços traduzidos, page faults, taxa de page fault, TLB hits e taxa de TLB hit.

## Como testar

Compare a saída do programa contra o gabarito do professor, salvo em `correctprofessor.txt` (gerado com lru na página e fifo na TLB):

```
./vm adresses.txt lru fifo
diff correct.txt correctprofessor.txt
```

Sem diferença impressa, a saída bate com o esperado.

Também é possível rodar as 4 combinações e comparar as estatísticas no final do `correct.txt`:

```
./vm adresses.txt fifo fifo
./vm adresses.txt fifo lru
./vm adresses.txt lru fifo
./vm adresses.txt lru lru
tail -5 correct.txt
```

Casos de entrada inválida (número de argumentos diferente de 3, algoritmo diferente de `fifo`/`lru`, arquivo de endereços inexistente, vazio ou com linha que não é um inteiro não negativo, ou `BACKING_STORE.bin` ausente) devem ser rejeitados com mensagem de erro em stderr e código de saída diferente de zero.
