#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "satsolver.h"
//cnf e a forma normal conjuntiva ex: ((AvB) ^ (-B V C)) soq esse aqui usa numeros

CNF *dimacs (const char *arquivo){
    FILE *arq = fopen(arquivo, "r");

    if(arq == NULL){
        printf("Vai funfar nao.\n");
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

        char formato_arquivo [10];
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
                int espaco_literais = 4; //espaco x de inicio
                int quantidade_agr = 0; //p contar os literais e servir de indice
                int literal;
                int *clause = (int*) malloc(espaco_literais * sizeof(int)); //clausula atual

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
        printf("tem nada p imprimir\n");
        return;
    }

    else{
        printf("CNF:\n");
        printf("Literais: %d\nClausulas: %d\n", formula->num_literals, formula->num_clauses);
        
        for(int i = 0; i< formula->num_clauses; i++){
            printf("Clausula %d:", i+1);

            for(int j = 0; j< formula->size_clause[i]; j++){
                printf("%d ", formula->clauses[i][j]);
            }
            printf("\n");
        }

        printf("-----------------\n");
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

int main(){
    CNF *formula_mita = dimacs("../teste.cnf"); //esse ../ faz ele parar de procurar na pasta output

    if(formula_mita != NULL){
        printar(formula_mita);
        dar_free(formula_mita);
    }


    return 0;
}