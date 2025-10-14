#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ITENS 20
#define TAM_NOME 30
#define TAM_TIPO 20

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int quantidade;
    int prioridade; // 1 a 5
} Item;

typedef enum {
    POR_NOME = 1,
    POR_TIPO,
    POR_PRIORIDADE
} Criterio;

// Função para ler strings com segurança
void ler_string(char *buffer, int tamanho) {
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

// Listar todos os itens
void listar_itens(Item mochila[], int qtd) {
    printf("\n---- Mochila (%d itens) ----\n", qtd);
    if (qtd == 0) {
        printf("Mochila vazia.\n");
        return;
    }
    printf("%-3s | %-30s | %-20s | %-10s | %-8s\n", "No", "Nome", "Tipo", "Quantidade", "Prioridade");
    printf("----+--------------------------------+----------------------+-----------+----------\n");
    for (int i = 0; i < qtd; i++) {
        printf("%-3d | %-30s | %-20s | %-10d | %-8d\n", i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
    }
}

// Busca sequencial por nome
int busca_sequencial(Item mochila[], int qtd, char nome_busca[]) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(mochila[i].nome, nome_busca) == 0) {
            return i;
        }
    }
    return -1;
}

// Insertion Sort com contagem de comparações
void ordenar_itens(Item mochila[], int qtd, Criterio crit, int *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < qtd; i++) {
        Item chave = mochila[i];
        int j = i - 1;
        bool cond;
        do {
            if (crit == POR_NOME)
                cond = strcmp(mochila[j].nome, chave.nome) > 0;
            else if (crit == POR_TIPO)
                cond = strcmp(mochila[j].tipo, chave.tipo) > 0;
            else // POR_PRIORIDADE
                cond = mochila[j].prioridade > chave.prioridade;

            (*comparacoes)++;
            if (cond) {
                mochila[j + 1] = mochila[j];
                j--;
            }
        } while (cond && j >= 0);
        mochila[j + 1] = chave;
    }
}

// Busca binária por nome
int busca_binaria(Item mochila[], int qtd, char nome_busca[], int *comparacoes) {
    int esquerda = 0, direita = qtd - 1;
    *comparacoes = 0;
    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        (*comparacoes)++;
        int cmp = strcmp(mochila[meio].nome, nome_busca);
        if (cmp == 0) return meio;
        else if (cmp < 0) esquerda = meio + 1;
        else direita = meio - 1;
    }
    return -1;
}

int main() {
    Item mochila[MAX_ITENS];
    int qtd = 0;
    int opc;

    do {
        printf("\n=== MOCHILA DE AVENTUREIRO / MESTRE ===\n");
        printf("1 - Adicionar item\n");
        printf("2 - Remover item pelo nome\n");
        printf("3 - Listar itens\n");
        printf("4 - Buscar item por nome (Sequencial)\n");
        printf("5 - Ordenar itens\n");
        printf("6 - Buscar item por nome (Binaria, lista deve estar ordenada por nome)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opc) != 1) { while (getchar() != '\n'); opc = -1; }
        getchar(); // limpar buffer

        switch (opc) {
            case 1:
                if (qtd >= MAX_ITENS) {
                    printf("Mochila cheia. Remova algum item primeiro.\n");
                } else {
                    printf("Nome: "); ler_string(mochila[qtd].nome, TAM_NOME);
                    printf("Tipo: "); ler_string(mochila[qtd].tipo, TAM_TIPO);
                    printf("Quantidade: "); scanf("%d", &mochila[qtd].quantidade);
                    printf("Prioridade (1-5): "); scanf("%d", &mochila[qtd].prioridade);
                    getchar();
                    qtd++;
                    printf("Item adicionado!\n");
                }
                break;

            case 2: {
                if (qtd == 0) { printf("Mochila vazia.\n"); break; }
                char nome_remover[TAM_NOME];
                printf("Digite o nome do item a remover: ");
                ler_string(nome_remover, TAM_NOME);
                int idx = busca_sequencial(mochila, qtd, nome_remover);
                if (idx == -1) printf("Item não encontrado.\n");
                else {
                    for (int i = idx; i < qtd - 1; i++) mochila[i] = mochila[i + 1];
                    qtd--;
                    printf("Item removido!\n");
                }
                break;
            }

            case 3:
                listar_itens(mochila, qtd);
                break;

            case 4: {
                if (qtd == 0) { printf("Mochila vazia.\n"); break; }
                char nome_busca[TAM_NOME];
                printf("Digite o nome do item a buscar: ");
                ler_string(nome_busca, TAM_NOME);
                int idx = busca_sequencial(mochila, qtd, nome_busca);
                if (idx == -1) printf("Item '%s' não encontrado.\n", nome_busca);
                else {
                    printf("\nItem encontrado!\nNome: %s\nTipo: %s\nQuantidade: %d\nPrioridade: %d\n",
                        mochila[idx].nome, mochila[idx].tipo, mochila[idx].quantidade, mochila[idx].prioridade);
                }
                break;
            }

            case 5: {
                if (qtd == 0) { printf("Mochila vazia.\n"); break; }
                int criterio;
                printf("Escolha critério de ordenação:\n1-Nome 2-Tipo 3-Prioridade: ");
                scanf("%d", &criterio); getchar();
                if (criterio < 1 || criterio > 3) { printf("Critério inválido.\n"); break; }
                int comparacoes;
                ordenar_itens(mochila, qtd, criterio, &comparacoes);
                printf("Itens ordenados! Comparações realizadas: %d\n", comparacoes);
                listar_itens(mochila, qtd);
                break;
            }

            case 6: {
                if (qtd == 0) { printf("Mochila vazia.\n"); break; }
                char nome_busca[TAM_NOME];
                printf("Digite o nome do item para busca binária: ");
                ler_string(nome_busca, TAM_NOME);
                int comparacoes, idx = busca_binaria(mochila, qtd, nome_busca, &comparacoes);
                if (idx == -1) printf("Item '%s' não encontrado.\n", nome_busca);
                else {
                    printf("\nItem encontrado!\nNome: %s\nTipo: %s\nQuantidade: %d\nPrioridade: %d\n",
                        mochila[idx].nome, mochila[idx].tipo, mochila[idx].quantidade, mochila[idx].prioridade);
                    printf("Comparações realizadas na busca binária: %d\n", comparacoes);
                }
                break;
            }

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opção inválida.\n");
        }
    } while (opc != 0);

    return 0;
}
