#ifndef SATSOLVER_H
#define SATSOLVER_H

typedef struct{
    int num_literals;
    int num_clauses;
    int *size_clause;
    int **clauses;
}CNF;

CNF *dimacs(const char *arquivo);
void dar_free(CNF *formula);
int dpll(CNF *formula, int *atribuicoes);

#endif //SATSOLVER_H