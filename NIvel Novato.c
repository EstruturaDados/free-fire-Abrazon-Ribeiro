#include <stdio.h>
#include <string.h>

#define MAX_ITENS 10
#define TAM_NOME 30
#define TAM_TIPO 20

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int quantidade;
} Item;

void ler_string(char *buffer, int tamanho) {
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // remove \n
}

void listar_itens(Item mochila[], int qtd) {
    printf("\n---- Inventario (%d itens) ----\n", qtd);
    if (qtd == 0) { 
        printf("Mochila vazia.\n"); 
        return; 
    }
    printf("%-3s | %-30s | %-20s | %-10s\n", "No", "Nome", "Tipo", "Quantidade");
    printf("----+--------------------------------+----------------------+-----------\n");
    for (int i = 0; i < qtd; i++) {
        printf("%-3d | %-30s | %-20s | %-10d\n", i+1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
    }
}

int main() {
    Item mochila[MAX_ITENS];
    int qtd = 0;
    int opc;

    do {
        printf("\n==== MOCHILA DE SOBREVIVENCIA CODIGO DA ILHA ====\n");1
        printf("1 - Adicionar item\n");
        printf("2 - Remover item pelo nome\n");
        printf("3 - Listar itens\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &opc) != 1) { 
            while (getchar() != '\n'); 
            opc = -1; 
        }
        getchar(); // remove \n do buffer

        switch(opc) {
            case 1:
                if (qtd >= MAX_ITENS) {
                    printf("Mochila cheia. Remova algum item primeiro.\n");
                } else {
                    printf("Nome: "); ler_string(mochila[qtd].nome, TAM_NOME);
                    printf("Tipo: "); ler_string(mochila[qtd].tipo, TAM_TIPO);
                    printf("Quantidade: "); scanf("%d", &mochila[qtd].quantidade);
                    getchar(); // limpar buffer
                    qtd++;
                    printf("Item adicionado!\n");
                }
                break;
            case 2:
                if (qtd == 0) { 
                    printf("Mochila vazia.\n"); 
                    break; 
                }
                char nome_remover[TAM_NOME];
                printf("Digite o nome do item a remover: "); 
                ler_string(nome_remover, TAM_NOME);
                int idx = -1;
                for (int i = 0; i < qtd; i++) {
                    if (strcmp(mochila[i].nome, nome_remover) == 0) {
                        idx = i; break;
                    }
                }
                if (idx == -1) {
                    printf("Item nao encontrado.\n");
                } else {
                    for (int i = idx; i < qtd - 1; i++) mochila[i] = mochila[i+1];
                    qtd--;
                    printf("Item removido!\n");
                }
                break;
            case 3:
                listar_itens(mochila, qtd);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opc != 0);

    return 0;
}
