#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100

#define NAO_VISITADO 0
#define VISITANDO 1
#define VISITADO 2


typedef struct {
    int matrizAdj[MAX_VERTICES][MAX_VERTICES];
    int numVertices;
} Grafo;

typedef struct {
    int dados[MAX_VERTICES];
    int topo;
} Pilha;

typedef struct {
    int dados[MAX_VERTICES];
    int frente;
    int tras;
} Fila;

void initPilha(Pilha *p) { p->topo = -1; }
void push(Pilha *p, int val) { p->dados[++(p->topo)] = val; }
int pop(Pilha *p) { return p->dados[(p->topo)--]; }
int pilhaVazia(Pilha *p) { return p->topo == -1; }

void initFila(Fila *f) { f->frente = 0; f->tras = 0; }
void enfileirar(Fila *f, int val) { f->dados[f->tras++] = val; }
int desenfileirar(Fila *f) { return f->dados[f->frente++]; }
int filaVazia(Fila *f) { return f->frente == f->tras; }

//DFS

int dfsAux(Grafo *g, int u, int estado[], Pilha *p) {
    if (estado[u] == VISITANDO) return 0; // Encontrou um ciclo
    if (estado[u] == VISITADO) return 1;

    estado[u] = VISITANDO;

    for (int v = 0; v < g->numVertices; v++) {
        if (g->matrizAdj[u][v] == 1) {
            if (!dfsAux(g, v, estado, p)) return 0;
        }
    }

    estado[u] = VISITADO;
    push(p, u); // O nó entra na pilha no backtracking (pós-fixado)
    return 1;
}

void ordenacaoTopologicaDFS(Grafo *g) {
    Pilha p;
    initPilha(&p);
    int estado[MAX_VERTICES];
    
    for (int i = 0; i < g->numVertices; i++) estado[i] = NAO_VISITADO;

    for (int i = 0; i < g->numVertices; i++) {
        if (estado[i] == NAO_VISITADO) {
            if (!dfsAux(g, i, estado, &p)) {
                printf("\n[DFS] ERRO: Grafo possui um CICLO! Impossivel ordenar.\n");
                return;
            }
        }
    }

    printf("Ordem Topologica (DFS + Pilha): ");
    while (!pilhaVazia(&p)) {
        printf("%d ", pop(&p));
    }
    printf("\n");
}

//metodo de kahn

void ordenacaoTopologicaKahn(Grafo *g) {
    int grauEntrada[MAX_VERTICES] = {0};
    int ordem[MAX_VERTICES];
    int visitadosCont = 0;
    Fila f;
    initFila(&f);

// calcula o grau de entrada (in-degree) de cada vértice
    for (int u = 0; u < g->numVertices; u++) {
        for (int v = 0; v < g->numVertices; v++) {
            if (g->matrizAdj[u][v] == 1) {
                grauEntrada[v]++;
            }
        }
    }

    for (int i = 0; i < g->numVertices; i++) {
        if (grauEntrada[i] == 0) {
            enfileirar(&f, i);
        }
    }

    while (!filaVazia(&f)) {
        int u = desenfileirar(&f);
        ordem[visitadosCont++] = u; // Adiciona à ordenação final

        // Reduz o grau de entrada de todos os vizinhos de 'u'
        for (int v = 0; v < g->numVertices; v++) {
            if (g->matrizAdj[u][v] == 1) {
                grauEntrada[v]--;
                // Se o grau do vizinho zerou, ele está livre para ser executado
                if (grauEntrada[v] == 0) {
                    enfileirar(&f, v);
                }
            }
        }
    }

    //Se não visitou todos os nós, significa que há um ciclo
    if (visitadosCont != g->numVertices) {
        printf("\n[Kahn] ERRO: Grafo possui um CICLO! Impossivel ordenar.\n");
        return;
    }

    // Imprime o resultado obtido
    printf("Ordem Topologica (Kahn + Fila):  ");
    for (int i = 0; i < visitadosCont; i++) {
        printf("%d ", ordem[i]);
    }
    printf("\n");
}

int main() {
    Grafo g;
    g.numVertices = 6;

    // Inicializa a matriz com zeros
    for (int i = 0; i < g.numVertices; i++) {
        for (int j = 0; j < g.numVertices; j++) g.matrizAdj[i][j] = 0;
    }

    // Criando as arestas direcionadas do DAG de exemplo:
    // 5 -> 2, 5 -> 0, 4 -> 0, 4 -> 1, 2 -> 3, 3 -> 1
    g.matrizAdj[5][2] = 1;
    g.matrizAdj[5][0] = 1;
    g.matrizAdj[4][0] = 1;
    g.matrizAdj[4][1] = 1;
    g.matrizAdj[2][3] = 1;
    g.matrizAdj[3][1] = 1;

    printf("TESTE 1: GRAFO ACICLICO DIRECIONADO (DAG)\n");
    ordenacaoTopologicaDFS(&g);
    ordenacaoTopologicaKahn(&g);

    printf("\nTESTE 2: INTRODUZINDO UM CICLO (1 -> 4 e 4 -> 1)\n");
    g.matrizAdj[1][4] = 1;
    
    ordenacaoTopologicaDFS(&g);
    ordenacaoTopologicaKahn(&g);

    return 0;
}