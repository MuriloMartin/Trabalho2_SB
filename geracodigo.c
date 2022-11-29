
//Murilo_Martin   1921063 33B
//Stella_Maranhão 2111494 33B

#include <stdio.h>
#include <stdlib.h>
#define MAXT 1000
#define MAXVAR 4
#define MAXPAR 2

typedef int (*funcp)();
int i;


static void error(const char *msg, int line)
{
    fprintf(stderr, "erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}

struct Go{
    int desvio[30];
    int line;
};

void writeLocalVar(int n, int line, unsigned char codigo[])
{
    if (n > MAXVAR)
        error("Numero máximo de variáveis locais atingido", line);
    codigo[i++] = 0xff - 4 * n + 1;
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

funcp geraCodigo(FILE *f, unsigned char codigo[])
{
    funcp Funcp;
    char c;
    int line = 1; 
    i = 0;
    long int lineAddress[20]; //vetor onde cada elemento é o o endereço do inicio de cada linha
    

    struct Go jump;


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
    
    while ((c = fgetc(f)) != EOF)
    {  
        lineAddress[line-1]=(long)(&codigo[i]);

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
                    codigo[i++] = 0xc9;
                    codigo[i++] = 0xc3;
     
                    break;

                case 'v':

                    codigo[i++] = 0x8b;
                    codigo[i++] = 0x45;
                    writeLocalVar(IntVal, line, codigo);
                    codigo[i++] = 0xc9;
                    codigo[i++] = 0xc3;
                  
                    break;

                case 'p':

                    writePar(IntVal, line, codigo);
                    codigo[i++] = 0xc9; //0x5d, 0xc3
                    codigo[i++] = 0xc3;
                   
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
                case ':':           //
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
                        writeLocalVar(IntVal, line, codigo);        //atribuindo direto a constante para a psoição certa no ra,     onde ficam as variaveis locais
                        int *ip = (int *)&codigo[i];
                        *(ip) = IntVal1;
                        i += 4;
                       
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
                        writeLocalVar(IntVal, line, codigo);
                      

                        break;

                    case 'p': //move direto para o ra(onde estão as variáveis locais), sem usar registradores auxiares

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
                  
                        break;
                    default:
                    {

                        error("comando desconhecido", line);
                    }
                    }
                    break;
                case '*': // caso produto


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
                            codigo[i++] = 0xd2;             //especifico para o r10d
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
                            writeLocalVar(IntVal,line,codigo); //passa v1 para o %r9d
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal2,line,codigo); //passa v2 para o %r8d
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x0f;
                            codigo[i++] = 0xaf;
                            codigo[i++] = 0xc1;              //imul %r9d , %r8d

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo); //passa %r8d para o local certo no Ra
                            break;               
                        case 'p':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal, line, codigo); //passa v1 para o %r9d
                            codigo[i++] = 0x41;
                            codigo[i++] = 0x89;
                            if (IntVal2 ==1)
                                codigo[i++] = 0xf8;
                            if (IntVal2 ==2)                    // passa %edi ou %esi para %r8d
                                 codigo[i++] = 0xf0;
                            
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x0f;
                            codigo[i++] = 0xaf;     //imul %r9d , %r8d
                            codigo[i++] = 0xc1;

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal, line, codigo); //passa %r8d para o local certo no Ra
                            break;
                    }
                    break;

                case '+': // caso soma

                    if (fscanf(f, "%c %c%d", &igual, &var2, &IntVal2) != 3)
                    {
                        error("comando invalido", line);
                    }
                    switch(var2)
                    {
                        case '$':
                            codigo[i++] = 0x81;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo);  //faz o addl e guarda o resultado ja na posição do Ra correspondente à variável
                            int *ip = (int *)&codigo[i];
                            *(ip) = IntVal2;
                            i += 4;
                            break;
                        case 'v':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal,line,codigo); //passa v1 para r9d
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal2,line,codigo); //passa v2 %r8d
                            
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x01;
                            codigo[i++] = 0xc8;                 //opera com r9d e r8d

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal,line,codigo);
                        case 'p':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal, line, codigo); //passa v1 para o %r9d
                            codigo[i++] = 0x41;
                            codigo[i++] = 0x89;
                            if (IntVal2 ==1)
                                codigo[i++] = 0xf8;
                            if (IntVal2 ==2)                    // passa %edi ou %esi para %r8d
                                 codigo[i++] = 0xf0;
                            
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x01;
                            codigo[i++] = 0xc8;             //opera com %r8d e %r9d

                            codigo[i++] = 0x44;
                            codigo[i++] = 0x89;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal, line, codigo); //passa %r8d para o local certo no Ra
                            break;
                    }
                    
                   
                    
                    break;

                case '-': // case sub

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
                            int *ip = (int *)&codigo[i];
                            *(ip) = IntVal2;
                            i += 4;     //faz o addl e guarda o resultado ja na posição do Ra correspondente à variável
                            break;
                        case 'v':
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x4d;
                            writeLocalVar(IntVal,line,codigo); //passa v1 para %r9d
                            codigo[i++] = 0x44;
                            codigo[i++] = 0x8b;
                            codigo[i++] = 0x45;
                            writeLocalVar(IntVal2,line,codigo); //passa v2 %r8d
                            codigo[i++] = 0x45;
                            codigo[i++] = 0x29;
                            codigo[i++] = 0xc8; //opera com r9d e r8d

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
                            writeLocalVar(IntVal, line, codigo); //passa %r8d para o local certo no Ra
                            break;
                    }
                    break;

                }
                break;

            case 'p': 
                if (fscanf(f, "%d %c", &IntVal, &c1) != 2) //c1 = operador, IntVal = 1 ou 2 (p1 e p2)
                {

                    error("comando invalido", line);
                }
                switch (c1)
                {
                    case ':': //passar sempre varp para %r9d e varpc para %r10d
                    if (fscanf(f, " %c %c%d", &igual, &var1, &IntVal1) != 3) //var1 -> p,v ou $ IntVal1 -> numero da variavel ou parametro ou valor da constante
                    {
                        error("comando invalido", line);
                    }
                    switch (var1)
                    {
                    case '$':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1; 
                        codigo[i++] = 0x41;
                        codigo[i++] = 0xba;
                        int *ip = (int *)&codigo[i];
                        *(ip) = IntVal1;
                        i += 4;
                        codigo[i++] = 0x45;
                        codigo[i++] = 0x89;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce; 
                       
                        break;

                        case 'v':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1; 
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x8b;
                        codigo[i++] = 0x55;
                        writeLocalVar(IntVal1,line, codigo);
                        codigo[i++] = 0x45;
                        codigo[i++] = 0x89;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce; 
                        break;

                        case 'p':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;

                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                         if(IntVal1 == 1)
                            codigo[i++] = 0xfa;   
                        if(IntVal1 == 2)
                            codigo[i++] = 0xf2;
                        codigo[i++] = 0x45;
                        codigo[i++] = 0x89;
                        codigo[i++] = 0xd1;
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;
                        break;

                    }
                    break;
                    
                    case '*':
                     if (fscanf(f, " %c %c%d", &igual, &var1, &IntVal1) != 3) //var1 -> p,v ou $ IntVal1 -> numero da variavel ou parametro ou valor da constante
                    {
                        error("comando invalido", line);
                    }
                    switch (var1)
                    {
                        case '$':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;
                        codigo[i++] = 0x41;
                        codigo[i++] = 0xba;
                        int *ip = (int *)&codigo[i];
                        *(ip) = IntVal1;
                        i += 4;

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x0f;
                        codigo[i++] = 0xaf;
                        codigo[i++] = 0xca;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;

                        case 'v':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x8b;
                        codigo[i++] = 0x55;
                        writeLocalVar(IntVal1,line, codigo);

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x0f;
                        codigo[i++] = 0xaf;
                        codigo[i++] = 0xca;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;
                        
                        case 'p':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;

                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                         if(IntVal1 == 1)
                            codigo[i++] = 0xfa;   
                        if(IntVal1 == 2)
                            codigo[i++] = 0xf2;

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x0f;
                        codigo[i++] = 0xaf;
                        codigo[i++] = 0xca;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;
                    }

                    break;

                    case '+':

                    if (fscanf(f, " %c %c%d", &igual, &var1, &IntVal1) != 3) //var1 -> p,v ou $ IntVal1 -> numero da variavel ou parametro ou valor da constante
                    {
                        error("comando invalido", line);
                    }
                    switch (var1)
                    {   
                        
                        case '$':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;
                        codigo[i++] = 0x41;
                        codigo[i++] = 0xba;
                        int *ip = (int *)&codigo[i];
                        *(ip) = IntVal1;
                        i += 4;
                        codigo[i++] = 0x45;
                        codigo[i++] = 0x01;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;
                        break;

                        case 'v':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x8b;
                        codigo[i++] = 0x55;
                        writeLocalVar(IntVal1,line, codigo);

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x01;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;

                        case 'p':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;

                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                         if(IntVal1 == 1)
                            codigo[i++] = 0xfa;   
                        if(IntVal1 == 2)
                            codigo[i++] = 0xf2;

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x01;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;
                    }


                    break;

                    case '-':
                    if (fscanf(f, " %c %c%d", &igual, &var1, &IntVal1) != 3) //var1 -> p,v ou $ IntVal1 -> numero da variavel ou parametro ou valor da constante
                    {
                        error("comando invalido", line);
                    }
                    switch(var1)
                    {
                        case '$':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;
                        codigo[i++] = 0x41;
                        codigo[i++] = 0xba;
                        int *ip = (int *)&codigo[i];
                        *(ip) = IntVal1;
                        i += 4;

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x29;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;

                        case 'v':
                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;
                        codigo[i++] = 0x44;
                        codigo[i++] = 0x8b;
                        codigo[i++] = 0x55;
                        writeLocalVar(IntVal1,line, codigo);

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x21;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;


                        break;

                        case 'p':
                         codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xf9;   
                        if(IntVal == 2)
                            codigo[i++] = 0xf1;

                        codigo[i++] = 0x41;
                        codigo[i++] = 0x89;
                         if(IntVal1 == 1)
                            codigo[i++] = 0xfa;   
                        if(IntVal1 == 2)
                            codigo[i++] = 0xf2;

                        codigo[i++] = 0x45;
                        codigo[i++] = 0x29;
                        codigo[i++] = 0xd1;

                        codigo[i++] = 0x44;
                        codigo[i++] = 0x89;
                        if(IntVal == 1)
                            codigo[i++] = 0xcf;   
                        if(IntVal == 2)
                            codigo[i++] = 0xce;

                        break;
                    }

                    break;
                }
                break;

            case 'g':;
                int nLinha;
                if (fscanf(f, "o %d", &nLinha) != 1)
                {
                    error("comando invalido", line);
                }
                codigo[i++] = 0xe9; //usando a instrução do jump "longo"
                codigo[i++] = 0x00;
                codigo[i++] = 0x00;
                codigo[i++] = 0x00;
                codigo[i++] = 0x00;
                jump.desvio[jump.line] = nLinha; 
                jump.line++;
                printf("saiu do go");
                break;

            case 'i':;
 
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

    i=0;    
    jump.line=0;
    int offset;
    
    while(codigo[i]!=0xc9){    //percorre até a intrução leave
                             
            if(codigo[i] == 0xe9){
            i+=5;                            //pula para o linha seguinte para pegar o endereço e fazer o offset
            offset = lineAddress[jump.desvio[jump.line]-1]-(long)&codigo[i];
            i-=4;  
            jump.line++;                             //volta no vetor código para sobreescreber os valores zerados do comando de jump
            codigo[i++] = 0xff&offset;
            codigo[i++] = (char)(0xff00&offset)>>8;        //escrevendo byte a byte, começãdo pelo menos significativo
            codigo[i++] = (char)(0xff0000&offset)>>16;
            codigo[i++] = (char)(0xff000000&offset)>>24;
            
           
            }

            i++;
        }  

    
    for (int c = 0; c < 110; c++)
    {
        printf("%02x ",codigo[c]);

    }
    Funcp = (funcp)codigo;
    return Funcp ;
}


