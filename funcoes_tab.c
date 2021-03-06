#define _GNU_SOURCE char *strcasestr(const char *haystack, const char *needle);
#include "lista_func.h"

tabela *alocaStruct_tab(int col){ 
  int i;
  tabela *tab;

  tab=malloc(sizeof(tabela));
  
  //alocando vetores da struct
  tab->tab_L=malloc(sizeof(char*)*col); //colunas alocadas
  for(i=0; i<col; i++)
    tab->tab_L[i]=malloc(sizeof(char)*50); // strings alocadas
  
  tab->tipos=malloc(sizeof(enum tipos_tab)*col); //vetor de enum alocado

  return tab;
}

void fillStruct_tab(tabela *tab, char nome_tab[50], int col){ 
  FILE *tab_file;
  int i,temp;
  char str_temp[50];
  
  strcpy(tab->nome, nome_tab); 
  tab->C=col;

  tab_file = fopen(nome_tab, "r");    
  if(tab_file == NULL)
    printf("\nALERTA: Tabela na linha %d não abriu!\n",__LINE__);

  // Leia a linha de tipos
  for(i=0; i<tab->C; i++){
    fscanf(tab_file," %[^;]s", str_temp);
    sscanf(str_temp,"%d", &temp);
    fgetc(tab_file);
    tab->tipos[i]=temp;
  }

  // Leia a linha dos campos
  for(i=0; i<tab->C; i++){
    fscanf(tab_file," %[^;]s", str_temp);
    fgetc(tab_file);
    strcpy(tab->tab_L[i], str_temp);
  }
  
  fclose(tab_file);

}

void freeStruct_tab(tabela *tab){
  int j;
  
  for(j=0; j<tab->C; j++)  
    free(tab->tab_L[j]);

  free(tab->tab_L);
  free(tab->tipos);
  free(tab);
}

void conversao_tipos(enum tipos_tab TIPO, char str_tipo[10]){
  char tipos_all[][10]={"int", "float", "double", "char", "string"};
  strcpy(str_tipo, tipos_all[TIPO-1]);
}

// checagem se valor de campo é compatível com o tipo do campo
int checa_campos(char campo[50], enum tipos_tab tipo_campo){
  int campoOK=1; // bool
  int n_pontos=0;
  unsigned int i;
  double x_temp;
  char  tipo_temp[10];

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
    if(strlen(campo)>1 || campo[0]==';' )
      campoOK=0;

  }

  if(!campoOK){
    conversao_tipos(tipo_campo,tipo_temp);
    printf("\n>>> ERRO: Campo passado precisa ser do tipo %s.\n\n",tipo_temp);
  }
  
  return campoOK;
}


// Função que imprime uma linha de uma tabela a partir de um id
void listaLinha_tab(char nome_tab[50], int n, char id_print[50]){ 
  FILE *tab_file;
  char str_temp[50], chave[50];
  int line,col;
  unsigned int maior;
  
  tabela *tab;
 
  tab=alocaStruct_tab(n);
  fillStruct_tab(tab, nome_tab, n);	

  tab_file=fopen(nome_tab,"r");
  if(tab_file==NULL)
    printf("\nALERTA: tabela não abriu na linha %d.\n\n",__LINE__);

  maior=0;
  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
    if(strlen(str_temp)>maior)
      maior=strlen(str_temp);

    fgetc(tab_file); // pule delimitador ; 
  }

  rewind(tab_file);

  //procurar linha do id passado
  line=0;
  while(fscanf(tab_file," %[^;]s", chave)!=EOF){
    if(line>=2 && strcmp(chave, id_print)==0){
      break;
    }
    fscanf(tab_file, " %*[^\n]s");
    line++;
  }
  
  //imprima linha do id encontrado
  printf("%-*s ",maior+5,chave);
  fgetc(tab_file); // pule delimitador ; 
  col=1; 
  while(col<tab->C){
    fscanf(tab_file," %[^;]s",str_temp);

    if(strcmp(str_temp,"NULL")==0){
      printf("%-*s ",maior+5," ");
    } else{
      printf("%-*s ",maior+5,str_temp);
    }

    fgetc(tab_file); // pule delimitador ; 
    col++;

  }
  
  printf("\n");

  fclose(tab_file);

  freeStruct_tab(tab);
  
}


//Função para listar todas tabelas
void listagem_tab(){ 
  FILE *relacao_file;
  char nome[50];
  
  
  printf("\n *** Tabelas existentes ***\n");

  relacao_file = fopen("relacaoTab", "r");

  while(fscanf(relacao_file,"%s %*d\n",nome)!=EOF){ // enquanto não for End of File, continue imprimindo
    printf("%s\n",nome);
  }

  if(relacao_file == NULL){
    printf("Relação de tabelas não existe!\n");
  }
  else{
    fclose(relacao_file);
  }

}

