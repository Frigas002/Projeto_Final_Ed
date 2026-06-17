#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simplex.h"


tableau *criar_tab(int restricoes, int var){
    tableau *new_tab = (tableau*) malloc(sizeof(tableau));

    new_tab->linhas = restricoes + 1;
    new_tab->colunas = var + restricoes + 1;
    //as colunas sao dadas assim pq no tableau se tem uma coluna p cada variavel, uma p cada variavel resto e outra pro resultado
    

    new_tab->matriz = (double **) malloc(new_tab->linhas * sizeof(double *)); //aloca um vetor na vertical que guarda o ponteiro de cada linha

    for(int i = 0; i< new_tab->linhas; i++){
        new_tab->matriz[i] = (double *) calloc(new_tab->colunas, sizeof(double)); //agora ele aloca o vetor horizontal em cada coluna
    } //usa calloc pq ele reserva o espaço e zera tudo (facilita)

    return new_tab;
}

void free_tab(tableau *t){
    if(t == NULL){
        return;
    }

    else{
        for (int i = 0; i<t->linhas; i++){
            free(t->matriz[i]);
        }
        free(t->matriz);
        free(t);
    }
}

int find_col(tableau *t){ //varredura horizontal
    int lin_obj = t->linhas - 1; //a ultima linha é guiadora por convencao. Ela serve pra dizer pra onde andar, guardando o fato de ter um coeficiente negativo
    int col_pivo = -1;
    double menor = 0.0;

    for (int i = 0; i< t->colunas - 1; i++){ //para em colunas-1 pq eh a de resultado
        if(t->matriz[lin_obj][i] < menor){
            menor = t->matriz[lin_obj][i];

            col_pivo = i;
        }
    }

    //se procura o menor coeficiente de uma expressao justamente pra ir em direção das folgas matematicas pra validar o sistema
    return col_pivo;//se retornar -1, significa que nao há mais pulos pra fazer (estou no melhor vertice possivel)
}

int find_line(tableau *t, int col_pivo){ //varredura vertical
    int lin_pivo = -1; //a linha pivo, na geometria, impede q o algoritmo saia da regiao possivel
    double menor_razao = -1;  //flag. Forma que se avalia a possibilidade de estar dentro do intervalo ou nao
    //o lin pivo ser -1 indica q as variaveis podem crescer indefinidamente (algo errado ae)

    for(int i = 0; i< t->linhas - 1; i++){ //ignora a linha objetivo pq ela nao conta como restricao
        double valor_pivo = t->matriz[i][col_pivo];

        if(valor_pivo > 0.0){ //eh necessario q o valor seja maior q 0 (por conta da divisao) e tambem por que se for negativo a parede formada pela expressao ta atras da gnt
            double resultado_dir = t->matriz[i][t->colunas-1];
            double razao = resultado_dir/valor_pivo;

            //a razao diz a distancia de onde eu to pra uma parede no grafico
            //a menor razao diz qual ta mais perto de mim, ou seja, pra qual eu devo ir

            /* ex: 2x<= 8 (razao = 4) e 4x<=12 (razao =3);
                escolher x = 3 garante q eu vou cumprir ambas as equacoes: x = 3 -> goat :)
            */

            if(lin_pivo == -1|| razao < menor_razao){
                menor_razao = razao;
                lin_pivo = i;
            }
        }
    }
    return lin_pivo;
}

void pivotar (tableau *t, int lin_pivo, int col_pivo){ //eliminacao de gauss jordan soq greedy a full
    double pivo_ofc = t->matriz[lin_pivo][col_pivo];

    for(int j = 0; j<t->colunas; j++){
        t->matriz[lin_pivo][j] /= pivo_ofc;
    }

    for(int i = 0; i< t->linhas; i++){

        if(i != lin_pivo){
            double fat = t->matriz[i][col_pivo];

            for(int j = 0; j< t->colunas; j++){
                t->matriz[i][j] -= (fat * t->matriz[lin_pivo][j]);
            }
        }

    }
    //o algoritmo basicamente vai atrás das quinas q melhoram o resultado (optimal)
    //a funcao find_col diz qual dessas quinas eh a mais mita p ir. Aí o pivotamento ocorre em direcao dessa mita 
}

