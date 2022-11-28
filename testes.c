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

    FILE *fp = fopen("implementacao6.txt", "r");
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
    int bytesPerLine[20]; //vetor onde cada elemento é o byte em que se inicia cad linha. Ex: bytesPerLine[0] é igual as 9 -> linha 1 se inicia no nono byte do codugo de maquina
   
    // Criando o RA da função:
    codigo[i++] = 0x55;
    codigo[i++] = 0x48;
    codigo[i++] = 0x89;
    codigo[i++] = 0xe5;
    // Abrindo espaço no RA para até 4 variaveis locais
    codigo[i++] = 0x48;
    codigo[i++] = 0x83;
    codigo[i++] = 0xec;
    codigo[i++] = 0x10;
    bytesPerLine[0] = i+1;
    while ((c = fgetc(f)) != EOF)
    {
        
        
        printf("%c\n", c);
        if (c != '\n')
        {
            switch (c) //c = primeira letra lida na linha
            {
            case 'r':;
                char var;
                int IntVal;
                if (fscanf(f, "et %c%d", &var, &IntVal) != 2)
                    error("comando invalido", line);
                switch (var)
                {
                case '$':

                    codigo[i++] = 0xb8;
                    int *ip = (int *)&codigo[i];
                    *(ip) = IntVal;
                    i += 4;
                    codigo[i++] = 0x5d;
                    codigo[i++] = 0xc3;
                    bytesPerLine[line] = (i+1);
                    break;

                case 'v':

                    codigo[i++] = 0x8b;
                    codigo[i++] = 0x45;
                    writeLocalVar(IntVal, line, codigo);
                    codigo[i++] = 0xc9;
                    codigo[i++] = 0xc3;
                    bytesPerLine[line] = i+1;
                    break;

                case 'p':
                    writePar(IntVal, line, codigo);
                    codigo[i++] = 0x5d;
                    codigo[i++] = 0xc3;
                    bytesPerLine[line] = i+1;
                }
                break;
            case 'v': // atribuição e operação aritmética
                var = c;
                char c1, var1, var2, igual;
                int IntVal1, IntVal2;
                if (fscanf(f, "%d %c", &IntVal, &c1) != 2) //c1 = operador
                {

                    error("comando invalido", line);
                }
                switch (c1)
                {
                case ':':
                    // atribuição
                    if (fscanf(f, " %c %c%d", &igual, &var1, &IntVal1) != 3)
                    {
                        error("comando invalido", line);
                    }
                    switch (var1)
                    {
                    case '$':
                        // atribuição de constante
                        codigo[i++] = 0xc7;
                        codigo[i++] = 0x45;
                        writeLocalVar(IntVal, line, codigo);
                        int *ip = (int *)&codigo[i];
                        *(ip) = IntVal1;
                        i += 4;
                        printf("iiiii: %d\n", i);
                        bytesPerLine[line] = (i+1);
                        break;

                    case 'v':
                        // atribuição de variável
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x8b;
                        codigo[i++] = 0x55;
                        writeLocalVar(IntVal1, line, codigo);
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        codigo[i++] = 0x55;
                        writeLocalVar(IntVal1, line, codigo);
                        bytesPerLine[line] = (i+1);
                        break;

                    case 'p':

                        if (IntVal1 > MAXPAR)
                            error("Numero máximo de paremetros atingido", line);
                        codigo[i++] = 0x89;
                        switch (IntVal1)
                        {
                        case 1: // primeiro parâmetro
                            codigo[i++] = 0x7d;
                            writeLocalVar(IntVal,line,codigo);
                            break;
                        case 2: // segundo parâmetro
                            codigo[i++] = 0x75;
                            writeLocalVar(IntVal,line,codigo);
                            break;
                        }
                        bytesPerLine[line] = (i+1);
                        break;
                    default:
                    {

                        error("comando desconhecido", line);
                    }
                    }
                    break;
                case '*': // caso produto

                    printf("entrou produto\n");
                    if (fscanf(f, "%c %c%d", &igual, &var2, &IntVal2) != 3)
                    {
                        error("comando invalido", line);
                    }
                    switch(var2)
                    {
                        case '$': //No caso da multiplicação, passamos o valor dá variavél para o reg %r10d para fazer o imul. 
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x55;     
                            writeLocalVar(IntVal,line,codigo);
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x69;
                            codigo[i++] = 0xd2; //especifico para o r10d
                            int *ip = (int *)&codigo[i];
                            *(ip) = IntVal2;
                            i += 4;
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x55;
                            writeLocalVar(IntVal,line,codigo);
                            break;
                        case 'v':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal,line,codigo);
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal2,line,codigo);
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x0f;
                            codigo[i++] = 0xaf;
                            codigo[i++] = 0xc1;              // 45 01 c8 //45 29 c8 //45 0f af c1

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo);
                            break;               
                        case 'p':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal, line, codigo);
                            codigo[i++] = 0x41;
                            codigo[i++] = 0x89;
                            if (IntVal2 ==1)
                                codigo[i++] = 0xf8;
                            if (IntVal2 ==2)
                                 codigo[i++] = 0xf0;
                            
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x0f;
                            codigo[i++] = 0xaf;     //45 0f af c1
                            codigo[i++] = 0xc1;

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal, line, codigo);
                            break;
                    }
                    break;

                case '+': // caso soma
                    printf("entrou soma\n");
                    if (fscanf(f, "%c %c%d", &igual, &var2, &IntVal2) != 3)
                    {
                        error("comando invalido", line);
                    }
                    switch(var2)
                    {
                        case '$':
                            codigo[i++] = 0x81;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo);
                            printf("\n\nIntVal1: %d",IntVal2);
                            int *ip = (int *)&codigo[i];
                            *(ip) = IntVal2;
                            i += 4;
                            break;
                        case 'v':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal,line,codigo);
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal2,line,codigo);
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x01;
                            codigo[i++] = 0xc8; 

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo);
                        case 'p':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal, line, codigo);
                            codigo[i++] = 0x41;
                            codigo[i++] = 0x89;
                            if (IntVal2 ==1)
                                codigo[i++] = 0xf8;
                            if (IntVal2 ==2)
                                 codigo[i++] = 0xf0;
                            
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x01;
                            codigo[i++] = 0xc8;

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal, line, codigo);
                            break;
                    }
                    
                   
                    
                    break;

                case '-': // case sub
                    printf("entrou sub\n");
                    if (fscanf(f, "%c %c%d", &igual, &var2, &IntVal2) != 3)
                    {
                        error("comando invalido", line);
                    }  
                    switch(var2)
                    {
                        case '$':
                            codigo[i++] = 0x81;
                            codigo[i++] = 0x6d;
                            writeLocalVar(IntVal,line,codigo);
                            printf("\n\nIntVal1: %d",IntVal2);
                            int *ip = (int *)&codigo[i];
                            *(ip) = IntVal2;
                            i += 4;
                            break;
                        case 'v':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal,line,codigo);
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal2,line,codigo);
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x29;
                            codigo[i++] = 0xc8; // 45 01 c8 //45 29 c8

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo);
                        case 'p':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal, line, codigo);
                            codigo[i++] = 0x41;
                            codigo[i++] = 0x89;
                            if (IntVal2 ==1)
                                codigo[i++] = 0xf8;
                            if (IntVal2 ==2)
                                 codigo[i++] = 0xf0;
                            
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x29;
                            codigo[i++] = 0xc8; //45 29 c8

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal, line, codigo);
                            break;
                    }
                    break;

                }
                break;

            case 'p':
                //fazer operação e atribuição, vai ser muito parecido com o case "v"
                break;

            case 'g':;
                int nLinha;
                if (fscanf(f, "o %d", &nLinha) != 1)
                {
                    error("comando invalido", line);
                }
                break;

            case 'i':;
                printf("entrou caso if\n");
                char type;
                int valNum, n1,n2;
                 if (fscanf(f, "f %c%d %d %d", &type, &valNum, &n1, &n2) != 4)
                    {
                        error("comando invalido", line);
                    }
                break;
            }
        }
        else
        line++;
    }

    for (int c = 0; c < 70; c++)
    {
        printf("%02x ",codigo[c]);
        //printf("\n\n\nInicio linha %d : %d ",(c+1), bytesPerLine[c]);
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