// Função para criar uma nova tabela
void cria_tab(){ //>>>>>>>>>>>já usa funcao alocação<<<<<<<<<<<<<
  FILE *tabela_file, *relacao_file;
  tabela *tabela_user;
  unsigned int i,j;
  char nome_campo[50], nome_tab[50], str_temp[50];
  int tipo_campo, quant_campos; 
  int campoOK, idOK, id_int=1; // bool

  
  do{
    campoOK=1;
    printf("Digite o nome da tabela: "); 
    scanf(" %s", nome_tab);  
    strcat(nome_tab,".tab"); 

    relacao_file=fopen("relacaoTab","r");
    if(relacao_file==NULL){
      printf("\nPrimeira tabela criada no sistema.\n\n");
      break;
    }

    while(fscanf(relacao_file," %s %*d",str_temp)!=EOF){
      if(strcmp(nome_tab,str_temp)==0){
	campoOK=0;
	printf("\n>>> ERRO: tabela já existe.\n\n");
	break;
      }
    }
    
    fclose(relacao_file);
  } while(!campoOK);
 
  printf("Quantos campos irá inserir?\n");
  scanf("%d", &quant_campos);  
 
  //>>>>>alocando vetores da struct
  tabela_user=alocaStruct_tab(quant_campos); 
  strcpy(tabela_user->nome, nome_tab);
  tabela_user->C=quant_campos;

  j=0;
  do{
    //tipo_campo=0;
    printf("\nInsira o campo[%d](sem espaços) e o seu tipo\n\n 1-int\n 2-float\n 3-double\n 4-char\n 5-string\n\n", j+1);
    scanf(" %s %d", nome_campo, &tipo_campo); 

    // verifique que os campos inseridos são únicos		
    campoOK=1;
    for(i=0; i<j; i++){
      if(strcmp(tabela_user->tab_L[i],nome_campo)==0){
	campoOK=0;
	break;
      }
    }

    // Proiba a utilização de caracteres reservados pelo sistema
    for(i=0;i<strlen(nome_campo);i++){
      if(nome_campo[i]=='*' || nome_campo[i]==';'){
	printf("\n>>>ERRO: '*' e ';' são caracteres reservados pelo sistema.\n\n");
	campoOK=0;
	break;
      }  
    }

    // Se for o último campo, VERIFICAR SE TEM UM CAMPO DISPONIVEL PARA SER UM ID
    if(j==(unsigned int)(tabela_user->C-1)){ 

      id_int=0;
      for(i=0;i<j;i++){
	if(tabela_user->tipos[i]==1){ 
	  id_int=1;
	  break;
	}
      }
      
      if(tipo_campo==1)
	id_int=1;
    }

    if(tipo_campo != 1 && tipo_campo != 2 && tipo_campo != 3 && tipo_campo != 4 && tipo_campo != 5){
      printf("\n>>> ERRO: Os tipos precisam ser: 1-int, 2-float, 3-double, 4-char ou 5-string.\n\n");
    } else if(!campoOK){ 
      printf("\n>>> ERRO: Campo já foi criado ou inválido.\n\n"); 
    } else if(!id_int){ 
      printf("\n>>> ERRO: Insira um campo obrigatoriamente do tipo int para ser o ID.\n\n");
    } else{      
      strcpy(tabela_user->tab_L[j],nome_campo);
      tabela_user->tipos[j]=tipo_campo;
      j++;
    }

  } while(j<(unsigned int)(tabela_user->C));

  // Criando campo de id
  int j_id;
  do{
    idOK=0;
    do{
      campoOK=0;
      printf("Qual dos campos é o ID da tabela? \n");
      scanf(" %s",nome_campo);
  
      // verificar se campo ID passado existe
      for(int j=0; j<tabela_user->C; j++){
	if(strcmp(tabela_user->tab_L[j],nome_campo)==0){
	  campoOK=1;
	  j_id=j;
	  break;
	}
      }

      if(!campoOK){
	printf("\n>>> ERRO: Campo inexistente. \nOs campos passados são: \n\n");
	for(int j=0; j<tabela_user->C; j++)
	  printf("%s ",tabela_user->tab_L[j]);
	printf("\n");
      }
    }while(!campoOK);
  
    // verifique se o id da tabela é valido
    if(tabela_user->tipos[j_id] == int_){
      idOK=1;
    } else{ 
      printf("\n>>> ERRO: ID invalido. O ID da tabela precisa conter inteiros positivos (tipo int).\n\n");
    }
  
  }while(!idOK);

  tabela_file = fopen(nome_tab,"wr+");
    
  // guarde na tabela a linha de tipo
  fprintf(tabela_file,"%d;",tabela_user->tipos[j_id]); // tipo do id
  for(int j=0; j<tabela_user->C; j++){
    if(j!=j_id)
      fprintf(tabela_file,"%d;",tabela_user->tipos[j]);
  }
  fprintf(tabela_file,"\n");	

  // guarde na tabela a linha de campos
  fprintf(tabela_file,"%s;",tabela_user->tab_L[j_id]); // campo do id	
  for(int j=0; j<tabela_user->C; j++){
    if(j!=j_id)
      fprintf(tabela_file,"%s;",tabela_user->tab_L[j]);	
  }
  fprintf(tabela_file,"\n");	

  if(tabela_file == NULL){
    printf("\nALERTA: Tabela na linha %d não abriu!\n\n",__LINE__);
  }
  else{
    fclose(tabela_file);
  }

  // Registre a nova tabela no arquivo de relação de tabelas 
  relacao_file = fopen("relacaoTab", "a");
  fprintf(relacao_file,"%s %d\n",tabela_user->nome, tabela_user->C);	
 
  if(tabela_file == NULL){
    printf("\nALERTA: Relação de tabelas na linha %d não abriu!\n",__LINE__);
  }
  else{
    fclose(relacao_file);
  }

  // fazer o free de tabela
  freeStruct_tab(tabela_user);

}