int executar (tableau *t){
    
    for(int k = 0; k < 20000; k++){
        int lin_pivo_dual = -1;
        double menor_b = -1e-5;

        // Procura a linha com o resultado mais negativo
        for(int i = 0; i < t->linhas - 1; i++){
            if(t->matriz[i][t->colunas - 1] < menor_b){
                menor_b = t->matriz[i][t->colunas - 1];
                lin_pivo_dual = i;
            }
        }

        if(lin_pivo_dual == -1) break;

        int col_pivo_dual = -1;
        double menor_razao_dual = -1.0;

        // Procura a coluna pivô
        for(int j = 0; j < t->colunas - 1; j++){
            double coef = t->matriz[lin_pivo_dual][j];
            if(coef < -1e-5){ // No Dual, o pivô tem que ser negativo
                double razao = fabs(t->matriz[t->linhas - 1][j] / coef);
                if(col_pivo_dual == -1 || razao < menor_razao_dual){ //coloca a primeira razao e depois ela é comparada com as outras para ser det qual a menor
                    menor_razao_dual = razao;
                    col_pivo_dual = j;
                }
            }
        }

        if(col_pivo_dual == -1){
            return 0; // Se a linha é negativa mas não tem coeficiente negativo é impossivel
        }

        pivotar(t, lin_pivo_dual, col_pivo_dual);
    }

    for(int i = 0; i < 20000; i++){
        int coluna = find_col(t);

        if(coluna == -1){

            for(int l = 0; l < t->linhas - 1; l++) {
                if(t->matriz[l][t->colunas - 1] < -1e-5) {
                    return 0; 
                }
            }
            return 1; 
        }

        int linha = find_line(t, coluna);

        if(linha == -1){
            return 0;
        }

        pivotar(t, linha, coluna);
    }

    return 0;
}

double extrair (tableau *t, int indice_col){ 
    int linha_com_um = -1;
    int contador_uns = 0;
    int contador_zeros = 0;
    int num_linhas = t->linhas - 1; // ignora a linha objetivo

    for(int i = 0; i < num_linhas; i++){ 
        double valor = t->matriz[i][indice_col];

        if(fabs(valor - 1.0) < 1e-5){
            linha_com_um = i;
            contador_uns++;
        } else if (fabs(valor) < 1e-5) {
            contador_zeros++;
        }
    }

    // Para estar no padrao tem que ter EXATAMENTE um 1.0 e o resto todo mundo 0.0
    if(contador_uns == 1 && contador_zeros == (num_linhas - 1)){
        return t->matriz[linha_com_um][t->colunas-1]; 
    }

    return 0.0; // Se não for básica, ela vale 0 na solução atual
}

tableau *ler_arquivo(const char *nome_arq){
    FILE *arquivo =  fopen(nome_arq, "r");

    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo pro smt: %s\n", nome_arq); //"protecao" p arquivos nulos
        return NULL;
    }

    else{
        int restricoes, var_originais;

        if(fscanf(arquivo, "%d %d", &restricoes, &var_originais) != 2){
            printf("Erro ao ler as dimensões do problema\n");
            fclose(arquivo);

            return NULL;
        }

        tableau *tab = criar_tab(restricoes, var_originais);

        if(tab == NULL){
            fclose(arquivo);
            return NULL;
        }

        int linha_obj = tab->linhas - 1;
        for(int i = 0; i<var_originais; i++){
            fscanf(arquivo, "%lf", &tab->matriz[linha_obj][i]);
        }

        tab->matriz[linha_obj][tab->colunas -1] = 0.0;

        for(int i = 0; i < restricoes; i++){
            for(int j = 0; j < var_originais; j++){
                fscanf(arquivo, "%lf", &tab->matriz[i][j]);
            }

            int col_folga = var_originais + i;
            tab->matriz[i][col_folga] = 1.0; //coloca os coeficientes das variaveis de folga como 1
            
            int ultima_col = tab->colunas - 1;
            fscanf(arquivo, "%lf", &tab->matriz[i][ultima_col]);
        }

        fclose(arquivo);
        return tab;
    }
}


int encontrar_fracao(tableau *t, int var_originais, int *indice_variavel, double *valor_fracao){//depois de pegar o numero como double
    for(int i = 0; i < var_originais; i++){
        double valor = extrair(t, i);//extrai o valor

        double arredondamento = round(valor); //arredonda o valor (p cima ou p baixo)

        if(fabs(valor - arredondamento) > 1e-5){ //se a diferenca entre o valor original e o valor arredondado for menor que 10^-5 ele considera o numero inteiro
            *indice_variavel = i;
            *valor_fracao = valor;
            return 1;
        }
    }
    return 0;
}//isso é necessario pela forma que os doubles sao armazenados na memoria. Ex: 3.00000000000000044 (apos sucessivas operações eh esse nmr que ta na matriz)
//se comparar 3.0000000000000044 com 3 (usando ==) vai dar erro e isso destroi as decisoes futuras

