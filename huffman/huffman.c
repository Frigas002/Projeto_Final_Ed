#include<stdio.h>
#include<stdlib.h>
#include<locale.h>
#define TAM 256
typedef struct arve{
    unsigned char caracter;
    int frequencia;
    struct arve *esq, *dir,*prox;
}arve;
typedef struct Lista{
    arve *inicio;
    int tam;
}Lista;

void criar_lista(Lista*lista){
    lista->inicio=NULL;
    lista->tam=0;
}

void inserirorde(Lista *lista, arve *no){
    arve *aux;
    //lista vazia?
    if(lista->inicio==NULL){
        lista->inicio=no;
    }
    //freq menor que a do inicio?
    else if(no->frequencia<lista->inicio->frequencia){
        no->prox=lista->inicio;
        lista->inicio=no;
    }
    else{
        aux=lista->inicio;
        while(aux->prox&&aux->prox->frequencia<=no->frequencia)
            aux=aux->prox;
        no->prox=aux->prox;
        aux->prox=no;
    }
    lista->tam++;
}
void preencher(unsigned int tab[], Lista *lista){
    int i;
    arve *novo;
    for(i=0;i<TAM;i++){
        if(tab[i]>0){
            novo = malloc(sizeof(arve));
            if(novo){
                novo->caracter=i;
                novo->frequencia = tab[i];
                novo->dir=NULL;
                novo->esq=NULL;
                novo->prox=NULL;
                inserirorde(lista,novo);
            }
            else{
                printf("\tErro ao alocar em Alocar memeoria\n");
                break;
            }
        }
    }
}

void printlist(Lista *lista){
    arve *aux=lista->inicio;
    printf("\tLista de tamanho: %d\n", lista->tam);
    while (aux){
        printf("\tCaracter :%c Freq: %d\n",aux->caracter,aux->frequencia);
        aux = aux->prox;
    }
    
}

void init(unsigned int tab[]){
    int i;
    for (i=0;i<TAM; i++){
        tab[i]=0;
    }
}
void freq(unsigned char text[], unsigned int tab[]){
    int i =0;
    while(text[i]!='\0'){
        tab[text[i]]++;
        i++;
    }
}
void print_freq(unsigned int tab[]){
    int i=0;
    printf("\tTabela de freq\n");
    for(i=0;i<TAM; i++){
        if(tab[i]>0){
        printf("\t%d = %u = %c\n",i,tab[i],i);
    }
}
}
int main(){
    unsigned char texto[]="Did you ever hear the tragedy of Darth Plagueis The Wise?";
    unsigned int tabela_freq[TAM];
    Lista lista;
    setlocale(LC_ALL,"Portuguese");

//p1
    init(tabela_freq);
    freq(texto,tabela_freq);
    print_freq(tabela_freq);
//p2
    printf("\t-------------------------------------\n");
    criar_lista(&lista);
    preencher(tabela_freq,&lista);
    printlist(&lista);
    return 0;
}