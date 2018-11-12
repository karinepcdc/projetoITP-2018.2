/* 
Problemas:
- Código fica preso se na hora de escolher qual dos campos é o ID na função cria_tabela, nenhum dos campos fornecidos anteriormente for int. O código deve abortar e permitir que a pessoa tente criar a tabela novamente.

Melhorias:
- coversão de tipos pode retornar um string constane de modo a não precisar declarar variáveis para utilizá-lo?
- Minimizar a abertura de arquivos
- Mensagens de error de abertura de tabela mostram qual a linha e param o código

Dúvidas:
- No insereLinha_tab(), estamos conferindo se string passada é compatível com tipo do campo. Estamos fazendo da melhor maneira?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h> // macros dos limites dos tipos int, uint, char, entre outros

enum tipos_tab{int_=1, float_, double_, char_, string_};

typedef struct tabela_t{
  char nome[50];
  int C;
  char **tab_L; // linha da tabela
  enum tipos_tab *tipos; //char **tab_tipos; // tipos de cada campo da tabela
}tabela;


void conversao_tipos(enum tipos_tab TIPO, char str_tipo[10]){
  char tipos_all[][10]={"int", "float", "double", "char", "string"};
  strcpy(str_tipo, tipos_all[TIPO-1]);
}


// checagem se valor de campo é compatível com o tipo do campo
int checaLimite_campos(char campo[50], enum tipos_tab tipo_campo){
  int campoOK=1; // bool
  int i, n_pontos=0;
  double x_temp;
  char c_temp, tipo_temp[10];

  //  printf("%d: %d\n",__LINE__,campoOK);
  if(tipo_campo==int_){

      if(isdigit(campo[0])==0 && campo[0]!='+' && campo[0]!='-')
	campoOK=0;
      
      for(i=1;i<strlen(campo);i++){
	if(isdigit(campo[i])==0){ 
	  campoOK=0;
	  break;
	}		    	
      }
      
      if(campoOK){
	sscanf(campo,"%lf",&x_temp);
	if(x_temp<INT_MIN || x_temp>INT_MAX)
	  campoOK=0;
      }
      
    } else if(tipo_campo==float_){ 

    if(isdigit(campo[0])==0 && campo[0]!='+' && campo[0]!='-')
      campoOK=0;
    
    for(i=1;i<strlen(campo);i++){
      if(campo[i]=='.')
	n_pontos++;
      
      if((isdigit(campo[i])==0 && campo[i]!='.') || n_pontos>1){ 
	campoOK=0;
	break;
      }		    	
    }
    
    } else if(tipo_campo==double_){ 

    if(isdigit(campo[0])==0 && campo[0]!='+' && campo[0]!='-')
      campoOK=0;
    
    for(i=1;i<strlen(campo);i++){
      if(campo[i]=='.')
	n_pontos++;
      
      if((isdigit(campo[i])==0 && campo[i]!='.') || n_pontos>1){ 
	campoOK=0;
	break;
      }		    	
    }

    } else if(tipo_campo==char_){
      if(strlen(campo)>1 || isalpha(campo[0])==0)
	campoOK=0;

      if(campoOK){
	sscanf(campo,"%c",&c_temp);
	if(c_temp<CHAR_MIN || c_temp>CHAR_MAX)
	  campoOK=0;
      }
    }

  if(!campoOK){
    conversao_tipos(tipo_campo,tipo_temp);
    printf(">>> ERRO: Campo passado precisa ser do tipo %s.\n",tipo_temp);
  }
  
  return campoOK;
}


//Função para listar todas tabelas
void listagem_tab(){ 
  FILE *relacao_file;
  char nome[50];
  int n;


  printf("\n *** Tabelas existentes ***\n");

  relacao_file = fopen("relacaoTab", "r");

  while(fscanf(relacao_file,"%s %*d\n",nome)!=EOF){ //enquanto não for End of File, continue imprimindo
    printf("%s\n",nome);
  }

  if(relacao_file == NULL){
    printf("Relação de tabelas não existe!\n");
  }
  else{
    fclose(relacao_file);
  }

}

//Função para criar uma nova tabela
void cria_tab(){
  FILE *tabela_file, *relacao_file;
  tabela tabela_user;
  int i,j;
  char nome_campo[50];
  int tipo_campo; 
  int campoOK, idOK; // bool

  printf("Digite o nome da tabela: "); //printf("%d", __LINE__);
  scanf("%s", tabela_user.nome);
  strcat(tabela_user.nome,".tab");
  tabela_file = fopen(tabela_user.nome, "wr+"); 

  printf("Quantos campos irá inserir?\n");
  scanf("%d", &tabela_user.C); 

  //alocando vetores da struct
  tabela_user.tab_L=malloc(sizeof(char*)*tabela_user.C); //colunas alocadas
  for(int j=0; j<tabela_user.C; j++)
    tabela_user.tab_L[j]=malloc(sizeof(char)*50); // strings alocadas
  
  tabela_user.tipos=malloc(sizeof(enum tipos_tab)*tabela_user.C); //vetor de enum alocado

  j=0;
  do{
    tipo_campo=0;
    printf("\nInsira o campo[%d](sem espaços) e o seu tipo\n\n 1-int\n 2-float\n 3-double\n 4-char\n 5-string\n\n", j+1);
    scanf(" %s %d", nome_campo, &tipo_campo); 

    // verifique que os campos inseridos são únicos		
    campoOK=1;
    for(int i=0; i<j; i++){
      if(strcmp(tabela_user.tab_L[i],nome_campo)==0){
	campoOK=0;
	break;
      }
    }
	
    if(tipo_campo != 1 && tipo_campo != 2 && tipo_campo != 3 && tipo_campo != 4 && tipo_campo != 5){
      printf("ERRO: Os tipos precisam ser: 1-int, 2-float, 3-double, 4-char ou 5-string.\n");
    } else if(!campoOK){ 
      printf("Campo já foi criado.\n"); // mensagem de erro caso campo inserido já exista
    }else{      
      strcpy(tabela_user.tab_L[j],nome_campo);
      tabela_user.tipos[j]=tipo_campo;
      j++;
    }

  }while(j<tabela_user.C);

  // Criando campo de id
  int j_id;
  do{
    idOK=0;
    do{
      campoOK=0;
      printf("Qual dos campos é o ID da tabela? \n");
      scanf("%s",nome_campo);
  
      // verificar se campo ID passado existe
      for(int j=0; j<tabela_user.C; j++){
	if(strcmp(tabela_user.tab_L[j],nome_campo)==0){
	  campoOK=1;
	  j_id=j;
	  break;
	}
      }

      if(!campoOK){
	printf("Campo inexistente. \nOs campos passados são: \n");
	for(int j=0; j<tabela_user.C; j++)
	  printf("%s ",tabela_user.tab_L[j]);
	printf("\n");
      }
    }while(!campoOK);
  
    // verifique se o id da tabela é valido
    if(tabela_user.tipos[j_id] == int_){
      idOK=1;
    } else{ 
      printf("ID invalido. O ID da tabela precisa conter inteiros positivos (tipo int).\n");
    }
  
  }while(!idOK);

  // guarde na tabela a linha de tipo
  fprintf(tabela_file,"%d;",tabela_user.tipos[j_id]); // tipo do id
  for(int j=0; j<tabela_user.C; j++){
    if(j!=j_id)
      fprintf(tabela_file,"%d;",tabela_user.tipos[j]);
  }
  fprintf(tabela_file,"\n");	

  // guarde na tabela a linha de campos
  fprintf(tabela_file,"%s;",tabela_user.tab_L[j_id]); // campo do id	
  for(int j=0; j<tabela_user.C; j++){
    if(j!=j_id)
      fprintf(tabela_file,"%s;",tabela_user.tab_L[j]);	
  }
  fprintf(tabela_file,"\n");	

  if(tabela_file == NULL){
    printf("Tabela não abriu!\n");
  }
  else{
    fclose(tabela_file);
  }

  // Registre a nova tabela no arquivo de relação de tabelas 
  relacao_file = fopen("relacaoTab", "a");
  fprintf(relacao_file,"%s %d\n",tabela_user.nome, tabela_user.C);	
 
  if(tabela_file == NULL){
    printf("Relação de tabelas não abriu!\n");
  }
  else{
    fclose(relacao_file);
  }

  // fazer o free de tabela_user.tab
  for(int j=0; j<tabela_user.C; j++)  
    free(tabela_user.tab_L[j]);

  free(tabela_user.tab_L);
  free(tabela_user.tipos);

}

void insereLinha_tab(char nome_tab[50], int n){
  FILE *tab_file;
  char str_temp[50], id_file[50];
  int i,j,temp;
  int campoOK; //bool
  double x_temp;
  
  tabela tab;
  strcpy(tab.nome, nome_tab); 
  tab.C=n;

  // alocando vetores da struct
  tab.tab_L=malloc(sizeof(char*)*tab.C); // colunas alocadas
  for(j=0; j<tab.C; j++)
    tab.tab_L[j]=malloc(sizeof(char)*50); // strings alocadas
  
  tab.tipos=malloc(sizeof(enum tipos_tab)*tab.C); // vetor de enum alocado


  tab_file = fopen(nome_tab, "r");    
  if(tab_file == NULL)
    printf("Tabela não abriu!\n");

  // Leia a linha de tipos
  for(i=0; i<tab.C; i++){
    fscanf(tab_file," %[^;]s", str_temp);
    sscanf(str_temp,"%d", &temp);
    fgetc(tab_file);
    tab.tipos[i]=temp;
    //printf("%s %d\n", str_temp, tab.tipos[i]);
  }

  // Leia a linha dos campos
  for(i=0; i<tab.C; i++){
    fscanf(tab_file," %[^;]s", str_temp);
    fgetc(tab_file);
    strcpy(tab.tab_L[i], str_temp);
    //printf("temp: %s  tab.tab_L: %s\n", str_temp, tab.tab_L[i]);
  }
  fclose(tab_file);

  i=0;
  do{
    campoOK=1;
 
    conversao_tipos(tab.tipos[i], str_temp);
    printf("Insira o campo %s (tipo: %s )\n", tab.tab_L[i],str_temp);
    scanf(" %[^\n]s", str_temp);
    
    if(i==0){
      // cheque se chave passada é um unsigned int
      for(j=0; j<strlen(str_temp); j++){
	if(isdigit(str_temp[j]) == 0){ 
	  campoOK=0;
	  printf(">>> ERRO: campo ID precisa ser do tipo unsigned int.\n");
	  break;
	}		    	
      }
		
      if(campoOK){
	sscanf(str_temp,"%lf",&x_temp);
	if(0>x_temp || x_temp>UINT_MAX){
	  campoOK=0;
	  printf(">>> ERRO: campo passado não cabe em um unsigned int.\n");
	} else{
	  // checagem se id é único
	  tab_file = fopen(nome_tab, "r");
	  if(tab_file == NULL)
	    printf("Tabela não abriu!\n");
	  
	  j=0; // pule linha de tipos e campos do arquivo
	  while(fscanf(tab_file, " %[^;]s", id_file)!=EOF){
	    fscanf(tab_file, " %*[^\n]s");
	    if(j>1 && strcmp(str_temp,id_file)==0){ 
	      printf(">>> ERRO: ID %s já existe!\n",str_temp);
	      campoOK=0;
	      break;
	    }
	    j++;
	  }

	  fclose(tab_file);
	}	
      }
    } else{
      campoOK=checaLimite_campos(str_temp,tab.tipos[i]);
    }
    
    if(campoOK){
      i++;
    
      tab_file = fopen(nome_tab, "a");
      if(tab_file == NULL)
	printf("Tabela não abriu!\n");
      
      fprintf(tab_file, "%s;", str_temp);
      fclose(tab_file);
    }
    
  } while(i<tab.C);

  tab_file = fopen(nome_tab, "a");
  if(tab_file == NULL)
    printf("Tabela não abriu!\n");
  
  fprintf(tab_file,"\n");
  fclose(tab_file);
  
}

void editar_tab(char nome_tab[50], int n){

  FILE *tab_file;
  int temp, temp2;
  char str_temp[50], id_file[50];
  int i;
  int campo_OK; //bool
  char aux2[10];

  tabela tab; //tipo struct tabela_t

  strcpy(tab.nome, nome_tab); 
  tab.C=n;

  //alocando vetores da struct
  tab.tab_L=malloc(sizeof(char*)*tab.C); //colunas alocadas
  for(int j=0; j<tab.C; j++)
    tab.tab_L[j]=malloc(sizeof(char)*50); // strings alocadas
  
  tab.tipos=malloc(sizeof(enum tipos_tab)*tab.C); //vetor de enum alocado

  tab_file = fopen(nome_tab, "r+");

  //Leia a linha de tipos
  for(i=0; i<tab.C; i++){
    fscanf(tab_file," %[^;]s", str_temp);
    sscanf(str_temp,"%d", &temp);
    fgetc(tab_file);
    tab.tipos[i]=temp;
  }
      
  //Leia a linha dos campos
  for(i=0; i<tab.C; i++){
    fscanf(tab_file," %[^;]s", str_temp);
    fgetc(tab_file);
    strcpy(tab.tab_L[i], str_temp);
  }
       
  fclose(tab_file);

  tab_file = fopen(nome_tab, "a");
  if(tab_file == NULL)
    printf("Relação de tabelas não abriu!\n");
    
  i=0; 
  tab_file = fopen(nome_tab, "r");
  int cont;
  int existe_id; //bool
  char chave[10];
  char chave_user[10];
  int linha_id;

  //Pede a chave primária(ID), caso não exista repete o processo
  do{ 
    tab_file = fopen(nome_tab, "r");
    cont=0;
    printf("Insira o ID da linha: ");
    scanf("%s", chave_user);
    while(fscanf(tab_file," %[^\n]s", str_temp)!=EOF){ //AQUI 
      cont++;
      existe_id=0;
      sscanf(str_temp," %[^;]s", aux2);
    
      if(cont>=2 && strcmp(aux2, chave_user)==0){
	existe_id=1;
        linha_id=cont-1;
        printf("Linha a editar: %d\n", linha_id);
	strcpy(chave,aux2);

	break;
      }
    }
    fclose(tab_file);
  }while(existe_id==0);
  if(existe_id)
    printf("Achei, o ID é: %s\n", chave);
  else
    printf("ID não existe!\n");
    
    
  ///////////////////////////////////////////
  fpos_t posicao;

  //teste de edição de arquivo
  printf("\n\n*** Meu teste ***\n\n");
  tab_file = fopen(nome_tab, "r+");
  if(tab_file == NULL)
    printf("Tabela não abriu!\n");
    
  i=0;
  while(fscanf(tab_file, " %[^;]s", id_file)!=EOF){
    if(strcmp(chave_user,id_file)==0){
      printf("Encontrei o ID %s!\n",chave_user);
      fgetc(tab_file);
      fgetpos(tab_file,&posicao);
       
      fsetpos(tab_file,&posicao);
      fprintf(tab_file,"%s","ESTRAGO");
      break;
    }
    printf("Li linha %d do aquivo %s.\n",i,nome_tab);
    fscanf(tab_file, " %*[^\n]s");
    i++;
  }

  fclose(tab_file);


}


int main(void) {
  FILE *tabela_file, *relacao_file;
  char op;
  char input[50], str_aux[50];
  int continua, existe; //bool
  int temp, temp2;
  char aux[10];

  printf("Bem vindo ao SGBD KI! - vs beta - \n");

  continua=1;

  do{
    printf("\n ***** MENU ***** \n");
    printf("1 - Criar tabela\n");
    printf("2 - Listar tabelas existentes\n");
    printf("3 - Inserir linha em tabela\n");
    printf("4 - Editar colunas da tabela\n");
    printf("s - sair\n\n");

    scanf(" %c",&op);

    switch (op){
    case '1':
      cria_tab();
      break;
    case '2':
      listagem_tab();
      break;
    case '3':
      printf("Qual a tabela que deseja acessar?\n");
      scanf(" %s",input);
      relacao_file = fopen("relacaoTab", "r");
      
      existe=0;
      while(fscanf(relacao_file,"%s %d\n",str_aux, &temp)!=EOF){ //enquanto não for End of File, continue imprimindo
	if(strcmp(str_aux, input)==0){
	  existe=1;
	  break;
        }
      }
      fclose(relacao_file);

      if(existe)
        insereLinha_tab(input, temp);
      else
	printf("Tabela não existe!\n");

      break;
    case '4':
      printf("Qual a tabela que deseja acessar?\n");
      scanf(" %s",input);
      
      relacao_file = fopen("relacaoTab", "r");
      existe=0;
      while(fscanf(relacao_file,"%s %d\n",str_aux, &temp2)!=EOF){ //enquanto não for End of File, continue 
	if(strcmp(str_aux, input)==0){
	  existe=1;
	  break;
        }
      }

      fclose(relacao_file);
      if(existe)
      	editar_tab(input, temp2);
      else
	printf("Tabela não existe!\n");
      
      break;
    case 's':
      printf("Tchau!\n");
      continua=0;
      break;
    default:
      printf("Opção inválida. \n");
      break;
    }

  } while(continua);

  return 0;
}



