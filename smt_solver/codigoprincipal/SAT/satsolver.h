#ifndef SATSOLVER_H
#define SATSOLVER_H

typedef struct{
    int num_literals;
    int num_clauses;
    int *size_clause;
    int **clauses;
}CNF;

typedef struct no{ //usando arvore oficial
    int literal;
    int valor; //pos ou neg
    struct no *right;
    struct no *left;
} no;

CNF *dimacs(const char *arquivo);
void dar_free(CNF *formula);


int dpll(CNF *formula, int *atribuicoes, no **raiz);
void free_tree (no *no);

int sat_para_smt(const char *arquivo_cnf, int **vetor_atribuicoes_saida, int *num_literais);

#endif //SATSOLVER_H