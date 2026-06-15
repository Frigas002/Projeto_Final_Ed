#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "satsolver.h"
//cnf e a forma normal conjuntiva ex: ((AvB) ^ (-B V C)) soq esse aqui usa numeros

no* criar_no (int literal, int valor){ //agr usa arvore propriamente
    no *novo_no = (no*) malloc(sizeof(no)); //usa-se malloc pois o lixo de memoria que o malloc guarda é sobrescrito direto na função (logo em seguida)

    novo_no->literal = literal;
    novo_no->valor = valor;
    novo_no->right = NULL;
    novo_no->left = NULL;

    return novo_no;
}

void free_tree(no *no){
    if(no ==  NULL){
        return;
    }

    else{
        free_tree (no->left);
        free_tree (no->right);
        free(no);
    }

}

CNF *dimacs (const char *arquivo){ //funcao pra fazer funfar o arquivo
    FILE *arq = fopen(arquivo, "r"); //cria o arquivo

    if(arq == NULL){
        printf("Erro ao utilizar arquivo teste (arquivo vazio)\n");
        return NULL;
    }

    else{
        CNF *formula = (CNF*) malloc(sizeof(CNF));
        char letra;

        while(1){
            letra = fgetc(arq); 

            if(letra == 'c'){ //eh um comentario
                while(letra != '\n' && letra != EOF){
                    letra = fgetc(arq); //"ignora" as linhas
                }
            }

            else{ // se n for c, devolve a letra ("p" provavelmente ou nulo sla) e da break
                ungetc(letra, arq);
                break;
            }
        }

        char formato_arquivo [10];//so p "descartar" o cnf que vem no arquivo
        int lidos = fscanf(arq, "p %s %d %d", formato_arquivo, &formula->num_literals, &formula->num_clauses);

        if(lidos != 3){
            printf("Da nao champs.\n");
            fclose(arq);
            free(formula);
            return NULL;
        }

        else{
            formula->clauses = (int**) malloc(formula->num_clauses *sizeof(int*)); //alocando um array de ponteiros
            formula->size_clause = (int*) malloc(formula->num_clauses *sizeof(int)); //alocando o array especifico


            for(int i = 0; i< formula->num_clauses; i++){
                int espaco_literais = 4; //aposta de tamanho no inicio
                int quantidade_agr = 0; //p contar os literais e servir de indice
                int literal;
                int *clause = (int*) malloc(espaco_literais * sizeof(int)); //vetor temporario

                while(fscanf(arq, "%d", &literal) == 1){
                    if(literal == 0){ //se for 0 acabou a linha
                        break;
                    }

                    if(quantidade_agr >= espaco_literais){ //se tem mais literais q o esperado
                        espaco_literais *=2;
                        clause = (int*) realloc(clause, espaco_literais * sizeof(int));
                    }

                    clause[quantidade_agr] = literal;
                    quantidade_agr++;
                }

                formula->clauses[i] = clause; //atualiza a clausula i
                formula->size_clause[i] = quantidade_agr; //atualiza o tamanho da clausula i
            }

            fclose(arq);
            return formula;

        }

    }
}

//funcao p ver se ta funfando (dps devo apagar)

void printar (CNF *formula){
    if(formula == NULL){
        printf("Nada a imprimir\n");
        return;
    }

    else{
        printf("CNF:\n");
        printf("Literais: %d\nClausulas: %d\n", formula->num_literals, formula->num_clauses);
        
        for(int i = 0; i< formula->num_clauses; i++){
            printf("Clausula %d: ", i+1);

            for(int j = 0; j< formula->size_clause[i]; j++){
                printf("%d ", formula->clauses[i][j]);
            }
            printf("\n");
        }

        printf("-----------------\n");
        printf("Resultado: ");
    }
}

void dar_free (CNF *formula){
    if(formula == NULL){ //se for null so retorna
        return;
    }

    else{
        for(int i = 0; i< formula->num_clauses; i++){ //da free em cada clausula unitaria
            free(formula->clauses[i]);
        }

        free(formula->clauses); //free no array de ponteiro
        free(formula->size_clause); //free na estrutura da clausula

        free(formula); //free no bglh principal
    }

}
/*
    Essa func ai serve como auxiliar ao DPLL (bglh principal pro SATsolver)
    *DPLL eh o nome de cada fera q fez (os caba sao bom)
      ->funciona como uma arvore de decisao

*/

int ve_clausula (CNF *formula, int indice, int *atribuicoes, int *literal_livre_unico){ //recebe a formula, o indice p saber em qual olhar e o array das atribuicoes das clausulas
    int literais_livres = 0; //literais que nao foram considerados nada ainda e eu posso usar p chutar
    
    for(int j = 0; j< formula->size_clause[indice]; j++){
        int literal = formula->clauses[indice][j];
        int literal_absoluto = abs (literal);

        if((literal > 0 && atribuicoes[literal_absoluto] == 1) || (literal < 0 && atribuicoes[literal_absoluto] == -1)){
            return 1;
            //se o literal for positivo e tiver como verdadeiro na clausula a clausula td eh vdd ex: (-A V B V C) : B = 1
            //se o literal for negativo e tiver como negativo na clausula, a clausula td eh vdd ex: (-A V B V C) : A = -1
        }

        if(atribuicoes[literal_absoluto] == 0){
            literais_livres++;
            *literal_livre_unico = literal;
        }
    }

    if (literais_livres == 0){ //se deu 0 e n retornou 1 o DPLL deu confusao qnd foi chutar
        return 0;
    }

    if(literais_livres == 1){ //se deu 1, eu posso chutar se o literal eh vdd ou nao
        return -1;
    }

    return -2; //se n retornou 1, nem 0, nem -1, quer dizer que tem mto literal sem atribuição
}

