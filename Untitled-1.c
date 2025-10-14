/*
 * jogo_inventario_completo.c
 *
 * Integração final: três módulos do desafio
 *  1) Inventário simples (vetor fixo, até 10 itens)
 *  2) Comparativo: Mochila com Vetor dinâmico vs Lista encadeada
 *     - inserção, remoção, listagem, busca sequencial
 *     - ordenação (vetor) + busca binária
 *     - contadores de comparações e tempos
 *  3) Torre de fuga: gerenciamento de componentes (até 20)
 *     - ordenações: Bubble (nome), Insertion (tipo), Selection (prioridade)
 *     - busca binária por nome (após ordenação por nome)
 *
 * Compilar:
 * gcc -std=c11 -O2 -Wall -Wextra -o jogo_inventario_completo jogo_inventario_completo.c
 *
 * Observações:
 *  - Entradas textuais usam fgets via ler_linha_trim para segurança.
 *  - Evitei mistura perigosa scanf/fgets ao limpar buffers corretamente.
 *  - Cada módulo tem menu próprio; o menu mestre integra tudo.
 *
 * Autor: Integrado para Abrazon — código direto ao ponto, com humor contido.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----------------------
   Constantes e utilitários
   ---------------------- */
#define TAM_NOME 30
#define TAM_TIPO 20

/* Safe read line and trim newline */
void ler_linha_trim(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

/* Limpa restante do stdin após scanf quando necessário */
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Imprime separador */
void separador() { printf("\n----------------------------------------\n"); }

/* ===========================
   MÓDULO 1: Inventário simples
   (vetor fixo até 10 itens)
   =========================== */
#define MOD1_MAX 10

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int quantidade;
} ItemSimple;

void mod1_inserir(ItemSimple mochila[], int *qtd) {
    if (*qtd >= MOD1_MAX) {
        printf("Mochila cheia (max %d). Remova algum item primeiro.\n", MOD1_MAX);
        return;
    }
    ItemSimple it;
    printf("Nome: "); ler_linha_trim(it.nome, TAM_NOME);
    printf("Tipo: "); ler_linha_trim(it.tipo, TAM_TIPO);
    printf("Quantidade: ");
    if (scanf("%d", &it.quantidade) != 1) { limpar_buffer(); printf("Entrada invalida.\n"); return; }
    limpar_buffer();
    mochila[*qtd] = it;
    (*qtd)++;
    printf("Item adicionado.\n");
}

void mod1_remover(ItemSimple mochila[], int *qtd) {
    if (*qtd == 0) { printf("Mochila vazia.\n"); return; }
    char nome[TAM_NOME];
    printf("Nome do item a remover: "); ler_linha_trim(nome, TAM_NOME);
    int idx = -1;
    for (int i = 0; i < *qtd; ++i) if (strcmp(mochila[i].nome, nome) == 0) { idx = i; break; }
    if (idx == -1) { printf("Item '%s' nao encontrado.\n", nome); return; }
    for (int j = idx; j < *qtd - 1; ++j) mochila[j] = mochila[j+1];
    (*qtd)--;
    printf("Item removido.\n");
}

