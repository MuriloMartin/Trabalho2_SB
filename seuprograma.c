#include "geracodigo.c"

int main(void){
    FILE *fp = fopen("implementacao6.txt", "r");
    unsigned char Vetor[MAXT];
    geraCodigo(fp, Vetor);
    return 0;
}