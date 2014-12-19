/*/////////////////////////////////////////////////////////////////////////////////*/
//  Alunos: Natan J. Mai, Ricardo Zanuzzo e Rogério Torchelsen                    //
//  Disciplina: Banco de Dados II                                                 //      
//  Curso: Ciência da Computação                                                  //      
//  Universidade Federal da Fronteira Sul                                         //      
//                                                                                //      
//////////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffend.h"
#include "funcoesNRR.h"


/***********************************************************************************|
|* FUNÇÃO: Utilizada na impressão das tabelas, conforme nomeTabela                 */   
    
void imprime(char nomeTabela[]) {

    int j,erro, x;
    struct fs_objects objeto = leObjeto(nomeTabela);    
    
    tp_table *esquema = leSchema(objeto);

    if(esquema == ERRO_ABRIR_ESQUEMA){
        printf("Erro ao criar o esquema.\n");
        return;
    }

    tp_buffer *bufferpoll = initbuffer();

    if(bufferpoll == ERRO_DE_ALOCACAO){
        printf("Erro ao alocar memória para o buffer.\n");
        return;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);        
    

    column *pagina = getPage(bufferpoll, esquema, objeto, 0);

    if(pagina == ERRO_PARAMETRO){
        printf("Tabela Vazia\n");
        return ;
    }

    // PARA IMPRIMIR PÁGINA
    printf("Número de tuplas: %d\n", bufferpoll[0].nrec);
    for(j=0; j < objeto.qtdCampos*bufferpoll[0].nrec; j++){
        
        if(pagina[j].tipoCampo == 'S')
            printf("%s: %-15s ", pagina[j].nomeCampo,pagina[j].valorCampo);
        else if(pagina[j].tipoCampo == 'I'){
            int *n = (int *)&pagina[j].valorCampo[0];
            printf("%s: %-15d ",pagina[j].nomeCampo, *n);
        }
        else if(pagina[j].tipoCampo == 'C'){
            printf("%s: %-15c ",pagina[j].nomeCampo, pagina[j].valorCampo[0]);
        }
        else if(pagina[j].tipoCampo == 'D'){
            double *n = (double *)&pagina[j].valorCampo[0];
            printf("%s: %-15f ",pagina[j].nomeCampo, *n);
        }
        if(j>=1 && ((j+1)%objeto.qtdCampos)==0){
            printf("\n");
        }
    }
    printf("\n\n");
}

        
/***********************************************************************************|
|* FUNÇÃO: Verifica a existência do arquivo da tabela 'filename'.                  */   
    
int existeArquivo(const char* filename){
    FILE* fptr = fopen(filename, "r");
    if (fptr != NULL){
        fclose(fptr);
        
        return 1;
    }

    return 0;
}

/***********************************************************************************|
|* FUNÇÃO: Organiza o arquivo para remover espaços vazios                          */   

void ProcuraNoArquivo(char *nomeTabela){
    int tam, teste;
    FILE *dicionario;
    char *tupla = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);
    if((dicionario = fopen("fs_object.dat","a+b")) == NULL)
        return ERRO_ABRIR_ARQUIVO;

    teste = 0;

    while(fgetc (dicionario) != EOF){
        
        fread(tupla, sizeof(char), TAMANHO_NOME_TABELA, dicionario); //Lê somente o nome da tabela
        tam = strlen(tupla);
        
        if(strcmp(tupla, nomeTabela) == 0){ // Verifica se o nome dado pelo usuario existe no dicionario de dados.
            int cont = 0;
            fseek(dicionario, 1, 1);            
            printf("TESTE: %d\n", teste);
            fseek(dicionario, 1, SEEK_SET);
            fwrite("ssss", 4 * sizeof(char), SEEK_SET, dicionario);

            while(cont < tam){
                printf("CONT %c\n", tupla[cont]);
                fwrite('a', sizeof(char), teste, dicionario);
                tupla[cont] = '\0';
                cont++;
            }

            printf("TUPLA: %s\n", tupla);
            return 1;
        }
        
        teste = sizeof(tupla);    
        fseek(dicionario, 28, 1);
    }

    fclose(dicionario);
    return 0;
}


