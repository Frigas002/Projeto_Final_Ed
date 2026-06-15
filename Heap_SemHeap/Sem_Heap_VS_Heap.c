#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 10000

/* 1. File de Prioridade Sem Heap
 * mantem o array sempre ordenado
 * remoçao rapida, insercao fica bem lenta em grandes volumes O(n) */
typedef struct{
    int size;               
    int data[MAX_SIZE];     
} Sem_Heap_pq;

// inicializa
void init_Sem_Heap(Sem_Heap_pq *pq){
    pq->size = 0;
}

/* Enqueue O(n)
 * Move para direita do maior p menor ate encontrar seu devido lugar de prioridade
 * Do final do array voltando */
void enqueue_Sem_Heap(Sem_Heap_pq *pq, int item, int *comparacoes){
    int i = pq->size - 1;
    while (i >= 0){
        (*comparacoes)++; // p contar no final
        if(pq->data[i] > item){
            pq->data[i + 1] = pq->data[i];
            i--;
        } else {
            break;
        }
    }
    //  posiçao correta bota e aumenta a fila
    pq->data[i + 1] = item;
    pq->size++;
}

/* Dequeue O(1)
 * Visto que o array eh crescente o elemento de maior prioridade esta sempre no fim
 * é necessario somente pegar esse numero e diminuir a fila */
int dequeue_Sem_Heap(Sem_Heap_pq *pq){

    // pega ultimo e diminui
    int item = pq->data[pq->size - 1];
    pq->size--; 
    return item;
}

/* 2. Fila de Prioridade com Heap
 * Arvore Binaria inserçao e remoçao O(log n) */

typedef struct{
    int size;
    int data[MAX_SIZE + 1]; // p ignorar o indice 0
} heap_pq;

void init_heap(heap_pq*h){
    h->size = 0;
}

// funcoes p arvore 
int get_parent_index(int i){return i / 2;} 
int get_left_index(int i){return 2 * i;} 
int get_right_index(int i){return 2 * i + 1;} 

// p swapar o valos dos ponteiro
void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* Enqueue - O(log n)
 * bota no fim do array e sobe ele pela arvore ate chegar na posiçao correta, o papa tem
 * que ser bigger que o hijo */
void enqueue_heap(heap_pq *h, int item, int *comparacoes){
    
    //add no fim da fila
    h->size++;
    h->data[h->size] = item;
    
    // sobe na arve
    int key_index = h->size;
    int parent_index = get_parent_index(key_index);

    //enquanto n for o papa master ou ele for maior que o seu papa, precisa swapar
    while(parent_index >= 1){
        (*comparacoes)++; // +1 p contagem, p plotar
        if(h->data[key_index] > h->data[parent_index]){
            swap(&h->data[key_index], &h->data[parent_index]);
            //att os indices p analisar o restante subindo
            key_index = parent_index;
            parent_index = get_parent_index(key_index);
        } else {
            break;
        }
    }
}

/* p dequeue
 * compara o pai c seus filhos e se tiver um filho maior ele troca (desce) */
void max_heapify(heap_pq *h, int i, int *comparacoes){
    int largest = i; // assume i como no maior
    int left_index = get_left_index(i);
    int right_index = get_right_index(i);

    //ve se o esquerdo eh maior
    if(left_index <= h->size){
        (*comparacoes)++;
        if(h->data[left_index] > h->data[i]){
            largest = left_index;
        }
    }
    
    // ve se o direito eh maior
    if(right_index <= h->size){
        (*comparacoes)++;
        if(h->data[right_index] > h->data[largest]){
            largest = right_index;
        }
    }

    // se o maior n for o pai troca
    if(largest != i){
        //troca (c filho maior)
        swap(&h->data[i], &h->data[largest]);
        // recursivamente ate o ele deixar de ser menor q os filhos
        max_heapify(h, largest, comparacoes);
    }
}

/* Dequeue - O(log n) troca a raiz c o ultimo elemento e dps compara ele com os
 * filhos. Desce ele ate nao ter filhos maiores que ele*/
