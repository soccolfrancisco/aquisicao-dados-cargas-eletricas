#include<stdio.h>
#include<stdlib.h>
#define MAX 5000

void arquivo(void);
void arquivoCorrente(void);
void arquivoTensao(void);
void iniciaVet(void);
void calculo(void);

int tensao[MAX], corrente[MAX];
float tensaoReal[MAX], correnteReal[MAX], tensaoArduino[MAX], correnteArduino[MAX];

int main(){
    iniciaVet();
    arquivo();
    calculo();
    arquivoCorrente();
    arquivoTensao();
    printf("\n\nCONVERSAO REALIZADA COM SUCESSO!\n\n", cont);
}

void arquivo(void){
    FILE *file;
    file = fopen("10001.txt", "r");
    int i = 0;

    if(file == NULL){
        printf("Erro ao abrir arquivo");
    }

    while(fscanf(file, "%d %d", &corrente[i], &tensao[i]) != EOF){
        i++;
    }

    fclose(file);
}

void arquivoCorrente(void){
    FILE *fileCor;
    fileCor = fopen("C10001.txt", "w");
    int i;

    for(i = 0; i < MAX; i++){
        if(correnteReal[i] != -1){
            fprintf(fileCor,"%.3f ",correnteReal[i]);
        }
    }

    fclose(fileCor);
}

void arquivoTensao(void){
    FILE *fileTen;
    fileTen = fopen("T10001.txt", "w");
    int i;

    for(i = 0; i < MAX; i++){
        if(tensaoReal[i] != -1){
            fprintf(fileTen,"%.3f ",tensaoReal[i]);
        }
    }

    fclose(fileTen);
}

void iniciaVet(void){
    int i;

    for(i = 0; i < MAX; i++){
        tensao[i] = -1;
        tensaoReal[i] = -1;
        tensaoArduino[i] = -1;
        corrente[i] = -1;
        correnteReal[i] = -1;
        correnteArduino[i] = -1;
    }
}

void calculo(void){
    int i, qtdeVoltasSensor = 2;

    for(i = 0; i < MAX; i++){
        if(tensao[i] != -1){
            tensaoArduino[i] = (3.3 * (tensao[i] - 500)) / 4095;
            correnteArduino[i] = (3.3 * (corrente[i] - 500)) / 4095;
        }
    }

    for(i = 0; i < MAX; i++){
        if(tensaoArduino[i] != -1){
            tensaoReal[i] = (3 * tensaoArduino[i] / (47 * 2.5)) * 15000;
            correnteReal[i] = ((2.5 * correnteArduino[i] / 270) * 1000) / qtdeVoltasSensor;
        }
    }
}