void insereLinha_tab(char nome_tab[50], int n){//>>>>>>>>>>>já usa funcao alocação<<<<<<<<<<<<<
  FILE *tab_file;
  char str_temp[50], id_file[50];
  int i;
  unsigned int j;
  int campoOK; //bool
  double x_temp;
  
  tabela *tab;
 
  tab=alocaStruct_tab(n);
  fillStruct_tab(tab, nome_tab, n);

  
  tab_file = fopen(nome_tab, "r");    
  if(tab_file == NULL)
    printf("\nALERTA: Arquivo na linha %d não abriu!\n\n",__LINE__);

  i=0;
  do{
    campoOK=1;
 
    conversao_tipos(tab->tipos[i], str_temp);
    printf("Insira o campo %s (tipo: %s )\n", tab->tab_L[i],str_temp);
    scanf(" %[^\n]s", str_temp);
    
    if(i==0){
      // cheque se chave passada é um unsigned int
      for(j=0; j<strlen(str_temp); j++){
	if(isdigit(str_temp[j]) == 0){ 
	  campoOK=0;
	  printf("\n>>> ERRO: campo ID precisa ser do tipo unsigned int.\n\n");
	  break;
	}		    	
      }
		
      if(campoOK){
	sscanf(str_temp,"%lf",&x_temp);
	if(0>x_temp || x_temp>UINT_MAX){
	  campoOK=0;
	  printf("\n>>> ERRO: campo passado não cabe em um unsigned int.\n\n");
	} else{
	  // checagem se id é único
	
	  rewind(tab_file);
	  
	  j=0; // pule linha de tipos e campos do arquivo
	  while(fscanf(tab_file, " %[^;]s", id_file)!=EOF){
	    fscanf(tab_file, " %*[^\n]s");
	    if(j>1 && strcmp(str_temp,id_file)==0){ 
	      printf("\n>>> ERRO: ID %s já existe!\n\n",str_temp);
	      campoOK=0;
	      break;
	    }
	    j++;
	  }

	  fclose(tab_file);
	}	
      }
    } else{
      
      campoOK=checa_campos(str_temp,tab->tipos[i]);
      
      if(tab->tipos[i] == string_ ){

	// Proiba a utilização de caracteres reservados pelo sistema
	if(strcmp(str_temp, "NULL")==0){
	  campoOK=0;
	  printf("\n>>>ERRO: string reservada pelo sistema.\n\n");
	}

	for(j=0;j<strlen(str_temp);j++){
	  if(str_temp[j]=='*' || str_temp[j]==';'){
	    printf("\n>>>ERRO: '*' e ';' são caracteres reservados pelo sistema.\n\n");
	    campoOK=0;
	    break;
	  }  
	}

	
      }
    }
    
    if(campoOK){
      i++;
    
      tab_file = fopen(nome_tab, "a");
      if(tab_file == NULL)
	printf("\nALERTA: Tabela na linha %d não abriu!\n",__LINE__);
      
      fprintf(tab_file, "%s;", str_temp);
      fclose(tab_file);
    }
    
  } while(i<tab->C);

  tab_file = fopen(nome_tab, "a");
  if(tab_file == NULL)
    printf("\nALERTA: Tabela na linha %d não abriu!\n",__LINE__);
  
  fprintf(tab_file,"\n");
  fclose(tab_file);

  // fazer o free de tabela
  freeStruct_tab(tab);
  
}


