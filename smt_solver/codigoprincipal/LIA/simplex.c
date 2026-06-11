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
            tab->matriz[i][col_folga] = 1.0;
            
            int ultima_col = tab->colunas - 1;
            fscanf(arquivo, "%lf", &tab->matriz[i][ultima_col]);
        }

        fclose(arquivo);
        return tab;
    }
}

tableau *clonar(tableau *origin){ //pra testar as variaveis precisa-se mexer no tableau (pra nao estragar ele com caminhos errados, a gnt faz uma copia)
    if(origin == NULL){
        return NULL;
    }
    
    else{
        tableau *clone = (tableau*) malloc(sizeof(tableau));

        clone->linhas = origin->linhas;
        clone->colunas = origin->colunas;

        clone->matriz = (double**) malloc(clone->linhas * sizeof(double*)); //aloca as linhas da matriz
        for(int i = 0; i < clone->linhas; i++){//pra cada linha aloca as colunas
            clone->matriz[i] = (double*) malloc(clone->colunas * sizeof(double));

            for(int j = 0; j< clone->colunas; j++){ //copia a matriz original na copia linha por linha / coluna por coluna
                clone->matriz[i][j] = origin->matriz[i][j];
            }
        }

        return clone;
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

tableau *add_restricao(tableau *atual, int indice_variavel, double limite, int maior_ou_igual){
    int restricoes_antigas = atual->linhas-1; //p descobrir as restricoes originais pegamos o valor atual - 1 (tira a linha objetivo)
    int col_folgaeb = restricoes_antigas + 1; //folga e b sao dadas pela qntd de restricoes + 1
    int var_originais = atual->colunas - col_folgaeb; // pra descobrir quantas variaveis temos faz-se o seguinte:

    //p formar as colunas: variaveis + variaveis de folga + resultado ... variaveis =  colunas - variaveis de folga - resultado (colunas)

    tableau *tab = criar_tab(restricoes_antigas+1, var_originais); //coloquei "restricoes_antigas + 1" por uma questão semantica
    //tab é o novo tableau e atual passa a ser o antigo

    for(int i = 0; i< restricoes_antigas; i++){

        for(int j = 0; j<var_originais; j++){ //esse for copia a parte das variaveis originais no lugar adequado
            tab->matriz[i][j] = atual->matriz[i][j];
        }

        for(int j = 0; j<restricoes_antigas; j++){ //esse eh o mais importante, ele copia pra o tableau novo a parte das folgas no lugar correto
            tab->matriz[i][var_originais+j] = atual->matriz[i][var_originais+j];

            //quando adicionamos uma restriçao ela fica com uma variavel de folga a mais, ou seja, precisa-se ter cuidado na hr de colocar p dentro
            //se esse for fosse feito com tab->matriz[i][j] daria errado pq ele ia colocar a coluna de resultado nessa parte
        }

        tab->matriz[i][tab->colunas - 1] = atual->matriz[i][atual->colunas - 1]; //aqui ele copia a coluna resultado do tableau antigo pro novo na posicao correta
    }

    int new_lin_obj = tab->linhas - 1;
    int old_lin_onj = atual->linhas - 1;

    for(int i = 0; i<var_originais; i++){
        tab->matriz[new_lin_obj][i] = atual->matriz[old_lin_onj][i];
    }

    tab->matriz[new_lin_obj][tab->colunas-1] = atual->matriz[old_lin_onj][atual->colunas-1];

    int nova_linha_restricao = tab->linhas - 2; //logo atras da linha objetivo
    
    //flag pra aplicar o sinal da restricao (se for 1 ele aplica)
    //ex: simplex para em 2.73 e eu quero ramificar pra 2 e 3 (tenho que banir os valores quebrados)

    if(maior_ou_igual == 1){ //se a restricao for >= eu tenho que fazer esse truque aí de multiplicar a eq inteira por -1 e nao quebrar o simplex
    
        tab->matriz[nova_linha_restricao][indice_variavel] = -1.0;
        tab->matriz[nova_linha_restricao][tab->colunas - 1] = -limite;
    }

    else{//se a restricao for menor ou igual eu posso jogar direto na matriz ex: x<=2
        tab->matriz[nova_linha_restricao][indice_variavel] = 1.0;
        tab->matriz[nova_linha_restricao][tab->colunas-1] = limite;
    }

    int nova_folga = var_originais + restricoes_antigas;
    tab->matriz[nova_linha_restricao][nova_folga] = 1.0;

    return tab;
}

resultado branch_and_bound(tableau *t, int var_originais){
    resultado r;
    r.tem_solucao = 0;
    r.variaveis = var_originais;
    r.valores = (double*) calloc(var_originais, sizeof(double));

    int viabilidade = executar(t);
    
    if(viabilidade == 0){
        return r;
    }

    else{
        int indice_variavel;
        double valor_fracionario;

        if(encontrar_fracao(t,var_originais, &indice_variavel, &valor_fracionario) == 0){
            r.tem_solucao = 1;

            for(int i = 0;  i<var_originais; i++){
                r.valores[i] = extrair(t, i);//i como coluna fe
            }
            return r;
        }

        double piso = floor(valor_fracionario); //se eu trabalho com o piso, a restricao eh com <=
        double teto = ceil(valor_fracionario);//se eu tralho com teto, a restricao eh com >=

        tableau *esq = add_restricao(t, indice_variavel, piso, 0);
        resultado r_esq = branch_and_bound(esq, var_originais);
        free_tab(esq);

        tableau *dir = add_restricao(t, indice_variavel, teto, 1);
        resultado r_dir = branch_and_bound(dir, var_originais);
        free_tab(dir);

        free(r.valores);


        if(r_esq.tem_solucao == 1 && r_dir.tem_solucao == 1){ //avalia qual dos valores (esq ou dir) retorna o maior possivel para a funcao objetivo
            double esq = 0;
            double dir = 0;
            int lin_fo = t->linhas - 1;

            for(int i = 0; i < var_originais; i++){
                esq += t->matriz[lin_fo][i] *r_esq.valores[i];
                dir += t->matriz[lin_fo][i] *r_dir.valores[i];
            }

            if(esq >= dir){
                free(r_dir.valores);
                return r_esq;
            }
            else{
                free(r_esq.valores);
                return r_dir;
            }
        }
        else if(r_esq.tem_solucao == 1){
            free(r_dir.valores);
            return r_esq;
        }
        else{
            free(r_esq.valores);
            return r_dir;
        }
    }
}