int propagacao (CNF *formula, int *atribuicoes){
    int alteracao = 1; //define 

    while(alteracao){
        alteracao = 0; //supoe q é o ultimo round. Se n for a flag ativa de novo no if

        for (int i = 0; i< formula->num_clauses; i++){
            int literal;
            int resultado = ve_clausula(formula, i, atribuicoes, &literal);

            if(resultado == 0){ //falhou
                return 0;
            }

            if(resultado == -1){
                int indice_do_literal = abs(literal);
                
                if(literal > 0){
                    atribuicoes[indice_do_literal] = 1; //se for positivo, recebe verdadeiro
                }

                else{
                    atribuicoes[indice_do_literal] = -1; //se for falso, recebe negativo
                }

                alteracao = 1; // indica a alteracao (suposicao) e da break p refazer o while
                break;
            }
        }
        
    }

    return 1;
}



int dpll(CNF *formula, int *atribuicoes, no **no){
    int resultado_prop = propagacao(formula, atribuicoes);

    if(resultado_prop == 0){ //se o resultado deu 0, falhou a suposicao e o galho morre
        return 0;
    }

    else{
        int satisfeito = 1;

        for(int i = 0; i< formula->num_clauses; i++){
            int literal; //so p passar a func
            int resultado = ve_clausula(formula, i, atribuicoes, &literal); //ve qual clausula eh satisfeita

            if(resultado != 1){ //se deu break, a gnt sabe a linha q ta dando coisa
                satisfeito = 0;
                break;
            }

        }
            if(satisfeito == 1){ //se dps do loop, ele ta satisfeito, o dpll retorna 1
                return 1;
            }
        
        int livre = -1;

        for(int i = 1; i<= formula->num_literals; i++){ //dps do break ele vem p ca, p ver qual e o literal sem atribuicao
            if(atribuicoes[i] == 0){
                livre = i;
                break;
            }
        } //se livre n for mudado significa q n tem mais chute p fzr :/

        if(livre == -1){ //se nao tem livre pra chutar retorna 0 e eh unsat
            return 0;
        }

        int *salva_atribuicao = (int*) malloc((formula->num_literals + 1) * sizeof(int)); //array temporario p salvar o estado da atribuicao
        memcpy(salva_atribuicao, atribuicoes, (formula->num_literals + 1) * sizeof(int));

        //comeca pela esquerda chutando q eh positivo
        *no = criar_no(livre, 1);
        atribuicoes[livre] = 1; 
        

        if(dpll(formula, atribuicoes, &((*no) ->left )) == 1){ //se deu bom ele da free no array temporario e retorna q funfou
            free(salva_atribuicao);
            return 1;
        }

        free_tree ((*no) ->left); //se falhou ele da free no no e faz o pai apontar p null
        (*no) ->left = NULL;

        memcpy(atribuicoes, salva_atribuicao, (formula->num_literals + 1) * sizeof(int)); //restaura o array temporario

        //agr chuta q eh negativo pela direita
        (*no)->valor = -1;
        atribuicoes[livre] = -1;

        if(dpll(formula, atribuicoes, &((*no) ->right )) == 1){ //deu crt ele faz a msm coisa
            free(salva_atribuicao);
            return 1;
        }

        free_tree ((*no) ->right); //falhou ele da free no no da direita e faz o pai apontar p null
        (*no) ->right = NULL;


        //se ambas falharam, da free no no que gerou a esquerda e a direita
        free(*no);
        *no = NULL;

        memcpy(atribuicoes, salva_atribuicao, (formula->num_literals + 1) * sizeof(int));
        free(salva_atribuicao);

        return 0;
    }
}

int sat_para_smt(const char *arquivo_cnf, int **vetor_atribuicoes_saida, int *num_literais) {
    CNF *formula = dimacs(arquivo_cnf);
    if (formula == NULL) return 0;

    int *atribuicoes = (int*) calloc(formula->num_literals + 1, sizeof(int));
    if (atribuicoes == NULL) {
        dar_free(formula);
        return 0;
    }

    no *raiz = NULL;
    int res = dpll(formula, atribuicoes, &raiz);

    free_tree(raiz);
    
    if (res == 1) {
        // Se deu SAT, exportamos os dados que o SMT precisa
        *vetor_atribuicoes_saida = atribuicoes; 
        *num_literais = formula->num_literals;
    } else {
        free(atribuicoes);
    }

    dar_free(formula);
    return res; // Retorna 1 para SAT e 0 para UNSAT
}


#ifndef FOR_SMT
int main(int argc, char *argv[]){
    
    if(argc < 2){
        printf("Usando: %s <arquivo.cnf>\n", argv[0]);
        return 1;
    }

    CNF *formula = dimacs(argv[1]);

    if(formula == NULL){
        printf("Formula vazia!\n");
        return 1;
    }

    else{
        int *atribuicoes =  (int*) calloc(formula->num_literals +1, sizeof(int)); //determina todos os literais como "indecididos"

        if(atribuicoes == NULL){
            printf("Sem atribuicoes\n");
            dar_free(formula);
            return 1;
        }

        else{
            no *raiz = NULL;
            int res = dpll(formula, atribuicoes, &raiz);

            if(res == 1){
                printar(formula);
                printf("SAT!\n");
            }

            else{
                printar(formula);
                printf("UNSAT :/\n");
            }
            
            free_tree(raiz);
            free(atribuicoes);
            dar_free(formula);
        }

    }
return 0;

}
#endif