void mod1_listar(ItemSimple mochila[], int qtd) {
    separador();
    printf("Inventario simples (%d items):\n", qtd);
    if (qtd == 0) { printf("Vazio.\n"); return; }
    for (int i = 0; i < qtd; ++i)
        printf("%d) Nome: %s | Tipo: %s | Quantidade: %d\n",
               i+1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
}

void modulo1() {
    ItemSimple mochila[MOD1_MAX];
    int qtd = 0;
    int opc;
    do {
        separador();
        printf("MÓDULO 1 - Inventario simples (vetor fixo até %d)\n", MOD1_MAX);
        printf("1- Adicionar  2- Remover  3- Listar  0- Voltar\nEscolha: ");
        if (scanf("%d", &opc) != 1) { limpar_buffer(); opc = -1; }
        limpar_buffer();
        switch (opc) {
            case 1: mod1_inserir(mochila, &qtd); mod1_listar(mochila, qtd); break;
            case 2: mod1_remover(mochila, &qtd); mod1_listar(mochila, qtd); break;
            case 3: mod1_listar(mochila, qtd); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while (opc != 0);
}

/* =========================================
   MÓDULO 2: Comparativo Vetor Dinâmico x Lista
   ========================================= */

/* Tipos para o módulo 2 */
typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int quantidade;
} Item2;

/* LISTA ENC. NODE */
typedef struct No {
    Item2 dados;
    struct No *proximo;
} No;

/* VETOR DINAMICO */
typedef struct {
    Item2 *itens;
    int tamanho;
    int capacidade;
    int ordenado; /* flag: ordenado por nome? */
} VetorDyn;

/* Contadores de comparações (globais do modulo2) */
long long comp_seq_vetor = 0;
long long comp_bin_vetor = 0;
long long comp_seq_lista = 0;

/* Inicializa vetor dinâmico */
void vetor_init(VetorDyn *v) {
    v->capacidade = 8;
    v->itens = malloc(sizeof(Item2) * v->capacidade);
    if (!v->itens) { fprintf(stderr, "Memoria insuficiente\n"); exit(1); }
    v->tamanho = 0;
    v->ordenado = 0;
}

/* Garante capacidade */
void vetor_garantir(VetorDyn *v, int ncap) {
    if (v->capacidade >= ncap) return;
    int cap = v->capacidade;
    while (cap < ncap) cap *= 2;
    Item2 *novo = realloc(v->itens, sizeof(Item2) * cap);
    if (!novo) { fprintf(stderr, "Realloc falhou\n"); exit(1); }
    v->itens = novo; v->capacidade = cap;
}

/* Inserir no vetor (append) */
void vetor_inserir(VetorDyn *v) {
    Item2 it;
    printf("[VETOR] Nome: "); ler_linha_trim(it.nome, TAM_NOME);
    printf("[VETOR] Tipo: "); ler_linha_trim(it.tipo, TAM_TIPO);
    printf("[VETOR] Quantidade: "); if (scanf("%d", &it.quantidade) != 1) { limpar_buffer(); printf("Entrada invalida.\n"); return; }
    limpar_buffer();
    vetor_garantir(v, v->tamanho + 1);
    v->itens[v->tamanho++] = it;
    v->ordenado = 0;
    printf("[VETOR] Item adicionado. Tamanho agora: %d\n", v->tamanho);
}

/* Remover do vetor por nome (procura sequencial) */
void vetor_remover(VetorDyn *v) {
    if (v->tamanho == 0) { printf("[VETOR] Vazio.\n"); return; }
    char nome[TAM_NOME]; printf("[VETOR] Nome a remover: "); ler_linha_trim(nome, TAM_NOME);
    int idx = -1;
    comp_seq_vetor = 0;
    for (int i = 0; i < v->tamanho; ++i) {
        comp_seq_vetor++;
        if (strcmp(v->itens[i].nome, nome) == 0) { idx = i; break; }
    }
    if (idx == -1) { printf("[VETOR] Nao encontrado. Comparacoes: %lld\n", comp_seq_vetor); return; }
    for (int j = idx; j < v->tamanho - 1; ++j) v->itens[j] = v->itens[j+1];
    v->tamanho--;
    printf("[VETOR] Removido. Comparacoes: %lld\n", comp_seq_vetor);
}

/* Listar vetor */
void vetor_listar(VetorDyn *v) {
    separador(); printf("[VETOR] Itens (%d)\n", v->tamanho);
    if (v->tamanho == 0) { printf("Vazio.\n"); return; }
    for (int i = 0; i < v->tamanho; ++i)
        printf("%d) Nome: %s | Tipo: %s | Qt: %d\n",
               i+1, v->itens[i].nome, v->itens[i].tipo, v->itens[i].quantidade);
}

/* Selection Sort por nome (simples) para o vetor (usado para demonstracao/ordenar) */
void vetor_ordenar_nome(VetorDyn *v) {
    if (v->tamanho < 2) { v->ordenado = 1; printf("[VETOR] Nada a ordenar.\n"); return; }
    clock_t t0 = clock();
    for (int i = 0; i < v->tamanho - 1; ++i) {
        int menor = i;
        for (int j = i + 1; j < v->tamanho; ++j) {
            if (strcmp(v->itens[j].nome, v->itens[menor].nome) < 0) menor = j;
        }
        if (menor != i) {
            Item2 tmp = v->itens[i]; v->itens[i] = v->itens[menor]; v->itens[menor] = tmp;
        }
    }
    clock_t t1 = clock();
    v->ordenado = 1;
    double ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    printf("[VETOR] Ordenado por nome (Selection). Tempo: %.3f ms\n", ms);
}

/* Busca sequencial no vetor (mede comparacoes e tempo) */
void vetor_busca_sequencial(VetorDyn *v) {
    if (v->tamanho == 0) { printf("[VETOR] Vazio.\n"); return; }
    char nome[TAM_NOME]; printf("[VETOR] Nome pra buscar (sequencial): "); ler_linha_trim(nome, TAM_NOME);
    comp_seq_vetor = 0;
    clock_t t0 = clock();
    int idx = -1;
    for (int i = 0; i < v->tamanho; ++i) {
        comp_seq_vetor++;
        if (strcmp(v->itens[i].nome, nome) == 0) { idx = i; break; }
    }
    clock_t t1 = clock(); double ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    if (idx == -1) printf("[VETOR] Nao encontrado. Comparacoes: %lld | Tempo: %.3f ms\n", comp_seq_vetor, ms);
    else { printf("[VETOR] Encontrado idx %d. Comparacoes: %lld | Tempo: %.3f ms\n", idx, comp_seq_vetor, ms); printf("Detalhe: Nome:%s Tipo:%s Qt:%d\n", v->itens[idx].nome, v->itens[idx].tipo, v->itens[idx].quantidade); }
}

/* Busca binaria no vetor (apos ordenacao por nome) */
void vetor_busca_binaria(VetorDyn *v) {
    if (v->tamanho == 0) { printf("[VETOR] Vazio.\n"); return; }
    if (!v->ordenado) {
        printf("[VETOR] Vetor nao ordenado. Deseja ordenar agora? (s/n): ");
        char r[4]; ler_linha_trim(r, 4);
        if (r[0] == 's' || r[0] == 'S') vetor_ordenar_nome(v);
        else { printf("[VETOR] Abortando busca binaria.\n"); return; }
    }
    char nome[TAM_NOME]; printf("[VETOR] Nome pra buscar (binaria): "); ler_linha_trim(nome, TAM_NOME);
    comp_bin_vetor = 0;
    int l = 0, r = v->tamanho - 1, idx = -1;
    clock_t t0 = clock();
    while (l <= r) {
        int mid = l + (r - l) / 2;
        comp_bin_vetor++;
        int cmp = strcmp(v->itens[mid].nome, nome);
        if (cmp == 0) { idx = mid; break; }
        else if (cmp < 0) l = mid + 1;
        else r = mid - 1;
    }
    clock_t t1 = clock(); double ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    if (idx == -1) printf("[VETOR] Nao encontrado. Comparacoes: %lld | Tempo: %.3f ms\n", comp_bin_vetor, ms);
    else printf("[VETOR] Encontrado idx %d. Comparacoes: %lld | Tempo: %.3f ms\n", idx, comp_bin_vetor, ms);
}

/* LISTA: inserir no inicio */
void lista_inserir(No **head) {
    Item2 it;
    printf("[LISTA] Nome: "); ler_linha_trim(it.nome, TAM_NOME);
    printf("[LISTA] Tipo: "); ler_linha_trim(it.tipo, TAM_TIPO);
    printf("[LISTA] Quantidade: "); if (scanf("%d", &it.quantidade) != 1) { limpar_buffer(); printf("Entrada invalida.\n"); return; }
    limpar_buffer();
    No *n = malloc(sizeof(No));
    if (!n) { fprintf(stderr, "Memoria insuficiente\n"); exit(1); }
    n->dados = it; n->proximo = *head; *head = n;
    printf("[LISTA] Item inserido no inicio.\n");
}

/* LISTA: remover por nome */
void lista_remover(No **head) {
    if (*head == NULL) { printf("[LISTA] Vazia.\n"); return; }
    char nome[TAM_NOME]; printf("[LISTA] Nome a remover: "); ler_linha_trim(nome, TAM_NOME);
    No *cur = *head, *prev = NULL;
    comp_seq_lista = 0;
    while (cur) {
        comp_seq_lista++;
        if (strcmp(cur->dados.nome, nome) == 0) {
            if (prev) prev->proximo = cur->proximo; else *head = cur->proximo;
            free(cur);
            printf("[LISTA] Removido. Comparacoes: %lld\n", comp_seq_lista);
            return;
        }
        prev = cur; cur = cur->proximo;
    }
    printf("[LISTA] Nao encontrado. Comparacoes: %lld\n", comp_seq_lista);
}

/* LISTA: listar */
void lista_listar(No *head) {
    separador(); printf("[LISTA] Itens:\n");
    if (!head) { printf("Vazia.\n"); return; }
    int i = 1;
    for (No *p = head; p; p = p->proximo) {
        printf("%d) Nome: %s | Tipo: %s | Qt: %d\n", i++, p->dados.nome, p->dados.tipo, p->dados.quantidade);
    }
}

/* LISTA: busca sequencial */
void lista_busca_sequencial(No *head) {
    if (!head) { printf("[LISTA] Vazia.\n"); return; }
    char nome[TAM_NOME]; printf("[LISTA] Nome pra buscar: "); ler_linha_trim(nome, TAM_NOME);
    comp_seq_lista = 0;
    clock_t t0 = clock();
    int pos = 0;
    for (No *p = head; p; p = p->proximo) {
        comp_seq_lista++; pos++;
        if (strcmp(p->dados.nome, nome) == 0) {
            clock_t t1 = clock(); double ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
            printf("[LISTA] Encontrado pos %d. Comparacoes: %lld | Tempo: %.3f ms\n", pos-1, comp_seq_lista, ms);
            printf("Detalhe: Nome:%s Tipo:%s Qt:%d\n", p->dados.nome, p->dados.tipo, p->dados.quantidade);
            return;
        }
    }
    clock_t t1 = clock(); double ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    printf("[LISTA] Nao encontrado. Comparacoes: %lld | Tempo: %.3f ms\n", comp_seq_lista, ms);
}

/* Lista liberar */
void lista_liberar(No **head) {
    No *cur = *head;
    while (cur) { No *tmp = cur; cur = cur->proximo; free(tmp); }
    *head = NULL;
}

/* Menu do módulo 2 */
void modulo2() {
    VetorDyn v; vetor_init(&v);
    No *head = NULL;
    int opc;
    do {
        separador();
        printf("MÓDULO 2 - Comparativo Vetor Dinamico x Lista Encadeada\n");
        printf("1- Inserir (vetor)  2- Remover (vetor)  3- Listar (vetor)\n");
        printf("4- Busca sequencial (vetor) 5- Ordenar por nome (vetor) 6- Busca binaria (vetor)\n");
        printf("7- Inserir (lista) 8- Remover (lista) 9- Listar (lista)\n");
        printf("10- Busca sequencial (lista)\n");
        printf("11- Mostrar contadores (comparacoes)\n");
        printf("0- Voltar\nEscolha: ");
        if (scanf("%d", &opc) != 1) { limpar_buffer(); opc = -1; }
        limpar_buffer();
        switch (opc) {
            case 1: vetor_inserir(&v); break;
            case 2: vetor_remover(&v); break;
            case 3: vetor_listar(&v); break;
            case 4: vetor_busca_sequencial(&v); break;
            case 5: vetor_ordenar_nome(&v); break;
            case 6: vetor_busca_binaria(&v); break;
            case 7: lista_inserir(&head); break;
            case 8: lista_remover(&head); break;
            case 9: lista_listar(head); break;
            case 10: lista_busca_sequencial(head); break;
            case 11:
                printf("\nComparacoes (vetor - sequencial): %lld\n", comp_seq_vetor);
                printf("Comparacoes (vetor - binaria): %lld\n", comp_bin_vetor);
                printf("Comparacoes (lista - sequencial): %lld\n", comp_seq_lista);
                break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while (opc != 0);

    free(v.itens);
    lista_liberar(&head);
}

/* =====================================
   MÓDULO 3: Torre de Fuga (componentes)
   - até 20 componentes
   - bubble por nome, insertion por tipo, selection por prioridade
   - busca binária por nome (após ordenar por nome)
   ===================================== */
#define MOD3_MAX 20

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int prioridade; /* 1..10 */
} Componente;

void mod3_mostrar(Componente comps[], int n) {
    separador();
    printf("Componentes (%d):\n", n);
    if (n == 0) { printf("Nenhum componente.\n"); return; }
    printf("%-3s | %-28s | %-14s | %-9s\n", "No", "Nome", "Tipo", "Prioridade");
    printf("----+------------------------------+----------------+-----------\n");
    for (int i = 0; i < n; ++i)
        printf("%-3d | %-28s | %-14s | %-9d\n", i+1, comps[i].nome, comps[i].tipo, comps[i].prioridade);
}

/* Troca util */
void mod3_trocar(Componente *a, Componente *b) { Componente t = *a; *a = *b; *b = t; }

/* Bubble sort por nome */
long long mod3_bubble_por_nome(Componente comps[], int n, double *tms) {
    long long comps_count = 0;
    clock_t t0 = clock();
    for (int i = 0; i < n - 1; ++i) {
        int trocou = 0;
        for (int j = 0; j < n - 1 - i; ++j) {
            comps_count++;
            if (strcmp(comps[j].nome, comps[j+1].nome) > 0) {
                mod3_trocar(&comps[j], &comps[j+1]);
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
    clock_t t1 = clock(); if (tms) *tms = (double)(t1 - t0)*1000.0/CLOCKS_PER_SEC;
    return comps_count;
}

/* Insertion sort por tipo */
long long mod3_insertion_por_tipo(Componente comps[], int n, double *tms) {
    long long comps_count = 0;
    clock_t t0 = clock();
    for (int i = 1; i < n; ++i) {
        Componente chave = comps[i];
        int j = i - 1;
        while (j >= 0) {
            comps_count++;
            if (strcmp(comps[j].tipo, chave.tipo) > 0) {
                comps[j+1] = comps[j];
                j--;
            } else break;
        }
        comps[j+1] = chave;
    }
    clock_t t1 = clock(); if (tms) *tms = (double)(t1 - t0)*1000.0/CLOCKS_PER_SEC;
    return comps_count;
}

/* Selection sort por prioridade (maior->menor) */
long long mod3_selection_por_prioridade(Componente comps[], int n, double *tms) {
    long long comps_count = 0;
    clock_t t0 = clock();
    for (int i = 0; i < n - 1; ++i) {
        int idx_maior = i;
        for (int j = i + 1; j < n; ++j) {
            comps_count++;
            if (comps[j].prioridade > comps[idx_maior].prioridade) idx_maior = j;
        }
        if (idx_maior != i) mod3_trocar(&comps[i], &comps[idx_maior]);
    }
    clock_t t1 = clock(); if (tms) *tms = (double)(t1 - t0)*1000.0/CLOCKS_PER_SEC;
    return comps_count;
}

/* Busca binaria por nome (vetor ordenado por nome asc) */
int mod3_busca_binaria_por_nome(Componente comps[], int n, const char *nome, long long *comparacoes, double *tms) {
    *comparacoes = 0;
    int l = 0, r = n - 1, idx = -1;
    clock_t t0 = clock();
    while (l <= r) {
        int mid = l + (r - l) / 2;
        (*comparacoes)++;
        int cmp = strcmp(comps[mid].nome, nome);
        if (cmp == 0) { idx = mid; break; }
        else if (cmp < 0) l = mid + 1;
        else r = mid - 1;
    }
    clock_t t1 = clock();
    if (tms) *tms = (double)(t1 - t0)*1000.0/CLOCKS_PER_SEC;
    return idx;
}

void modulo3() {
    Componente comps[MOD3_MAX];
    int n = 0;
    int ordenado_por_nome = 0;

    int opc;
    do {
        separador();
        printf("MÓDULO 3 - Torre de Fuga (componentes, max %d)\n", MOD3_MAX);
        printf("1- Cadastrar  2- Listar  3- Bubble por NOME  4- Insertion por TIPO\n");
        printf("5- Selection por PRIORIDADE  6- Busca binaria por NOME  7- Confirmar componente-chave\n");
        printf("0- Voltar\nEscolha: ");
        if (scanf("%d", &opc) != 1) { limpar_buffer(); opc = -1; }
        limpar_buffer();
        if (opc == 1) {
            if (n >= MOD3_MAX) { printf("Capacidade atingida. Remova antes.\n"); continue; }
            Componente c;
            printf("Nome: "); ler_linha_trim(c.nome, TAM_NOME);
            printf("Tipo: "); ler_linha_trim(c.tipo, TAM_TIPO);
            printf("Prioridade (1..10): ");
            int p; if (scanf("%d", &p) != 1) { limpar_buffer(); printf("Entrada invalida.\n"); continue; }
            limpar_buffer(); if (p < 1) p = 1; if (p > 10) p = 10;
            c.prioridade = p;
            comps[n++] = c;
            ordenado_por_nome = 0;
            printf("Componente cadastrado.\n");
            mod3_mostrar(comps, n);
        } else if (opc == 2) mod3_mostrar(comps, n);
        else if (opc == 3) {
            double tm = 0; long long comps_count = mod3_bubble_por_nome(comps, n, &tm);
            ordenado_por_nome = 1;
            printf("[Bubble por NOME] Comparacoes: %lld | Tempo: %.3f ms\n", comps_count, tm);
            mod3_mostrar(comps, n);
        } else if (opc == 4) {
            double tm = 0; long long comps_count = mod3_insertion_por_tipo(comps, n, &tm);
            ordenado_por_nome = 0;
            printf("[Insertion por TIPO] Comparacoes: %lld | Tempo: %.3f ms\n", comps_count, tm);
            mod3_mostrar(comps, n);
        } else if (opc == 5) {
            double tm = 0; long long comps_count = mod3_selection_por_prioridade(comps, n, &tm);
            ordenado_por_nome = 0;
            printf("[Selection por PRIORIDADE] Comparacoes: %lld | Tempo: %.3f ms\n", comps_count, tm);
            mod3_mostrar(comps, n);
        } else if (opc == 6) {
            if (!ordenado_por_nome) {
                printf("Vetor precisa estar ordenado por NOME para busca binaria. Ordenar agora? (s/n): ");
                char r[4]; ler_linha_trim(r, 4);
                if (r[0] == 's' || r[0] == 'S') { double tm=0; mod3_bubble_por_nome(comps, n, &tm); ordenado_por_nome = 1; printf("[Ordenado automaticamente] Tempo: %.3f ms\n", tm); }
                else { printf("Busca binaria cancelada.\n"); continue; }
            }
            char chave[TAM_NOME]; printf("Nome do componente para buscar (binaria): "); ler_linha_trim(chave, TAM_NOME);
            long long comps_count=0; double tm=0;
            int idx = mod3_busca_binaria_por_nome(comps, n, chave, &comps_count, &tm);
            if (idx == -1) printf("NAO encontrado. Comparacoes: %lld | Tempo: %.3f ms\n", comps_count, tm);
            else { printf("Encontrado idx %d. Comparacoes: %lld | Tempo: %.3f ms\n", idx, comps_count, tm); printf("Detalhe: Nome:%s Tipo:%s Prioridade:%d\n", comps[idx].nome, comps[idx].tipo, comps[idx].prioridade); }
        } else if (opc == 7) {
            if (n == 0) { printf("Nenhum componente cadastrado.\n"); continue; }
            char chave[TAM_NOME]; printf("Nome do componente-chave: "); ler_linha_trim(chave, TAM_NOME);
            long long comps_count = 0; clock_t t0 = clock(); int found = -1;
            for (int i = 0; i < n; ++i) { comps_count++; if (strcmp(comps[i].nome, chave) == 0) { found = i; break; } }
            clock_t t1 = clock(); double tm = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
            if (found == -1) printf("Componente-chave NAO presente. Comparacoes: %lld | Tempo: %.3f ms\n", comps_count, tm);
            else printf("Componente-chave presente idx %d. Comparacoes: %lld | Tempo: %.3f ms\n", found, comps_count, tm);
        } else if (opc == 0) break;
        else printf("Opcao invalida.\n");
    } while (1);
}

/* ---------------------------
   MENU MESTRE (integra os 3 módulos)
   --------------------------- */
int main(void) {
    printf("=== JOGO: Sistema Integrado de Inventario e Prioridades ===\n");
    int opc;
    do {
        separador();
        printf("Escolha o modulo:\n1 - Inventario simples (vetor fixo)\n2 - Comparativo (vetor dinamico x lista)\n3 - Torre de fuga (ordenacoes e busca)\n0 - Sair\nOpcao: ");
        if (scanf("%d", &opc) != 1) { limpar_buffer(); opc = -1; }
        limpar_buffer();
        switch (opc) {
            case 1: modulo1(); break;
            case 2: modulo2(); break;
            case 3: modulo3(); break;
            case 0: printf("Saindo. Boa sorte no jogo — e evite a zona vermelha!\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opc != 0);
    return 0;
}