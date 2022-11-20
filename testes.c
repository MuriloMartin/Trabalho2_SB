// arquivo para testar testar as funções do trabalho

#include <stdio.h>
#include <stdlib.h>
#define MAXT 1000
#define MAXVAR 4
#define MAXPAR 2

typedef int (*funcp)();
int i;

// Protótipos:
funcp geraCodigo(FILE *f, unsigned char codigo[]);
static void error(const char *msg, int line);
void writeLocalVar(int n, int line, unsigned char codigo[]);
void writePar(int n, int line, unsigned char codigo[]);
// Mainnnnnnnnn////////////////////////////////////////////
int main(void)
{

    FILE *fp = fopen("implementacao5.txt", "r");
    unsigned char Vetor[MAXT];
    geraCodigo(fp, Vetor);
    return 0;
}
// Mainnnnnnnnn////////////////////////////////////////////

funcp geraCodigo(FILE *f, unsigned char codigo[])
{
    funcp Funcp;
    char c;
    int line = 1;
    i = 0;

    // Criando o RA da função:
    codigo[i++] = 0x55;
    codigo[i++] = 0x48;
    codigo[i++] = 0x89;
    codigo[i++] = 0xe5;
    // Abrindo espaço no RA para até 4 variaveis locais
    codigo[i++] = 0x48;
    codigo[i++] = 0x83;
    codigo[i++] = 0xec;
    codigo[i++] = 0x20;
    while ((c = fgetc(f)) != EOF)
    {
        printf("%c\n",c); 
        switch (c)
        {
        case 'r':;
            char var;
            int IntVal;
            if (fscanf(f, "et %c%d", &var, &IntVal) != 2)
                error("comando invalido", line);
            switch (var)
            {
            case '$':
            {
                codigo[i++] = 0xb8;
                int *ip = (int *)&codigo[i];
                *(ip) = IntVal;
                i += 4;
                break;
            }
            case 'v':
            {
                codigo[i++] = 0x8b;
                codigo[i++] = 0x45;
                writeLocalVar(IntVal, line, codigo);
                break;
            }
            case 'p':
                writePar(IntVal, line, codigo);
            }
            break;
        case 'v':; // atribuição e operação aritmética
            var = c;
            char  c1, var1, var2, igual;
            int IntVal1;
            if (fscanf(f, "%d %c", &IntVal, &c1 ) != 2){
                
                error("comando invalido", line);
            }
            switch (c1)
            {
            case ':':
            { // atribuição
                if (fscanf(f, " %c %c%d", &igual, &var1, &IntVal1) != 3){
                    printf("entrou aqui");
                    error("comando invalido", line);
                }
                switch (var1)
                {
                case '$':
                { // atribuição de constante
                    codigo[i++] = 0xc7;
                    codigo[i++] = 0x45;
                    writeLocalVar(IntVal, line, codigo);
                    int *ip = (int *)&codigo[i];
                    *(ip) = IntVal1;
                    i += 4;
                    break;
                }
                case 'v':
                { // atribuição de variável
                    codigo[i++] = 0x44;
                    codigo[i++] = 0x8b;
                    codigo[i++] = 0x55;
                    writeLocalVar(IntVal1, line, codigo);
                    codigo[i++] = 0x44;
                    codigo[i++] = 0x89;
                    codigo[i++] = 0x55;
                    writeLocalVar(IntVal1, line, codigo);
                    break;
                }
                case 'p':
                {
                    if (IntVal1 > MAXPAR)
                        error("Numero máximo de paremetros atingido", line);
                    codigo[i++] = 0x89;
                    switch (IntVal1)
                    {
                    case 1: // primeiro parâmetro
                        codigo[i++] = 0x7d;
                        break;
                    case 2: // segundo parâmetro
                        codigo[i++] = 0x75;
                        break;
                   
                    }
                }
                }
                
                break;
            }

            default:
                error("comando desconhecido", line);
            }
        }

    }
        for (int c = 0; c < 14; c++)
        {

            printf("%hhx ", codigo[c]);
        }
        printf("\n\ni: %d", i);
        return Funcp;
}
    void writeLocalVar(int n, int line, unsigned char codigo[])
    {
        if (n > MAXVAR)
            error("Numero máximo de variáveis locais atingido", line);
        codigo[i++] = 0xff - 4 * n + 1;
    }

    static void error(const char *msg, int line)
    {
        fprintf(stderr, "erro %s na linha %d\n", msg, line);
        exit(EXIT_FAILURE);
    }

    void writePar(int n, int line, unsigned char codigo[])
    {

        if (n > MAXPAR)
            error("Numero máximo de parametros atingido", line);
        codigo[i++] = 0x89;
        if (n == 1)
            codigo[i++] = 0xf8;
        else
            codigo[i++] = 0xf0;
    }
