#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define TAM 256

typedef struct arve {
    void* caracter; 
    int frequencia;
    struct arve *esq, *dir, *prox;
} arve;

typedef struct Lista {
    void *inicio;
    int tam;
} Lista;

void criar_lista(Lista *lista) {
    lista->inicio = NULL;
    lista->tam = 0;
}

void inserirorde(Lista *lista, arve *no) {
    arve *aux;
    arve *inicio = (arve *) lista->inicio; 
    
    if(inicio == NULL) {
        lista->inicio = no;
    }
    else if(no->frequencia < inicio->frequencia) {
        no->prox = inicio;
        lista->inicio = no;
    }
    else {
        aux = inicio;
        while(aux->prox && aux->prox->frequencia <= no->frequencia)
            aux = aux->prox;
            
        no->prox = aux->prox;
        aux->prox = no;
    }
    lista->tam++;
}

void preencher(unsigned int tab[], Lista *lista) {
    int i;
    arve *novo;
    for(i = 0; i < TAM; i++) {
        if(tab[i] > 0) {
            novo = malloc(sizeof(arve));
            if(novo) {
                unsigned char *c = malloc(sizeof(unsigned char));
                *c = i;
                novo->caracter = c; 
                novo->frequencia = tab[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo->prox = NULL;
                inserirorde(lista, novo);
            }
        }
    }
}

void freq_arquivo(const char *nome_arquivo, unsigned int tab[]) {
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq) {
        unsigned char c;
        while (fread(&c, sizeof(unsigned char), 1, arq)) {
            tab[c]++;
        }
        fclose(arq);
    } else {
        printf("\tErro: Não existe um arquivo '%s' na pasta para compactar.\n", nome_arquivo);
    }
}

void init(unsigned int tab[]) {
    int i;
    for (i = 0; i < TAM; i++) {
        tab[i] = 0;
    }
}

arve* remover(Lista *lista) {
    arve *aux = NULL;
    if(lista->inicio) {
        aux = (arve *)lista->inicio;
        lista->inicio = aux->prox;
        aux->prox = NULL;
        lista->tam--;
    }
    return aux;
}

arve* montar_arve(Lista *lista) {
    arve *primeiro, *segundo, *novo;
    while(lista->tam > 1) {
        primeiro = remover(lista);
        segundo = remover(lista);
        novo = malloc(sizeof(arve));
        
        if(novo) {
            unsigned char *c = malloc(sizeof(unsigned char));
            *c = '*'; // Nós internos são identificados por '*'
            
            novo->caracter = c;
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->prox = NULL;
            
            inserirorde(lista, novo);
        }
    }
    return (arve *)lista->inicio; 
}

int altura(arve *raiz) {
    if(raiz == NULL) return -1;
    else {
        int esq = altura(raiz->esq) + 1;
        int dir = altura(raiz->dir) + 1; 
        return (esq > dir) ? esq : dir;
    }
}

char** alocacaodi(int colunas) {
    char** dicionario = malloc(sizeof(char*) * TAM);
    for(int i = 0; i < TAM; i++) {
        dicionario[i] = calloc(colunas, sizeof(char)); 
    }
    return dicionario;
}

void g_dicionario(char **dicionario, arve *raiz, char *caminho, int colunas) {
    char esquerda[colunas], direita[colunas];
    
    if(raiz->esq == NULL && raiz->dir == NULL) {
        unsigned char c = *(unsigned char*)raiz->caracter;
        strcpy(dicionario[c], caminho);
    }
    else {
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);
        strcat(esquerda, "0");
        strcat(direita, "1");
        g_dicionario(dicionario, raiz->esq, esquerda, colunas);
        g_dicionario(dicionario, raiz->dir, direita, colunas);
    }
}