/***********************************************************************************|
|* FUNÇÃO: Exclui a tabela com 'nome'                                              */   

void excluirArquivo(char *nomeTabela){
	struct fs_objects objeto;
	tp_table *esquema;
	int x,erro, cont;
    char str[20]; 
    char dat[5] = ".dat";
    char a = 'a';
    
    strcpy (str, nomeTabela); 
    strcat (str, dat);              //Concatena e junta o nome com .dat

    if(!existeArquivo(str)){
        system("clear");
        printf("Arquivo não existe!\n");
        return;
    }
    
	abreTabela(nomeTabela, &objeto, &esquema);
	tp_buffer *bufferpoll = initbuffer();
	if(bufferpoll == ERRO_DE_ALOCACAO){
        printf("Erro ao alocar memória para o buffer.\n");
        return;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);        
    

    column *pagina = getPage(bufferpoll, esquema, objeto, 0);

    if(pagina == ERRO_PARAMETRO){
        printf("Tabela Vazia\n");
        return ;
    }
    
    printf("NOME do arquivo: %s\n", nomeTabela);
    ProcuraNoArquivo(nomeTabela);

    /*
    cont = 0;
    
    printf("sssssssss%d\n", strlen(objeto.nome));
    while(cont < strlen(objeto.nome)){
        objeto.nome[cont] = '\0';
        cont++;
    }
        
    printf("NOMEss do arquivo: %s\n", objeto.nome);*/


	//column *tuplaE = excluirTuplaBuffer(bufferpoll, esquema, objeto, 0, 2); //pg, tupla
	
    remove(str);
    
    return;
}


/***********************************************************************************|
|* FUNÇÃO: Inicia os atributos utilizados pela FK e PK. Além de erros.             */   

int iniciaAtributos(struct fs_objects *objeto, tp_table **tabela, tp_buffer **bufferpoll, char *nomeT){
	
    *objeto     = leObjeto(nomeT);
    *tabela     = leSchema(*objeto);
    *bufferpoll = initbuffer();

    if(*tabela == ERRO_ABRIR_ESQUEMA)
        return ERRO_DE_PARAMETRO;

    if(*bufferpoll == ERRO_DE_ALOCACAO)
        return ERRO_DE_PARAMETRO;

    return SUCCESS;
}

int existeAtributo(char *nomeTabela, char *atributo){
    int erro, x;
    struct fs_objects objeto; 
    tp_table *tabela;         
    tp_buffer *bufferpoll;
    

    //if(iniciaAtributos(&objeto, &tabela, &bufferpoll, nomeTabela) != SUCCESS) 
        //return ERRO_DE_PARAMETRO;
	
	objeto     = leObjeto(nomeTabela);
    tabela     = leSchema(objeto);
    bufferpoll = initbuffer();


    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);        

    column *pagina = getPage(bufferpoll, tabela, objeto, 0);
    
     if(pagina == ERRO_PARAMETRO){
        return 0;
    }
   

    for(x = 0; x < objeto.qtdCampos*bufferpoll[0].nrec; x++){
        if(strcmp(pagina[x].nomeCampo, atributo) == 0)
            return SUCCESS;
    }
 
    return ERRO_DE_PARAMETRO;
}

/***********************************************************************************|
|* FUNÇÃO: Verifica as condições para chave estrangeira (FK)                       */   