void editar_tab(char nome_tab[50], int n){//>>>>>>>>>>>já usa funcao alocação<<<<<<<<<<<<<
  FILE *tab_file,*file_temp;
  char str_temp[50];
  int cont;
  unsigned int i,j;
  int campoOK,existe_id; //bool
  char chave[10],chave_user[10],c_temp;
  long int pos;

  tabela *tab; //tipo struct tabela_t

  tab=alocaStruct_tab(n);  
  fillStruct_tab(tab, nome_tab, n);

  // Pede a chave primária(ID), caso não exista repete o processo
  tab_file = fopen(nome_tab, "r+");
  do{ 
    printf("Insira o ID da linha: ");
    scanf(" %s", chave_user);

    cont=0;
    existe_id=0;
    while(fscanf(tab_file," %[^;]s", chave)!=EOF){
      if(cont>=2 && strcmp(chave, chave_user)==0){
	existe_id=1;
	pos=ftell(tab_file);
	break;
      }
      fscanf(tab_file, " %*[^\n]s");
      cont++;
    }

    if(existe_id)
      printf("Achei o ID %s.\n", chave);
    else
      printf("\n>>> ERRO: ID não existe.\n\n");

    rewind(tab_file);
  } while(!existe_id);
    
  // copia arquivo até id da linha que será editada
  file_temp=fopen("fileTemp","w+");
  if(file_temp==NULL)
    printf("\nALERTA: Arquivo na linha %d não abriu!\n\n",__LINE__);
    
  cont=0;
  while(cont<=pos){
    c_temp=fgetc(tab_file);
    fprintf(file_temp,"%c",c_temp);
    cont++;
  }

  // armazena linha conforme campos passados pelo usuário
  i=1;
  do{
    printf("Entre com o campo %s (tipo: %d):",tab->tab_L[i],tab->tipos[i]);
    scanf(" %[^\n]s",str_temp);
    
    campoOK=checa_campos(str_temp,tab->tipos[i]);

    // Proiba a utilização de caracteres reservados pelo sistema
    if(strcmp(str_temp, "NULL")==0){
      campoOK=0;
      printf("\n>>>ERRO: string reservada pelo sistema.\n\n");
    }
	
    for(j=0;j<strlen(str_temp);j++){
      if(str_temp[j]=='*' || str_temp[j]==';'){
	printf("\n>>>ERRO: '*' e ';' são caracteres reservados pelo sistema.\n\n");
	campoOK=0;
	break;
      }  
    }
    
    if(campoOK){
      fprintf(file_temp,"%s;",str_temp);
      i++;
    }
  } while(i<(unsigned int) tab->C);
  fprintf(file_temp,"\n");
    
  // copia restante 
  fscanf(tab_file, " %*[^\n]s");
  fgetc(tab_file);
  while((c_temp=fgetc(tab_file))!=EOF){ 
    fprintf(file_temp,"%c",c_temp);
  }
    
  fclose(file_temp);
  fclose(tab_file);

  remove(tab->nome);
  rename("fileTemp",tab->nome);

  freeStruct_tab(tab);

}

void insereColuna_tab(char nome_tab[50], int n){

  FILE *tab_file, *file_temp, *relacao_file;
  char nome_campo[50];
  int tipo_campo;
  unsigned int i;
  int campoOK; //bool
 
  
  tabela *tab;
 
  tab=alocaStruct_tab(n+1);  	
  fillStruct_tab(tab, nome_tab, n);	

  do{
    tipo_campo=0;
    printf("\nInsira o campo[%d](sem espaços) e o seu tipo\n\n 1-int\n 2-float\n 3-double\n 4-char\n 5-string\n\n", tab->C+1);
    scanf(" %s %d", nome_campo, &tipo_campo); 

    campoOK=1;
    //verificar se o campo já existe
    for(i=0; i<(unsigned int) tab->C; i++){
      if(strcmp(tab->tab_L[i],nome_campo)==0){
	campoOK=0;
	break;
      }
    }
    
    //verificar se o tipo é válido
    if(tipo_campo != 1 && tipo_campo != 2 && tipo_campo != 3 && tipo_campo != 4 && tipo_campo != 5){
      printf("\n>>> ERRO: Os tipos precisam ser: 1-int, 2-float, 3-double, 4-char ou 5-string.\n\n");
    } else if(!campoOK){ 
      printf("\n>>> ERRO: Campo já foi criado.\n\n"); // mensagem de erro caso campo inserido já exista
    }else{      
      strcpy(tab->tab_L[tab->C],nome_campo);
      tab->tipos[tab->C]=tipo_campo;
    } 

  }while(!campoOK);

  
  tab_file = fopen(nome_tab, "r"); 

  file_temp=fopen("fileTemp","w+");
  
  if(file_temp==NULL){
    printf("\nALERTA: Arquivo na linha %d não abriu!",__LINE__);
  }


  char aux2[50];
  int cont;
  int num;
  
  
  cont=0;
  i=0;
  while(fscanf(tab_file," %[^;]s", aux2)!=EOF){ 
    fprintf(file_temp, "%s;", aux2);
    fgetc(tab_file);

  		
    if(cont==tab->C-1 && i==0){
      cont=-1;
      i++;
      fprintf(file_temp, "%d;", tipo_campo);
      fprintf(file_temp, "\n");
    }

    if(cont==tab->C-1 && i==1){
      cont=-1;
      i++;
      fprintf(file_temp, "%s;", nome_campo);
      fprintf(file_temp, "\n");
  			
    }
    if(cont==tab->C-1 && i>1){
      cont=-1;
      i++; // conta o numero de linhas
      fprintf(file_temp, "%s;", "NULL");
      fprintf(file_temp, "\n");
    }

    cont++; // conta o número de campos
  	
  }

  fclose(file_temp);
  remove(tab->nome);
  rename("fileTemp",tab->nome);
  fclose(tab_file); //AQUI<<<<<< 25 allocs and 24 free
		

  relacao_file = fopen("relacaoTab", "r");
    
  if(relacao_file == NULL){
    printf("\nALERTA: Relação de tabelas na linha %d não abriu!\n",__LINE__);
  }
  		

  file_temp=fopen("fileTemp","w+");

  if(file_temp==NULL)
    printf("\nALERTA: Arquivo na linha %d não abriu!\n\n",__LINE__);
	
  //Copiando o arquivo relacaoTab para um temporário até a tabela a ser substituida
  while(fscanf(relacao_file," %s %d", aux2, &num)!=EOF){
    if(strcmp(aux2, nome_tab)==0){
      fprintf(file_temp, "%s %d", aux2, num+1);
      fprintf(file_temp, "\n");
    }else{
      fprintf(file_temp, "%s %d", aux2, num);
      fprintf(file_temp, "\n");
    }

  }		  
  		
  fclose(relacao_file);
  remove("relacaoTab");
  rename("fileTemp","relacaoTab");
  fclose(file_temp);
  
  // fazer o free de tabela
  freeStruct_tab(tab);

}