void tamanho_arvore_arquivo(arve *raiz, int *tamanho) {
    if (raiz == NULL) return;
    if (raiz->esq == NULL && raiz->dir == NULL) {
        unsigned char c = *(unsigned char*)raiz->caracter;
        if (c == '*' || c == '\\') {
            (*tamanho) += 2; // Escape + Char
        } else {
            (*tamanho) += 1; // Apenas Char
        }
    } else {
        (*tamanho) += 1; // Nó interno '*'
        tamanho_arvore_arquivo(raiz->esq, tamanho);
        tamanho_arvore_arquivo(raiz->dir, tamanho);
    }
}

//salva a árvore no arquivo em Pré-Ordem.

void salvar_arvore_arquivo(arve *raiz, FILE *out) {
    if (raiz == NULL) return;
    if (raiz->esq == NULL && raiz->dir == NULL) {
        unsigned char c = *(unsigned char*)raiz->caracter;
        if (c == '*' || c == '\\') {
            unsigned char escape = '\\';
            fwrite(&escape, sizeof(unsigned char), 1, out); // Grava o Escape
        }
        fwrite(&c, sizeof(unsigned char), 1, out); // Grava a folha
    } else {
        unsigned char ast = '*';
        fwrite(&ast, sizeof(unsigned char), 1, out); // Grava nó interno
        salvar_arvore_arquivo(raiz->esq, out);
        salvar_arvore_arquivo(raiz->dir, out);
    }
}

int calcular_lixo(char **dicionario, unsigned int tab[]) {
    long long total_bits = 0;
    for (int i = 0; i < TAM; i++) {
        if (tab[i] > 0) {
            total_bits += tab[i] * strlen(dicionario[i]);
        }
    }
    return (8 - (total_bits % 8)) % 8;
}

// compacta em 2 bytes de cabeçalho, arvore em pré-ordem e dados
void compactar(char** dicionario, unsigned int tab[], arve *arvore, const char *arq_entrada, const char *arq_saida) {
    FILE *in = fopen(arq_entrada, "rb");
    FILE *out = fopen(arq_saida, "wb");
    
    if(in && out) {
        int tam_arvore = 0;
        tamanho_arvore_arquivo(arvore, &tam_arvore);
        int tam_lixo = calcular_lixo(dicionario, tab);
        
        unsigned char byte1 = (tam_lixo << 5) | (tam_arvore >> 8);
        unsigned char byte2 = (tam_arvore & 0xFF);
        
        fwrite(&byte1, sizeof(unsigned char), 1, out);
        fwrite(&byte2, sizeof(unsigned char), 1, out);

        salvar_arvore_arquivo(arvore, out);

        int j = 7;
        unsigned char byte = 0;
        unsigned char c;
        
        while (fread(&c, sizeof(unsigned char), 1, in)) {
            char *codigo = dicionario[c];
            int i = 0;
            while(codigo[i] != '\0') {
                if(codigo[i] == '1') {
                    byte = byte | (1 << j);
                }
                j--;
                if(j < 0) {
                    fwrite(&byte, sizeof(unsigned char), 1, out);
                    byte = 0;
                    j = 7;
                }
                i++;
            }
        }
        if(j != 7) {
            fwrite(&byte, sizeof(unsigned char), 1, out);
        }
        
        fclose(in);
        fclose(out);
        remove(arq_entrada);
        printf("\tArquivo compactado com Cabeçalho gerado com sucesso!\n");
    }
}

arve* reconstruir_arvore(FILE *arq, int *tree_size) {
    if (*tree_size <= 0) return NULL;
    
    unsigned char c;
    fread(&c, sizeof(unsigned char), 1, arq);
    (*tree_size)--;
    
    arve *novo = malloc(sizeof(arve));
    unsigned char *dado = malloc(sizeof(unsigned char));
    
    if (c == '*') {
        *dado = '*';
        novo->caracter = dado;
        novo->esq = reconstruir_arvore(arq, tree_size);
        novo->dir = reconstruir_arvore(arq, tree_size);
    } else {
        if (c == '\\') { // Se for escape, lê o próximo como caractere real
            fread(&c, sizeof(unsigned char), 1, arq);
            (*tree_size)--;
        }
        *dado = c;
        novo->caracter = dado;
        novo->esq = NULL;
        novo->dir = NULL;
    }
    return novo;
}

