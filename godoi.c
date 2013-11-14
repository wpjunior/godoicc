#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define READING_STRING 1
#define READING_INT 2
#define READING_VAR 3


char *PALAVRAS_RESERVADAS[] = {
  "inicio",
  "leia",
  "escreva",
  "var",
  "fim.",
  "se",
  "entao",
  "senao",
  "fim_se",
  "enquanto",
  "fim_enquanto"
};


typedef struct _Lista Lista;

struct _Lista
{
  char *variavel;
  int qtde;
  int utilizada;
  Lista *proxima;
};

bool inicia_com(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

Lista *variaveis = NULL;
Lista *palavras_usadas = NULL;

Lista *adiciona_lista(Lista *lista, char str[]) {
  char *dst;
  Lista *p;

  if (lista == NULL) {
    dst = malloc(sizeof(&str));
    strcpy(dst, str);

    p = malloc(sizeof(Lista));
    p->variavel = dst;
    p->qtde = 1;
    p->utilizada = 0;
    p->proxima = NULL;

    return p;
  } else {
    Lista *p = lista;
    dst = malloc(sizeof(&str));
    strcpy(dst, str);

    while(1) {
      if (!strcmp(p->variavel, str)) {
        p->qtde++;
        return lista;
      }

      if (p->proxima == NULL)
        break;

      p = p->proxima;
    }

    p->proxima = malloc(sizeof(Lista));
    p->proxima->variavel = dst;
    p->proxima->qtde=1;
    p->proxima->utilizada = 0;
    p->proxima->proxima = NULL;

    return lista;
  }
}

int inc_variavel_utilizada(Lista *lista, char str[]) {
  Lista *p = lista;
  while (p != NULL) {
    if (!strcmp(p->variavel, str)) {
      p->utilizada++;
      
      return 1;
    }
    p=p->proxima;
  }
  return 0;
}

void processa_variaveis(int num, char str[]) {
  int i, p=-1;
  char variavel[200];

  for (i=0; i<strlen(str); i++) {
    if (p >= 0) {

      if ((p == 0) && (str[i] >= 48) && (str[i] <= 57)) {
        printf("%d: %s -> Formato de variavel invalido\n", num, str);
        return;
      }

      if ((str[i] == ',') || (str[i] == ';') || (str[i] == ' ')||(str[i] == '=')) {
        variavel[p] = '\0';
        p=-1;
        
        variaveis = adiciona_lista(variaveis, variavel);

      } else {
        variavel[p] = str[i];
        p++;
      }
    }
    if (str[i] == '$')
      p=0;
   }
}

void processa_parametros(int num, char str[]) {
  /*
   * Processa os argumentos da função procurando strings e variaveis
   */
  int i, reading_pos=-1, parentesis_level = 0, reading=0;
  char variavel[200];

  for (i=0; i<strlen(str); i++) {
    if (reading == READING_VAR) {
      if ((str[i] >= 48) && (str[i] <= 57) && (reading_pos == 0)) {
        printf("%d: %s -> Formato de variavel invalido, variaveis nao podem comecar com numeros\n", num, str);
      }
      else if ((str[i] == ',') || (str[i] == ')')) {
        variavel[reading_pos] = '\0';
        reading_pos=-1;
        reading = 0;

        if (!inc_variavel_utilizada(variaveis, variavel)) {
          printf("%d: %s -> variavel \"%s\" nao esta declarada\n", num, str, variavel);
        }
      } else {
        variavel[reading_pos] = str[i];
        reading_pos++;
      }

    } else if (reading == READING_INT) {
      if ((str[i] == ',') || (str[i] == ')')) {
        variavel[reading_pos] = '\0';
        reading_pos=-1;
        reading = 0;

      } else if ((str[i] < 48) || (str[i] > 57)) {
        printf("%d: %s -> posicao %d, caractere %c nao esperado\n", num, str, i, str[i]);

      } else {
        reading_pos++;
        variavel[reading_pos] = str[i];
      }
    }
      
    if (str[i] == '(')
      parentesis_level++;
    else if (str[i] == ')')
      parentesis_level--;

    else if (str[i] == '$') {

      if (reading == 0) {
        reading_pos=0;
        reading=READING_VAR;
      } else {
        printf("%d: %s -> posicao %d, caractere $ nao esperado\n", num, str, i);
      }

      
    }
    /* quando achar um número */
    else if ((str[i] >= 48) && (str[i] <= 57) && (reading == 0)) {
      reading_pos=0;
      variavel[reading_pos] = str[i];
      reading=READING_INT;
    }
    /* quando achar uma string */
    else if (str[i] == '"') {
      if (reading == 0) {
        reading_pos=0;
        reading=READING_STRING;

      } else if (reading == READING_STRING) {
        reading = 0;
        reading_pos = -1;
      } else {
        printf("%d: %s -> posicao %d, caractere \" nao esperado\n", num, str, i);
      }
    }
   }

  if (parentesis_level != 0)
    printf("%d: %s -> parenteses nao balanceados\n", num, str);
}


void processa_se(int num, char str[]) {
}

void processa_senao(int num, char str[]) {
}

void processa_fim_se(int num, char str[]) {
}

void processa_entao(int num, char str[]) {
}

void processa_fim(int num, char str[]) {
}

void processa_linha(int num, char str[]) {
  int i;

  for (i=0; i<9; i++) {
    if (inicia_com(PALAVRAS_RESERVADAS[i], str)) {
      palavras_usadas = adiciona_lista(palavras_usadas, PALAVRAS_RESERVADAS[i]);
    }
  }
  // para a linha 1, sÃƒÂ³ ÃƒÂ© permitido a palavra inicio
  if (num == 1) {
    if (strcmp(str, "inicio")) {
      printf("%d: %s -> Palavra invalida, use \"inicio\" para a primeira linha do arquivo\n", num, str);
      return;
    } 
  }


  if (inicia_com("inicio", str)) {
    if (num > 1) {
      printf("%d: %s -> Palavra invalida, use \"inicio\" apenas no inicio do arquivo\n", num, str);
    }
  }
  else if (inicia_com("var", str)) {
    processa_variaveis(num, str);
  } else if (inicia_com("escreva", str)) {
    processa_parametros(num, str);
  }
  else if (inicia_com("leia", str)) {
    processa_parametros(num, str);
  }
  else if (inicia_com("$", str)) {
    processa_parametros(num, str);
  }
  else if (inicia_com("se", str)) {
    processa_se(num, str);
  }
  else if (inicia_com("entao", str)) {
    processa_entao(num, str);
  }
  else if (inicia_com("senao", str)) {
    processa_senao(num, str);
  }
  else if (inicia_com("fim_se", str)) {
    processa_fim_se(num, str);
  }
  else if (inicia_com("fim.", str)) {
    processa_fim(num, str);
  }
  else if (str[0] == '\0') {
    // detecta linha vazia
  }
  else {
    printf("%d: %s -> linha nao reconhecida\n", num, str);
  }
}

void compila(FILE *fp) {
  char str[200];
  int linha=1;

  while(fgets(str, sizeof(str), fp) != NULL)
    {
      // strip trailing '\n' if it exists
      int len = strlen(str)-1;
      if(str[len] == '\n') 
         str[len] = 0;

      processa_linha(linha, str);
      linha++; 
    }
  fclose(fp);
};

char *fgets_wrapper(char *buffer, size_t buflen, FILE *fp)
{
    if (fgets(buffer, buflen, fp) != 0)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n')
            buffer[len-1] = '\0';
        return buffer;
    }
    return 0;
}

