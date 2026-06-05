#include <stdlib.h>
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
    for(int i = 0; i< 20000; i++){
        int coluna = find_col(t);

        if(coluna == -1){
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

double extrair (tableau *t, int indice_col){ //serve p extrair o valor de uma variavel x de uma coluna da matriz
    int linha_com_um= -1;
    int contador_uns = 0;
    int contador_zeros = 0;

    for(int i = 0; i < t->linhas; i++){
        double valor = t->matriz[i][indice_col];

        if(valor == 1.0){
            linha_com_um = i;
            contador_uns++;
        }

        else if(valor == 0.0){
            contador_zeros++;
        }

    }

    if(contador_uns == 1 && (contador_zeros == t->linhas -1)){
        return t->matriz[linha_com_um][t->colunas-1]; //se a coluna estiver na base (tudo 0 e so tem um pivo com valor 1), ta ok: retorna o valor q ficou la no resultado p essa variavel
    }

    return 0.0;

}