void descompactar(const char *arq_entrada, const char *arq_saida) {
    FILE *in = fopen(arq_entrada, "rb");
    FILE *out = fopen(arq_saida, "wb");
    
    if(in && out) {
        unsigned char cabecalho[2];
        fread(cabecalho, sizeof(unsigned char), 2, in);
        
        // extrai o cabeçalho
        int trash_size = cabecalho[0] >> 5;
        int tree_size = ((cabecalho[0] & 0x1F) << 8) | cabecalho[1];
        
        arve *raiz = reconstruir_arvore(in, &tree_size);
        arve *aux = raiz;
        
        // olha quanto falta pra cabar a leitura
        long pos_atual = ftell(in);
        fseek(in, 0, SEEK_END);
        long tamanho_arquivo = ftell(in);
        fseek(in, pos_atual, SEEK_SET); // Volta para onde estava
        long bytes_restantes = tamanho_arquivo - pos_atual;
        
        //descompactação
        for (long b = 0; b < bytes_restantes; b++) {
            unsigned char byte;
            fread(&byte, sizeof(unsigned char), 1, in);
            
            // Se for o último byte do arquivo, parar no lixo
            int limite = (b == bytes_restantes - 1) ? trash_size : 0;
            
            for(int i = 7; i >= limite; i--) {
                if(byte & (1 << i))
                    aux = aux->dir;
                else
                    aux = aux->esq;
                    
                if(aux->dir == NULL && aux->esq == NULL) {
                    unsigned char c = *(unsigned char*)aux->caracter;
                    fwrite(&c, sizeof(unsigned char), 1, out);
                    aux = raiz;
                }
            }
        }
        fclose(in);
        fclose(out);
        remove(arq_entrada);
        printf("\tArquivo descompactado com sucesso a partir do Cabeçalho!\n");
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    
    unsigned int tabela_freq[TAM];
    Lista lista;
    arve *arvore;
    int colunas, opcao;
    char **dicionario;
    

    printf("\t-------------------------------------\n");
    printf("\tMenu de funcionalidades:\n\t1.Compactar\n\t2.Descompactar\n\t3.Encerrar\n\tSelecione sua funcionalidade:");
    scanf("%d",&opcao);
    switch (opcao){
    case 1:{
        char nome[TAM],nome_compactado[TAM];       
        printf("\n\tNome do arquivo a ser compactado:");
        scanf("%s",nome);
        sprintf(nome_compactado,"%s.huff",nome);



        init(tabela_freq);
        freq_arquivo(nome, tabela_freq);
    
        criar_lista(&lista);
        preencher(tabela_freq, &lista);
    
        if(lista.tam == 0) return 0; 
    
        arvore = montar_arve(&lista);
    
        colunas = altura(arvore) + 1;
        dicionario = alocacaodi(colunas);
        g_dicionario(dicionario, arvore, "", colunas);
        // Passamos a tabela para ajudar a contar o lixo, e a arvore para o cabeçalho
        compactar(dicionario, tabela_freq, arvore, nome, nome_compactado);
        break;
    }
    case 2:{
        char nome_compactado[TAM];       
        printf("\n\tNome do arquivo a ser descompactado:");
        scanf("%s",nome_compactado);
        int tam_des=strlen(nome_compactado)-5;
        char nome_descompactado[strlen(nome_compactado)-5];
        strncpy(nome_descompactado,nome_compactado,tam_des);
        nome_descompactado[tam_des]='\0';
        descompactar(nome_compactado, nome_descompactado);
        break;
    }
    case 3:
        return 0;
        break;

    }
    printf("\t-------------------------------------\n");
    return 0; 
}