tableau *add_restricao(tableau *atual, int indice_variavel, double limite, int maior_ou_igual) {
    int restricoes_antigas = atual->linhas - 1;
    int col_folgaeb = restricoes_antigas + 1;
    int var_originais = atual->colunas - col_folgaeb;

    // Cria o novo tableau com uma linha a mais (para a restrição) 
    // e uma coluna a mais (para a nova variável de folga)
    tableau *tab = criar_tab(restricoes_antigas + 1, var_originais);

    // Copia as restrições antigas (variáveis originais + folgas antigas)
    for (int i = 0; i < restricoes_antigas; i++) {
        for (int j = 0; j < atual->colunas - 1; j++) {
            tab->matriz[i][j] = atual->matriz[i][j];
        }
        tab->matriz[i][tab->colunas - 1] = atual->matriz[i][atual->colunas - 1];
    }

    // Copia a linha objetivo (incluindo as folgas antigas)
    int new_lin_obj = tab->linhas - 1;
    int old_lin_obj = atual->linhas - 1;
    for (int i = 0; i < atual->colunas - 1; i++) {
        tab->matriz[new_lin_obj][i] = atual->matriz[old_lin_obj][i];
    }
    tab->matriz[new_lin_obj][tab->colunas - 1] = atual->matriz[old_lin_obj][atual->colunas - 1];

    int nova_linha_restricao = tab->linhas - 2;
    if (maior_ou_igual == 1) {
        tab->matriz[nova_linha_restricao][indice_variavel] = -1.0;
        tab->matriz[nova_linha_restricao][tab->colunas - 1] = -limite;
    } else {
        tab->matriz[nova_linha_restricao][indice_variavel] = 1.0;
        tab->matriz[nova_linha_restricao][tab->colunas - 1] = limite;
    }

    // 4. Define a nova variável de folga na nova restrição
    // A coluna da nova folga é a última das variáveis de folga antes do RHS
    int nova_coluna_folga = var_originais + restricoes_antigas;
    tab->matriz[nova_linha_restricao][nova_coluna_folga] = 1.0;

    // 5. Ajuste da base: garante que a coluna da variável restrita seja básica (zeros na coluna)
    int linha_basica = -1;
    for (int i = 0; i < restricoes_antigas; i++) {
        if (fabs(tab->matriz[i][indice_variavel] - 1.0) < 1e-5) {
            linha_basica = i;
            break;
        }
    }

    if (linha_basica != -1) {
        double fator = tab->matriz[nova_linha_restricao][indice_variavel];
        for (int j = 0; j < tab->colunas; j++) {
            tab->matriz[nova_linha_restricao][j] -= (fator * tab->matriz[linha_basica][j]);
        }
    }

    return tab;
}

resultado branch_and_bound(tableau *t, int var_originais){
    resultado r;
    r.tem_solucao = 0;
    r.variaveis = var_originais;
    r.valores = (double*) calloc(var_originais, sizeof(double));
    r.fobj_valor = 0.0;

    int viabilidade = executar(t);
    
    if(viabilidade == 0){
        return r;
    }

    else{
        int indice_variavel;
        double valor_fracionario;

        if(encontrar_fracao(t, var_originais, &indice_variavel, &valor_fracionario) == 0){ 
            r.tem_solucao = 1;

            for(int i = 0;  i < var_originais; i++){
                r.valores[i] = extrair(t, i);
            }
            
            r.fobj_valor = t->matriz[t->linhas - 1][t->colunas - 1]; 
            
            return r;
        }

        double piso = floor(valor_fracionario); 
        double teto = ceil(valor_fracionario);

        tableau *esq = add_restricao(t, indice_variavel, piso, 0); // 0 indica ser <=
        resultado r_esq = branch_and_bound(esq, var_originais);
        free_tab(esq);

        tableau *dir = add_restricao(t, indice_variavel, teto, 1); //1 indica ser >= (la da funcao add restricao)
        resultado r_dir = branch_and_bound(dir, var_originais);
        free_tab(dir);

        free(r.valores);

        if(r_esq.tem_solucao == 1 && r_dir.tem_solucao == 1){ 
            if(r_esq.fobj_valor >= r_dir.fobj_valor){
                if(r_dir.valores != NULL) free(r_dir.valores);
                return r_esq;
            }
            else{
                if(r_esq.valores != NULL) free(r_esq.valores);
                return r_dir;
            }
        }
        else if(r_esq.tem_solucao == 1){
            if(r_dir.valores != NULL) free(r_dir.valores);
            return r_esq;
        }
        else{
            if(r_esq.valores != NULL) free(r_esq.valores);
            return r_dir;
        }
    }
}