int main(int argc, char *argv[])
{ 
  FILE *fp;
  char local[200];
   
  if (argc > 1) {
    fp=fopen(argv[1], "r");
    if (fp == NULL) {
      printf("Erro ao abrir arquivo: %s\n", argv[1]);
      system("PAUSE");
      return 1;
    }
  } else {
    fp = NULL;
    printf("Informe o caminho do codigo a ser compilado: ");
    fgets_wrapper(local, sizeof(local), stdin);
    fp=fopen(local,"r");
    
    while(fp == NULL){
      printf("Erro ao abrir arquivo: %s\n", local);
      printf("Caminho nao encotrado, digite o caminho novamente: ");
      fgets_wrapper(local, sizeof(local), stdin);
      fp=fopen(local,"r");
      system("cls");
      
    }
  }

  compila(fp);

  if (variaveis) {
    printf ("Variaveis encontradas: \n");
    Lista *p = variaveis;
    while (p != NULL) {
      if (p->utilizada > 0)
        printf ("%s, utilizada %d vezes.\n", p->variavel, p->utilizada);
      else
        printf ("AVISO: a variavel \"%s\" foi declarada mas nao utilizada.\n", p->variavel);
      p = p->proxima;
    }
  } else {
    printf ("Nenhuma variavel utilizada");
  }

  if (palavras_usadas) {
    printf ("Palavras reservadas encontradas: \n");
    Lista *p = palavras_usadas;
    while (p != NULL) {
      printf ("Palavra: %s, Quantidade: %d\n", p->variavel, p->qtde);
      p = p->proxima;
    }
  }
    
  system("PAUSE");
 
}
