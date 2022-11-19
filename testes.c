//arquivo para testar testar as funções do trabalho

#include <stdio.h>
#include <stdlib.h>
#define MAXT 1000
#define MAXVAR 4
#define MAXPAR 2

typedef int (*funcp) ();
int i;

//Protótipos:
funcp geraCodigo (FILE *f, unsigned char codigo[]);
static void error (const char *msg, int line);
void writeLocalVar(int n, int line, unsigned char codigo[]);
void writePar(int n, int line, unsigned char codigo[]);
//Mainnnnnnnnn////////////////////////////////////////////
int main(void){

    FILE * fp = fopen("implementacao1.txt","r");
    unsigned char Vetor[MAXT];
    geraCodigo(fp,Vetor);
    return 0;
}
//Mainnnnnnnnn////////////////////////////////////////////

funcp geraCodigo(FILE *f, unsigned char codigo[]){
    funcp Funcp;
    char c;
    int line =1;
    i=0;
    

    //Criando o RA da função:
    codigo[i++]=0x55; codigo[i++] = 0x48; codigo[i++] = 0x89 ; codigo[i++] = 0xe5;
    //Abrindo espaço no RA para até 4 variaveis locais
    codigo[i++] = 0x48; codigo[i++] = 0x83; codigo[i++] = 0xec; codigo[i++] = 0x20;
    while ((c = fgetc(f)) != EOF) {
        // printf("%c\n",c); //verificando se estava lendo letra por letra
        switch (c)
        {
        case 'r':;
            char var0;
            int idx0;
            if (fscanf(f, "et %c%d", &var0, &idx0) != 2)
                error("comando invalido", line);
            switch (var0) {
                    case '$': {                                
                        codigo[i++] = 0xb8;
                        int * ip = (int *)&codigo[i]   ;
                        *(ip) = idx0; 
                        break;
                    }
                    case 'v': {
                        codigo[i++] = 0x8b;
                        codigo[i++] = 0x45;
                        writeLocalVar(idx0,line,codigo);             
                        break;
                    }
                    case 'p':
                        writePar(idx0,line,codigo);
                }
            break;
        
        default:
            error("comando desconhecido", line);
        }
    }
    
    // for(int c=0;c<10;c++){

    //     printf("%hhx ",codigo[c]);
    // }
    // printf("\n\ni: %d",i);
    // return Funcp;
}

void writeLocalVar(int n, int line,unsigned char codigo[]){
    if (n>MAXVAR)
        error("Numero máximo de variáveis locais atingido", line);
    codigo[i++] = 0xff - 4 * n + 1;
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

void writePar(int n,int line,unsigned char codigo[]){
    
    if (n>MAXPAR)
        error("Numero máximo de parametros atingido", line);
    codigo[i++] = 0x89;
    if (n==1)
        codigo[i++] = 0xf8;
    else
        codigo[i++] = 0xf0;
}

