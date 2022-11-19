//arquivo para testar testar as funções do trabalho

#include <stdio.h>
#include <stdlib.h>
#define MAXT 1000

typedef int (*funcp) ();

//Protótipos:
funcp geraCodigo (FILE *f, unsigned char codigo[]);
static void error (const char *msg, int line);

int main(void){

    FILE * fp = fopen("implementacao1.txt","r");
    unsigned char Vetor[MAXT];
    geraCodigo(fp,Vetor);
    return 0;
}

funcp geraCodigo(FILE *f, unsigned char codigo[]){
    funcp Funcp;
    char c;
    int line =1;

    while ((c = fgetc(f)) != EOF) {
        // printf("%c\n",c); //verificando se estava lendo letra por letra
        switch(c){
            case 'r':;
                char var0;
                int idx0;
                 if (fscanf(f, "et %c%d", &var0, &idx0) != 2)
                    error("comando invalido", line);
                else
                    printf("caso de retorno -> ok");
                break;
            default: error("comando desconhecido", line);
        }
    }
    return Funcp;
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}