int verificaChaveFK(char *nomeTabela, char *nomeCampo, char *valorCampo, char *tabelaApt, char *attApt){
    int x,j, erro;
    char str[20]; 
    char dat[5] = ".dat";
    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;
    
    strcpy (str, tabelaApt); 
    strcat (str, dat);              //Concatena e junta o nome com .dat
    
    erro = existeAtributo(nomeTabela, nomeCampo);
    if(erro != SUCCESS )
        return ERRO_DE_PARAMETRO;
    
    if(existeAtributo(tabelaApt, attApt))
        return ERRO_CHAVE_ESTRANGEIRA;

    if(iniciaAtributos(&objeto, &tabela, &bufferpoll, tabelaApt) != SUCCESS)
        return ERRO_DE_PARAMETRO;
       
      

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);        

    column *pagina = getPage(bufferpoll, tabela, objeto, 0);


    for(j = 0; j < objeto.qtdCampos * bufferpoll[0].nrec; j++){		
		 
		 
        if(strcmp(pagina[j].nomeCampo, nomeCampo) == 0){
			
            if(pagina[j].tipoCampo == 'S'){     
                if(strcmp(pagina[j].valorCampo, valorCampo) == 0){
                    return SUCCESS;
                }
            }

            else if(pagina[j].tipoCampo == 'I'){ 
				
                int *n = (int *)&pagina[j].valorCampo[0];

                if(*n == atoi(valorCampo)){
					
                    return SUCCESS;
                }
            }

            else if(pagina[j].tipoCampo == 'D'){ 
                double *nn = (double *)&pagina[j].valorCampo[0];

                if(*nn == atof(valorCampo)){
                    return SUCCESS;
                }
            }

            else if(pagina[j].tipoCampo == 'C'){                        
                if(pagina[j].valorCampo == valorCampo){
                    return SUCCESS;
                }
            }else {
				return ERRO_CHAVE_ESTRANGEIRA;
			}
        }            
    }

    return ERRO_CHAVE_ESTRANGEIRA;
}

/***********************************************************************************|
|* FUNÇÃO: Verifica as condições para chave primária (PK)                          */   

int verificaChavePK(char *nomeTabela, char *nomeCampo, char *valorCampo){
    int j, x, erro;
    
    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;
    
    erro = existeAtributo(nomeTabela, nomeCampo);
	if(erro != SUCCESS )
        return ERRO_DE_PARAMETRO;
    

    if(iniciaAtributos(&objeto, &tabela, &bufferpoll, nomeTabela) != SUCCESS) 
        return ERRO_DE_PARAMETRO;

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);        

    column *pagina = getPage(bufferpoll, tabela, objeto, 0);

    for(j = 0; j < objeto.qtdCampos * bufferpoll[0].nrec; j++){

        if(strcmp(pagina[j].nomeCampo, nomeCampo) == 0){
			
            if(pagina[j].tipoCampo == 'S'){        
                if(strcmp(pagina[j].valorCampo, valorCampo) == 0){
                    return ERRO_CHAVE_PRIMARIA;
                }
            }

            else if(pagina[j].tipoCampo == 'I'){ 
                int *n = (int *)&pagina[j].valorCampo[0];

                if(*n == atoi(valorCampo)){
                    return ERRO_CHAVE_PRIMARIA;
                }
            }

            else if(pagina[j].tipoCampo == 'D'){ 
                double *nn = (double *)&pagina[j].valorCampo[0];

                if(*nn == atof(valorCampo)){
                    return ERRO_CHAVE_PRIMARIA;
                }
            }

            else if(pagina[j].tipoCampo == 'C'){                        
                if(pagina[j].valorCampo == valorCampo){
                    return ERRO_CHAVE_PRIMARIA;
                }
            }
        }            
    }

    return SUCCESS;
}

/***********************************************************************************|
|* FUNÇÃO: recebe o nome de uma tabela e engloba as funções leObjeto() e leSchema() */   

int abreTabela(char *nomeTabela, struct fs_objects *objeto, tp_table **tabela){
	*objeto     = leObjeto(nomeTabela);
    *tabela     = leSchema(*objeto);
	
    return 1;
}	