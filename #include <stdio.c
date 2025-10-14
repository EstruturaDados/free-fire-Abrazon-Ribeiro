#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ITENS 10   // Capacidade máxima da mochila
#define TAM_NOME 30
#define TAM_TIPO 20

// ================================
// Estrutura que representa um item
// ================================
typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int quantidade;
} Item;

// ================================
// Protótipos das funções
// ================================
void inserirItem(Item mochila[], int *qtd);
void removerItem(Item mochila[], int *qtd);
void listarItens(Item mochila[], int qtd);
void buscarItem(Item mochila[], int qtd);

// ================================
// Função principal (menu do jogo)
// ================================
int main() {
    Item mochila[MAX_ITENS];
    int qtd = 0;   // número atual de itens
    int opcao;

    do {
        printf("\n======= MOCHILA DE LOOT =======\n");
        printf("1 - Adicionar item\n");
        printf("2 - Remover item\n");
        printf("3 - Listar itens\n");
        printf("4 - Buscar item\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // limpa o buffer do '\n'

        switch(opcao) {
            case 1:
                inserirItem(mochila, &qtd);
                break;
            case 2:
                removerItem(mochila, &qtd);
                break;
            case 3:
                listarItens(mochila, qtd);
                break;
            case 4:
                buscarItem(mochila, qtd);
                break;
            case 0:
                printf("\nSaindo do sistema... Boa sorte na sua jornada!\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
        }
    } while(opcao != 0);

    return 0;
}

// ================================
// Função: inserirItem()
// Cadastra um novo item na mochila
// ================================
void inserirItem(Item mochila[], int *qtd) {
    if (*qtd >= MAX_ITENS) {
        printf("\nMochila cheia! Nao e possivel adicionar mais itens.\n");
        return;
    }

    Item novo;

    printf("\nDigite o nome do item: ");
    fgets(novo.nome, TAM_NOME, stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0'; // remove \n

    printf("Digite o tipo do item (arma, municao, cura, etc.): ");
    fgets(novo.tipo, TAM_TIPO, stdin);
    novo.tipo[strcspn(novo.tipo, "\n")] = '\0';

    printf("Digite a quantidade: ");
    scanf("%d", &novo.quantidade);
    getchar();

    mochila[*qtd] = novo;
    (*qtd)++;

    printf("\n✅ Item adicionado com sucesso!\n");
    listarItens(mochila, *qtd);
}

// ================================
// Função: removerItem()
// Remove item pelo nome informado
// ================================
void removerItem(Item mochila[], int *qtd) {
    if (*qtd == 0) {
        printf("\nA mochila esta vazia!\n");
        return;
    }

    char nomeBusca[TAM_NOME];
    printf("\nDigite o nome do item a remover: ");
    fgets(nomeBusca, TAM_NOME, stdin);
    nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

    int encontrado = -1;
    for (int i = 0; i < *qtd; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            encontrado = i;
            break;
        }
    }

    if (encontrado != -1) {
        for (int j = encontrado; j < *qtd - 1; j++) {
            mochila[j] = mochila[j + 1];
        }
        (*qtd)--;
        printf("\n🗑️  Item '%s' removido com sucesso!\n", nomeBusca);
    } else {
        printf("\nItem '%s' nao encontrado na mochila.\n", nomeBusca);
    }

    listarItens(mochila, *qtd);
}

// ================================
// Função: listarItens()
// Mostra todos os itens cadastrados
// ================================
void listarItens(Item mochila[], int qtd) {
    printf("\n======= ITENS NA MOCHILA =======\n");
    if (qtd == 0) {
        printf("A mochila esta vazia!\n");
        return;
    }

    for (int i = 0; i < qtd; i++) {
        printf("%d) Nome: %s | Tipo: %s | Quantidade: %d\n",
               i + 1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
    }
}

// ================================
// Função: buscarItem()
// Busca sequencial pelo nome do item
// ================================
void buscarItem(Item mochila[], int qtd) {
    if (qtd == 0) {
        printf("\nA mochila esta vazia!\n");
        return;
    }

    char nomeBusca[TAM_NOME];
    printf("\nDigite o nome do item para buscar: ");
    fgets(nomeBusca, TAM_NOME, stdin);
    nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

    int encontrado = 0;
    for (int i = 0; i < qtd; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            printf("\n🔎 Item encontrado!\n");
            printf("Nome: %s\nTipo: %s\nQuantidade: %d\n",
                   mochila[i].nome, mochila[i].tipo, mochila[i].quantidade);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        printf("\nItem '%s' nao encontrado na mochila.\n", nomeBusca);
    }
}