void listarDados_tab(char nome_tab[50], int n){

  FILE *tab_file;
  char str_temp[50];
  int line,col;
  unsigned int maior;
  
  tabela *tab;
 
  tab=alocaStruct_tab(n);
  fillStruct_tab(tab, nome_tab, n);	

  tab_file=fopen(nome_tab,"r");
  if(tab_file==NULL)
    printf("\nALERTA: tabela não abriu na linha %d.\n\n",__LINE__);

  maior=0;
  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
    if(strlen(str_temp)>maior)
      maior=strlen(str_temp);

    fgetc(tab_file); // pule delimitador ; 
  }

  rewind(tab_file);
  printf("\n***** TABELA %s *****\n",nome_tab);

  line=0; col=0; 
  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){

    if(line>0){
      if(strcmp(str_temp,"NULL")==0){
	printf("%-*s ",maior+5," ");
      }else{
	printf("%-*s ",maior+5,str_temp);
      }

    }

    fgetc(tab_file); // pule delimitador ; 
    col++;

    if(col==tab->C){
      printf("\n");
      line++;
      col=0;
    }

  }
  printf("\n***** FIM *****\n\n");

  fclose(tab_file);

  freeStruct_tab(tab);
}

int pesquisarDados_tab(char nome_tab[50], int n){
  
  FILE *tab_file;
  char str_temp[50], str_tipo[10], str_find[50], *str,*str2, id_print[50];
  int campoOK; //bool
  int op, col, usr_col, line, cont;
  unsigned int i, j, maior;
  int usr_int,tab_int;
  float usr_float,tab_float;
  double usr_double,tab_double;
  char usr_char,tab_char;
  char usr_str[50], usr_campo[50],usr_str2[50];
  enum tipos_tab tipo_campo; 
  
  tabela *tab;
 
  tab=alocaStruct_tab(n);  	
  fillStruct_tab(tab, nome_tab, n);

  tab_file=fopen(nome_tab,"r");
  if(tab_file==NULL) 
    printf("\nALERTA: tabela não abriu na linha %d.\n\n",__LINE__); 

  maior=0;
  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
    if(strlen(str_temp)>maior)
      maior=strlen(str_temp);

    fgetc(tab_file); // pule delimitador ; 
  }

  rewind(tab_file);
  printf("\n***** COLUNAS DISPONÍVEIS *****\n");

  line=0; col=0; 
  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){

    if(line==1)
      printf("%-*s ",maior+2,str_temp);

    fgetc(tab_file); // pule delimitador ; 
    col++;

    if(col==tab->C){
      line++;
      col=0;
    }

  }
  printf("\n\nDigite a coluna para realizar a pesquisa: \n");
  scanf(" %[^\n]s", usr_campo);
  
  rewind(tab_file);
  campoOK=0;
  line=0; col=0;
  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
    if(line==1){
      if(strcmp(usr_campo, str_temp)==0){
	campoOK=1;
	usr_col=col;
	tipo_campo=tab->tipos[usr_col];

	conversao_tipos(tipo_campo,str_tipo);
	printf("O campo escolhido foi: %s, tipo %s\n", usr_campo, str_tipo);
	break;
      }
    }
    fgetc(tab_file); // pule delimitador ;
    
    if(col==tab->C-1){
      line++;
      col=-1;
    }
    col++;
  }

  rewind(tab_file);
  
  if(!campoOK){
    printf("\n>>> ERRO: campo inválido.\n");
    freeStruct_tab(tab);
    return 0;
  }

  do{ 
    
    tab_file=fopen(nome_tab,"r");
    printf("\n *** MENU ***\n");
    printf("\n 1 - Valores maior que o valor informado");
    printf("\n 2 - Valores maior ou igual que o valor informado");
    printf("\n 3 - Valores igual ao valor informado");
    printf("\n 4 - Valores menor que o valor informado");
    printf("\n 5 - Valores menor ou igual que o valor informado");
    printf("\n 6 - Busca parcial de palavras (use * )");
    printf("\n 7 - Retornar ao menu anterior");

    printf("\n\nSelecione a operação desejada: \n");
    scanf(" %d", &op);

    do{
      if(op!=7){ 
	printf("\n\nInforme um valor para comparação: \n");
	scanf(" %[^\n]s", str_find); 
      }

      //Confere se o número passado pelo usuário corresponde ao tipo da coluna:
    } while(!checa_campos(str_find, tipo_campo));

    switch (op){
    case 1: //lista todos os números MAIORES que o passado pelo usuário

      if(tipo_campo==int_){
	sscanf(str_find,"%d",&usr_int);
	printf("Números maiores que %d na coluna %s:\n", usr_int, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%d",&tab_int);
	    if(usr_int<tab_int){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==float_){
	sscanf(str_find,"%f",&usr_float);
	printf("Números maiores que %f na coluna %s:\n", usr_float, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){

	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }

	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%f",&tab_float);
	    if(usr_float<tab_float){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==double_){
	sscanf(str_find,"%lf",&usr_double);
	printf("Números maiores que %lf na coluna %s:\n", usr_double, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){

	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%lf",&tab_double);
	    if(usr_double<tab_double){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==char_){
	sscanf(str_find,"%c",&usr_char);
	printf("Caracteres lexicograficamente maiores que '%c' na coluna %s:\n", usr_char, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%c",&tab_char);
	    if(usr_char<tab_char){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==string_){
	strcpy(usr_str,str_find);
	printf("Palavras lexicograficamente maiores que '%s' na coluna %s:\n", usr_str, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col && strcmp(str_temp,"NULL")){ 
	    if(strcmp(usr_str,str_temp)<0){ //se str_temp > usr_str
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}

      }
    
      break;
    case 2: //lista todos os números MAIORES OU IGUAIS ao número passado pelo usuário
      if(tipo_campo==int_){
	sscanf(str_find,"%d",&usr_int);
	printf("Números maiores ou iguais a %d na coluna %s:\n", usr_int, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%d",&tab_int);
	    if(usr_int<=tab_int){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
				    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==float_){
	sscanf(str_find,"%f",&usr_float);
	printf("Números maiores ou iguais a %f na coluna %s:\n", usr_float, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%f",&tab_float);
	    if(usr_float<=tab_float){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==double_){
	sscanf(str_find,"%lf",&usr_double);
	printf("Números maiores ou iguais a %lf na coluna %s:\n", usr_double, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%lf",&tab_double);
	    if(usr_double<=tab_double){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==char_){
	sscanf(str_find,"%c",&usr_char);
	printf("Caracteres lexicograficamente maiores ou iguais a '%c' na coluna %s:\n", usr_char, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%c",&tab_char);
	    if(usr_char<=tab_char){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==string_){
	strcpy(usr_str,str_find);
	printf("Palavras lexicograficamente maiores ou iguais a '%s' na coluna %s:\n", usr_str, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col && strcmp(str_temp,"NULL")){ 
	    if(strcmp(usr_str,str_temp)<=0){ //se str_temp >= usr_str
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      }

      break;
    case 3: //lista todos os números IGUAIS ao passado pelo usuário
      if(tipo_campo==int_){
	sscanf(str_find,"%d",&usr_int);
	printf("Números iguais a %d na coluna %s:\n", usr_int, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%d",&tab_int);
	    if(usr_int==tab_int){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
				    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==float_){
	sscanf(str_find,"%f",&usr_float);
	printf("Números iguais a %f na coluna %s:\n", usr_float, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%f",&tab_float);
	    if(usr_float==tab_float){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==double_){
	sscanf(str_find,"%lf",&usr_double);
	printf("Números iguais a %lf na coluna %s:\n", usr_double, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }

	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%lf",&tab_double);
	    if(usr_double==tab_double){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==char_){
	sscanf(str_find,"%c",&usr_char);
	printf("Caracteres lexicograficamente iguais a '%c' na coluna %s:\n", usr_char, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%c",&tab_char);
	    if(usr_char==tab_char){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}

      } else if(tipo_campo==string_){
	strcpy(usr_str,str_find);
	printf("Palavras iguais a '%s' na coluna %s:\n", usr_str, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col && strcmp(str_temp,"NULL")){ 
	    if(strcmp(usr_str,str_temp)==0){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      }

      break;
    case 4:
      if(tipo_campo==int_){
	sscanf(str_find,"%d",&usr_int);
	printf("Números menores que %d na coluna %s:\n", usr_int, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ //listas todos os números MAIORES que o passado pelo usuário
	    sscanf(str_temp,"%d",&tab_int);
	    if(usr_int>tab_int){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
				    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==float_){
	sscanf(str_find,"%f",&usr_float);
	printf("Números menores que %f na coluna %s:\n", usr_float, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%f",&tab_float);
	    if(usr_float>tab_float){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==double_){
	sscanf(str_find,"%lf",&usr_double);
	printf("Números menores que %lf na coluna %s:\n", usr_double, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%lf",&tab_double);
	    if(usr_double>tab_double){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==char_){
	sscanf(str_find,"%c",&usr_char);
	printf("Caracteres lexicograficamente menores que '%c' na coluna %s:\n", usr_char, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%c",&tab_char);
	    if(usr_char>tab_char){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}

      } else if(tipo_campo==string_){
	strcpy(usr_str,str_find);
	printf("Palavras lexicograficamente menores que '%s' na coluna %s:\n", usr_str, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col && strcmp(str_temp,"NULL")){ 
	    if(strcmp(usr_str,str_temp)>0){ //se str_temp < usr_str
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}

      }

      break;
    case 5:
      if(tipo_campo==int_){
	sscanf(str_find,"%d",&usr_int);
	printf("Números menores ou iguais a %d na coluna %s:\n", usr_int, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ //listas todos os numeros MAIORES que o passado pelo usuário
	    sscanf(str_temp,"%d",&tab_int);
	    if(usr_int>=tab_int){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
				    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==float_){
	sscanf(str_find,"%f",&usr_float);
	printf("Números menores ou iguais a %f na coluna %s:\n", usr_float, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%f",&tab_float);
	    if(usr_float>=tab_float){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==double_){
	sscanf(str_find,"%lf",&usr_double);
	printf("Números menores ou iguais a %lf na coluna %s:\n", usr_double, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%lf",&tab_double);
	    if(usr_double>=tab_double){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}
      } else if(tipo_campo==char_){
	sscanf(str_find,"%c",&usr_char);
	printf("Caracteres lexicograficamente maiores ou iguais a %c na coluna %s:\n", usr_char, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col){ 
	    sscanf(str_temp,"%c",&tab_char);
	    if(usr_char>=tab_char){
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}

      } else if(tipo_campo==string_){
	strcpy(usr_str,str_find);
	printf("Palavras lexicograficamente menores ou iguais a '%s' na coluna %s:\n", usr_str, usr_campo);

	line=0; col=0;
	while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	  if(line>1 && col==0){
	    strcpy(id_print,str_temp);
	  }
	  
	  if(line>1 && col==usr_col && strcmp(str_temp,"NULL")){ //AQUI<<<<<<<<<<<<<<<
	    if(strcmp(usr_str,str_temp)>=0){ //se str_temp <= usr_str
	      listaLinha_tab(nome_tab, tab->C, id_print);
	    }
	  }
	  fgetc(tab_file); // pule delimitador ;
			    
	  if(col==tab->C-1){
	    line++;
	    col=-1;
	  }
	  col++;
	}

      }
    
      break;
    case 6:
      if(tipo_campo==string_){

	if(str_find[0]=='*' && str_find[strlen(str_find)-1]=='*'){ //função que faz busca do tipo *str*

	  for(i=1;i<(strlen(str_find)-1);i++){
	    usr_str[i-1]=str_find[i];
	  }
	  usr_str[strlen(str_find)-2]='\0';

	  printf("\nCampos que contém '%s' na coluna %s:\n", usr_str, usr_campo);
	  
	  line=0; col=0;
	  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	    if(line>1 && col==0){
	      strcpy(id_print,str_temp);
	    }
	    
	    if(line>1 && col==usr_col){ 
	      if(strcasestr(str_temp,usr_str)!=NULL){
		listaLinha_tab(nome_tab, tab->C, id_print);
	      }
	    }
	    fgetc(tab_file); // pule delimitador ;
			    
	    if(col==tab->C-1){
	      line++;
	      col=-1;
	    }
	    col++;
	  }
	  

	} else if(str_find[0]=='*'){ //função que faz busca do tipo *str
	   
	  for(i=1;i<strlen(str_find);i++){
	    usr_str[i-1]=str_find[i];
	  }
	  usr_str[strlen(str_find)-1]='\0';

	  printf("Campos que terminam com '%s' na coluna %s:\n", usr_str, usr_campo);
	  
	  line=0; col=0;
	  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	    if(line>1 && col==0){
	      strcpy(id_print,str_temp);
	    }
	    
	    if(line>1 && col==usr_col){

	      str=str_temp;
	      do{
		str=strcasestr(str,usr_str);

		if(str!=NULL){
		  if(strlen(str)==strlen(usr_str)){
		    listaLinha_tab(nome_tab, tab->C, id_print);
		    break;
		  }
		  str=str+1;
	
		}
	      }while(str!=NULL);
	    }
	    fgetc(tab_file); // pule delimitador ;
			    
	    if(col==tab->C-1){
	      line++;
	      col=-1;
	    }
	    col++;
	  }
	  

	} else if(str_find[strlen(str_find)-1]=='*'){ //função que faz busca do tipo str*

	  for(i=0;i<strlen(str_find)-1;i++){
	    usr_str[i]=str_find[i];
	  }
	  usr_str[strlen(str_find)-1]='\0';

	  printf("Campos que iniciam com '%s' na coluna %s:\n", usr_str, usr_campo);
	  
	  line=0; col=0;
	  while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	    if(line>1 && col==0){
	      strcpy(id_print,str_temp);
	    }
 
	    if(line>1 && col==usr_col){ 
	      str=strcasestr(str_temp,usr_str);

	      if(str!=NULL){
		if(strlen(str)==strlen(str_temp))
		  listaLinha_tab(nome_tab, tab->C, id_print);
	      }
	    }
	    fgetc(tab_file); // pule delimitador ;
			    
	    if(col==tab->C-1){
	      line++;
	      col=-1;
	    }
	    col++;
	  }

	} else { //função que faz busca do tipo str*str
	  cont=0;
	  for(i=0;i<strlen(str_find);i++){
	    if(str_find[i]=='*'){
	      cont++;
	      break;
	    }
	  }

	  if(cont==1){

	    for(j=0;j<i;j++){
	      usr_str[j]=str_find[j];
	    }
	    usr_str[i]='\0';

	    int k=0;
	    for(j=i+1;j<strlen(str_find);j++){
	      usr_str2[k]=str_find[j];
	      k++;
	    }
	    usr_str2[k]='\0';


	    printf("Campos que iniciam com '%s' e terminam com '%s' na coluna %s:\n", usr_str, usr_str2, usr_campo);
	  
	    line=0; col=0;
	    while(fscanf(tab_file," %[^;]s",str_temp)!=EOF){
	      if(line>1 && col==0){
		strcpy(id_print,str_temp);
	      }
 
	      if(line>1 && col==usr_col){

		str=strcasestr(str_temp,usr_str);

		if(str!=NULL){
		  if(strlen(str)==strlen(str_temp)){ // se inicia com a substring usr_str
		    
		    str2=str_temp+strlen(usr_str);
		    do{
		      str2=strcasestr(str2,usr_str2);
		      
		      if(str2!=NULL){
			if(strlen(str2)==strlen(usr_str2)){
			  listaLinha_tab(nome_tab, tab->C, id_print);
			  break;
			}

			str2=str2+1;
	
		      }
		    }while(str2!=NULL);
		    
		  }
		}
	      }
	      fgetc(tab_file); // pule delimitador ;
			    
	      if(col==tab->C-1){
		line++;
		col=-1;
	      }
	      col++;
	    }
	    
	  }else {
	    printf("\n>>> ERRO: Operação só funciona com um *.\n\n");
	  }

	}
       
      } else {
	printf("\n>>> ERRO: Operação só funciona com string.\n\n");
      }
      break;
    case 7:
      freeStruct_tab(tab);
      return 1;
      break;
    default:
      printf("\n>>> ERRO: Operação inválida.\n");
    }

    fclose(tab_file);
  }while(1); 
 
  // FUNÇÃO opera apenas em inteiros, float, double e char por enquanto. Falta fazer para strings
  
  freeStruct_tab(tab);
  return 1;
}

void apagarLinha(char nome_tab[50], int n){
  FILE *tab_file,*file_temp;
  int col, linha, cont;
  int existe_id; //bool
  char chave[10],chave_user[10], aux[50];

  tabela *tab; //tipo struct tabela_t

  tab=alocaStruct_tab(n);  
  fillStruct_tab(tab, nome_tab, n);

  // Pede a chave primária(ID), caso não exista repete o processo
  tab_file = fopen(nome_tab, "r");
  
  do{ 
    printf("Insira o ID da linha: ");
    scanf(" %s", chave_user);

    //procura o ID da linha que será apagada
    rewind(tab_file);
    cont=0;
    existe_id=0;
    while(fscanf(tab_file," %[^;]s", chave)!=EOF){
    	
      if(cont>=2 && strcmp(chave, chave_user)==0){
	existe_id=1;
	printf("Linha a ser apagada: %d\n", cont+1); //cont+1 é a linha exata que será apagada do arquivo
	break;
      }

      fscanf(tab_file, " %*[^\n]s");
      cont++;	
    }

    if(existe_id)
      printf("Achei o ID %s.\n", chave);
    else
      printf("\n>>> ERRO: ID não existe.\n\n"); // colocar um return aqui? ** Melhoria **
    
  } while(!existe_id);

  //volta para o inicio da arquivo
  rewind(tab_file);

  //cria um arquivo temporário
  file_temp=fopen("fileTemp", "w+");
  if(file_temp==NULL)
    printf("\nAlerta: Arquivo na linha %d não abriu!\n\n", __LINE__);
 
  linha=0;
  col=-1;   
  //copia o arquivo da tabela excluindo a linha cont (linha do id)
  while(fscanf(tab_file," %[^;]s", aux)!=EOF){
    col++;

    if(linha!=cont){
      fprintf(file_temp, "%s;", aux);
      if(col==tab->C-1){
	fprintf(file_temp,"\n");
      }	
    }

    fgetc(tab_file);

    if(col==tab->C-1){
      linha++;
      col=-1;
    }
  }

  fclose(tab_file);
  remove("nome_tab");
  rename("fileTemp", tab->nome);
  fclose(file_temp);

  //liberando memória
  freeStruct_tab(tab);
}

void apagarTab(char nome_tab[50], int n){
  FILE *relacao_file, *file_temp;
  char aux[50];
  int num;
		
  tabela *tab; //tipo struct tabela_t

  tab=alocaStruct_tab(n);  
  fillStruct_tab(tab, nome_tab, n);

  //abri o arquivo relacaoTab para ler as tabelas existentes
  relacao_file = fopen("relacaoTab", "r");
  if(relacao_file == NULL)
    printf("\nALERTA: Relação de tabelas na linha %d não abriu!\n",__LINE__);

  file_temp=fopen("fileTemp", "a+");
  if(file_temp==NULL)
    printf("\nALERTA: Arquivo na linha %d não abriu!\n\n",__LINE__);

  //Copiando o arquivo relacaoTab para um temporário pulando alinha que contém a tabela a ser excluida
  while(fscanf(relacao_file," %s %d", aux, &num)!=EOF){
    if(strcmp(aux, nome_tab)==0){
      // nada

    } else{
      fprintf(file_temp, "%s %d", aux, num);
      fprintf(file_temp, "\n");
    }
  }

  rename("fileTemp", "relacaoTab");
  fclose(file_temp);
  fclose(relacao_file); 
  remove(tab->nome); 

  //liberando memória
  freeStruct_tab(tab);
}