int dequeue_heap(heap_pq *h, int *comparacoes){
    if(h->size == 0) return -1; // Fila vazia
    
    //  salvar a raiz 
    int item = h->data[1]; 
    
    // joga o ultimo elemento da arvore na raiz
    h->data[1] = h->data[h->size]; 
    
    // reduz o array
    h->size--; 
    
    // p ajeitar a arvore 
    max_heapify(h, 1, comparacoes); 
    
    return item; // retorna o elemento que foi removido
}

// Main 
int main(){
    // geracao de numeros aleatorios, time utiliza a tempo atual p gerar numeros, como 
    // os segundos estao smp passando ele sempre vai dar diff(outra seed tipo mine)
    srand(time(NULL));

    // Starta os Csvs
    FILE *arq_enqueue = fopen("comparacoes_enqueue.csv", "w");
    FILE *arq_dequeue = fopen("comparacoes_dequeue.csv", "w");


    // titulo das colunas dos csvs
    fprintf(arq_enqueue, "Tamanho,Insercao_Sem_Heap,Insercao_Com_Heap\n");
    fprintf(arq_dequeue, "Tamanho,Remocao_Sem_Heap,Remocao_Com_Heap\n");
    printf("Números Sorteados:\n");

    for(int tamanho_atual = 10; tamanho_atual <= 9010; tamanho_atual += 1000){
        
        Sem_Heap_pq fila_Sem_Heap_insercao;
        heap_pq fila_heap_insercao;
        Sem_Heap_pq fila_Sem_Heap_remocao;
        heap_pq fila_heap_remocao;
        init_Sem_Heap(&fila_Sem_Heap_insercao);
        init_heap(&fila_heap_insercao);
        init_Sem_Heap(&fila_Sem_Heap_remocao);
        init_heap(&fila_heap_remocao);
        
        int temp = 0; 
        
        // preenche a fila
        for(int i = 0; i < tamanho_atual; i++){
            int num = rand() % 100000;
            enqueue_Sem_Heap(&fila_Sem_Heap_insercao, num, &temp);
            enqueue_heap(&fila_heap_insercao, num, &temp);
            enqueue_Sem_Heap(&fila_Sem_Heap_remocao, num, &temp);
            enqueue_heap(&fila_heap_remocao, num, &temp);
        }
        
        // Lucky blox Randomizer
        int numero_sorteado = rand() % 100000;
        
        // comparacao
        int comp_insercao_Sem_Heap = 0;
        int comp_insercao_heap = 0;
        int comp_remocao_Sem_Heap = 0;
        int comp_remocao_heap = 0;
        
        enqueue_Sem_Heap(&fila_Sem_Heap_insercao, numero_sorteado, &comp_insercao_Sem_Heap);
        enqueue_heap(&fila_heap_insercao, numero_sorteado, &comp_insercao_heap);
        dequeue_Sem_Heap(&fila_Sem_Heap_remocao);
        dequeue_heap(&fila_heap_remocao, &comp_remocao_heap);
        
        // p gravar as linhas nos csvs
        fprintf(arq_enqueue, "%d,%d,%d\n", tamanho_atual, comp_insercao_Sem_Heap, comp_insercao_heap);
        fprintf(arq_dequeue, "%d,%d,%d\n", tamanho_atual, comp_remocao_Sem_Heap, comp_remocao_heap);
        
        // p visualizar no terminal
        printf("Fila: %4d itens | Sorteado: %5d | Ins. Sem Heap: %4d | Ins. Heap: %2d | Rem. Sem Heap: %d | Rem. Heap: %2d\n", 
               tamanho_atual, numero_sorteado, comp_insercao_Sem_Heap, comp_insercao_heap, comp_remocao_Sem_Heap, comp_remocao_heap);
    }

    fclose(arq_enqueue);
    fclose(arq_dequeue);
    printf("Os arquivos 'comparacoes_enqueue.csv' e 'comparacoes_dequeue.csv' foram gerados.\n");

    return 0;
}