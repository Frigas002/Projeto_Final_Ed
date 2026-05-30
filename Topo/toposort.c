#include <stdio.h>
#include <stdlib.h>

/* --- DEFINIÇÃO DAS ESTRUTURAS --- */

typedef struct No {
    int destino;
    struct No* prox;
} No;

typedef struct Grafo {
    int num_vertices;
    No** listas_adjacencia;
    int* in_degree;
} Grafo;


/* --- ASSINATURAS DAS FUNÇÕES --- */

// Função para alocar a memória inicial do grafo
Grafo* criarGrafo(int num_vertices) {
    // TODO: Usar malloc para alocar o Grafo.
    // TODO: Usar malloc para alocar o array listas_adjacencia (tamanho num_vertices).
    // TODO: Usar malloc para alocar o array in_degree (tamanho num_vertices).
    // TODO: Fazer um loop para inicializar todas as listas com NULL e in_degrees com 0.
    // TODO: Retornar o ponteiro do grafo criado.
    return NULL; // Substitua isso depois
}

// Função para adicionar uma conexão (aresta direcionada)
void adicionarAresta(Grafo* grafo, int origem, int destino) {
    // TODO: Criar um novo No usando malloc.
    // TODO: Configurar o 'destino' do novo No.
    // TODO: Inserir esse novo No no início da lista_adjacencia[origem].
    // TODO: Incrementar o in_degree[destino] em 1.
}

// Função principal do Algoritmo de Kahn
void ordenacaoTopologica(Grafo* grafo) {
    // TODO: Criar uma fila (um simples array de inteiros serve) e variáveis de 'inicio' e 'fim'.
    // TODO: Percorrer o array in_degree. Todo vértice com valor 0 deve entrar na fila.
    // TODO: Criar um loop "enquanto a fila não estiver vazia".
    // Dentro do loop:
    //   1. Tirar um vértice da fila (e já pode dar um printf nele, pois essa é a ordem!).
    //   2. Criar um ponteiro auxiliar para percorrer a lista_adjacencia desse vértice.
    //   3. Para cada nó vizinho na lista, diminuir o in_degree dele em 1.
    //   4. Se o in_degree do vizinho chegar a 0, colocá-lo na fila.
}


/* --- PROGRAMA PRINCIPAL --- */

int main() {
    // Exemplo: Criando um grafo com 6 vértices (de 0 a 5)
    int total_vertices = 6;
    Grafo* meuGrafo = criarGrafo(total_vertices);

    // Simulando algumas dependências (exemplo: calçar meias antes do sapato)
    // Se o vértice 5 aponta para o 2, significa que 5 deve vir ANTES de 2.
    adicionarAresta(meuGrafo, 5, 2);
    adicionarAresta(meuGrafo, 5, 0);
    adicionarAresta(meuGrafo, 4, 0);
    adicionarAresta(meuGrafo, 4, 1);
    adicionarAresta(meuGrafo, 2, 3);
    adicionarAresta(meuGrafo, 3, 1);

    printf("A Ordem Topologica encontrada e:\n");
    ordenacaoTopologica(meuGrafo);
    printf("\n");

    // TODO Opcional (mas recomendado!): Criar uma função para dar 'free' em toda a memória alocada.

    return 0;
}