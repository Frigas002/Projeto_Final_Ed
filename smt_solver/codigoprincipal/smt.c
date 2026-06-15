#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LIA/simplex.h"
#include "SAT/satsolver.h"

void gerar_arquivo(const char *nome_saida, const char *nome_entrada){
    FILE *in = fopen(nome_entrada, "r");
    FILE *out = fopen(nome_saida, "w");

    if(!in ||!out){
        printf("Erro ao manipular arquivos\n");
        if(in) fclose(in);
        if(out) fclose(out);

        return;
    }

    int restricoes, variaveis_originais;

    if(fscanf(in, "%d %d", &restricoes, &variaveis_originais) != 2){
        fclose(in);
        fclose(out);
        return;
    }

    fprintf(out, "%d %d\n", restricoes, variaveis_originais);

    double valor_fobj;

    for(int i = 0; i<variaveis_originais; i++){
        fscanf(in, "%lf", &valor_fobj);
        fprintf(out, "%.2lf ", -valor_fobj);
    }
    fprintf(out, "\n");

    double coeficiente;
    for(int i = 0; i<restricoes; i++){
        for(int j = 0; j<variaveis_originais + 1; j++){
            fscanf(in, "%lf", &coeficiente);
            fprintf(out, "%.2lf ", coeficiente);
        }
        fprintf(out, "\n");
    }
    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Uso: %s <arquivo_entrada.txt>\n", argv[0]);
        return 1;
    }

    else{
        printf("--------Ativando SAT solver--------\n");

        int *atribuicoes_sat = NULL;
        int num_literals_sat = 0;

        int sat = sat_para_smt(argv[1], &atribuicoes_sat, &num_literals_sat);

        if(sat == 0){
            printf("Resultado: UNSAT :/ (inviavel no nivel logico)\n");
            return 0;
        }

        printf("SAT encontrado!\n");
        printf("--------Ativando SMT solver e LIA--------\n");

        const char *arq_temp = "temp_lia.txt";
        gerar_arquivo(arq_temp, argv[2]);

        tableau *tab = ler_arquivo(arq_temp);


        if(tab == NULL){
            printf("Erro ao carregar arquivo com as restricoes\n");
            free(atribuicoes_sat);
            return 1;
        }

        int restricoes_antigas = tab->linhas - 1;
        int col_folgaeb = restricoes_antigas + 1;
        int var_originais = tab->colunas - col_folgaeb;

        resultado r = branch_and_bound(tab, var_originais);

        if (r.tem_solucao == 1){
            printf("\nResultado final: SAT!\n");
            printf("Valores encontrados para as variaveis:\n");

            for(int i = 0; i < r.variaveis; i++){
                printf("  x[%d] = %.0f\n", i, r.valores[i]);
            }
            free(r.valores);
        }

        else{
            printf("Resultado final: UNSAT! (as restricoes sao inviaveis)\n");
        }

        free(atribuicoes_sat);
        free_tab(tab);
        remove(arq_temp);
    }


    return 0;
}