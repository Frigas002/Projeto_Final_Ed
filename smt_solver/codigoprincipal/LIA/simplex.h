#ifndef SIMPLEX_H
#define SIMPLEX_H

typedef struct{
    int linhas;
    int colunas;
    double **matriz;
}tableau;

typedef struct{
    int tem_solucao;
    double *valores;
    int variaveis;
}resultado;

tableau *criar_tab(int restricoes, int var);
void free_tab(tableau *t);

#endif