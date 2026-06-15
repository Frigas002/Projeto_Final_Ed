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
    double fobj_valor;
}resultado;

tableau *criar_tab(int restricoes, int var);
void free_tab(tableau *t);

int find_col(tableau *t);
int find_line(tableau *t, int col_pivo);
void pivotar(tableau *t, int lin_pivo, int col_pivo);
int executar(tableau *t);
double extrair(tableau *t, int indice_col);

tableau *ler_arquivo(const char *nome_arq);
tableau *add_restricao (tableau *atual, int indice_variavel, double limite, int maior_ou_igual);
resultado branch_and_bound(tableau *t, int var_originais);


#endif