#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SProductRef
{
  char name[50];
  int keyRef;
} ProductRef;

enum EProductInfo
{
  DELETED = 0,
  PURCHASE_PRICE,
  SALE_PRICE,
  STOCK_QTY,
  SOLD_QTY
};

typedef struct SProductInfo
{
  int deleted;
  float purchPrice;
  float salePrice;
  int stockQty;
  int soldQty;
} ProductInfo;

enum ECases
{
  SUCCESS = 0,
  STOCK_BELOW,
  PRODUCT_NOT_FOUND,
  INVALID_VALUE,
  PROCCESS_ERROR
};

enum EFiles
{
  KEY_BIN = 0,
  INFS_BIN
};

/**********************************GLOBAIS*************************************/

/*********************************ESTRUTURAS***********************************/

/********************************PROTOTIPOS************************************/
FILE *openFile(char *path);
ProductRef *loadProductsRef(int *ctRefs);
ProductInfo *loadProductsInfo(int *ctInfos);
int binarySearchByName(ProductRef *D, int n, char k[50]);
int sellProduct(ProductRef *refs, ProductInfo *infos, int ctRefs);
void registerProduct(ProductInfo **infos, int *ctInfos, ProductRef **refs, int *ctRefs);
int increaseStock(ProductRef *refs, ProductInfo *infos, int ctRefs);
int removeProduct(ProductRef *refs, int *ctRefs, ProductInfo *infos, int *ctInfos);
void getStock(ProductInfo *infos, int ctInfos, ProductRef *refs, int ctRefs);
void getSaleValue(ProductInfo *infos, int ctInfos, ProductRef *refs, int ctRefs);
int saveChanges(char *path, int cols, int which, ProductRef *refs, ProductInfo *infos, int ct);
void convert(char *parser, int j, ProductInfo info, ProductRef ref, int which);
void exitApp(ProductInfo *infos, ProductRef *refs, int ctInfos, int ctRefs);
void partition(int left, int right, int *i, int *j, ProductRef *v);
void sort(int left, int right, ProductRef *v);
void quicksort(ProductRef *v, int n);

/**********************************MAIN****************************************/
int main(void)
{
  ProductRef *refs = malloc(sizeof(ProductRef) * 5);
  ProductInfo *infos = malloc(sizeof(ProductInfo) * 5);

  int ctRefs = 0, ctInfos = 0;
  int i;

  printf("Antes de tudo, entre com pelo menos 5 produtos inicias\n");
  for (i = 0; i < 5; i++)
  {
    registerProduct(&infos, &ctInfos, &refs, &ctRefs);
    printf("\n");
  }

  system("cls");

  int op, res;

  printf("\n*----------------------------------------------------------------------------------*\n");
  printf("*------------------- BEM VINDO AO SISTEMA DE CONTROLE DE VENDAS -------------------*\n");
  printf("*----------------------------------------------------------------------------------*\n\n");

  do
  {
    printf("*------------------MENU-------------------*\n");
    printf("| Digite   |             Para             |\n");
    printf("*-----------------------------------------*\n");
    printf("|   1      | Vender um produto            |\n");
    printf("|   2      | Cadastrar um novo produto    |\n");
    printf("|   3      | Remover produto do estoque   |\n");
    printf("|   4      | Consultar estoque            |\n");
    printf("|   5      | Consultar lucro              |\n");
    printf("|   6      | Repor estoque                |\n");
    printf("|   0      | Sair do sistema              |\n");
    printf("*-----------------------------------------*\n\n");

    printf("Digite aqui: ");
    scanf("%d", &op);
    system("cls");

    switch (op)
    {
    // Vender um produto
    case 1:
    {
      res = sellProduct(refs, infos, ctRefs);

      if (res == STOCK_BELOW)
        printf("Nao ha estoque disponivel para atender a venda.\n");
      else if (res == PRODUCT_NOT_FOUND)
        printf("Produto nao encontrado.\n");
    }
    break;
    // Cadastrar um novo produto
    case 2:
    {
      registerProduct(&infos, &ctInfos, &refs, &ctRefs);
      break;
    }
    // Remover produto do estoque
    case 3:
    {
      res = removeProduct(refs, &ctRefs, infos, &ctInfos);

      if (res == PRODUCT_NOT_FOUND)
        printf("Produto nao encontrado.\n");
    }
    break;
    // Consultar estoque
    case 4:
      getStock(infos, ctInfos, refs, ctRefs);
      break;
    // Consultar lucro
    case 5:
      getSaleValue(infos, ctInfos, refs, ctRefs);
      break;
    // Repor estoque
    case 6:
    {
      res = increaseStock(refs, infos, ctRefs);

      if (res == INVALID_VALUE)
      {
        printf("O valor deve maior que 0.\n");
      }
      else if (res == PRODUCT_NOT_FOUND)
      {
        printf("Produto nao encontrado.\n");
      }
    }
    break;
    // Sair do sistema
    case 0:
    {
      res = saveChanges("chave.bin", 2, 0, refs, infos, ctRefs);
      res = saveChanges("infs.bin", 5, 1, refs, infos, ctInfos);

      if (res == PROCCESS_ERROR)
        printf("O processamento falhou!\n");
      else if (res == SUCCESS)
        printf("Arquivo salvo com sucesso!\n\n");

      exitApp(infos, refs, ctInfos, ctRefs);
      system("pause");
      break;
    }
    default:
      printf("Erro: Digite um dos numeros listados\n");
      break;
    }
  } while (op != 0);
}

/*********************************FUNCOES**************************************/
FILE *openFile(char *path)
{
  FILE *file;

  file = fopen(path, "wb");

  if (file == NULL)
    printf("Erro: O arquivo binario nao pode ser aberto. Verifique-o.\n");

  return file;
}

int sellProduct(ProductRef *refs, ProductInfo *infos, int ctRefs)
{
  char name[50];
  int i, qty;
  int line, posInfo;

  printf("Digite o nome do produto: ");
  scanf(" %[^\n]%*c", name);

  for (i = 0; i < ctRefs; i++)
  {

    line = binarySearchByName(refs, ctRefs, name);

    if (line != -1)
    {
      posInfo = refs[line].keyRef - 1;

      printf("Preco: R$ %.2f\n", infos[posInfo].salePrice);
      printf("Estoque: %d\n", infos[posInfo].stockQty);
      printf("Quantas unidades deseja comprar?\n");
      printf("Digite aqui: ");
      scanf("%d", &qty);

      if (infos[posInfo].stockQty >= qty)
      {
        printf("O valor total da compra e: R$ %.2f\n", (float)(infos[posInfo].salePrice * qty));
        infos[posInfo].stockQty -= qty;
        infos[posInfo].soldQty += qty;

        return SUCCESS;
      }
      else
      {
        return STOCK_BELOW;
      }
    }
    else
      break;
  }

  return PRODUCT_NOT_FOUND;
}

void registerProduct(ProductInfo **infos, int *ctInfos, ProductRef **refs, int *ctRefs)
{
  int i;

  *refs = realloc(*refs, sizeof(ProductRef) * (*ctRefs + 1));

  int line = -1;

  for (i = 0; i < *ctInfos; i++)
  {
    if ((*infos)[i].deleted == 1)
    {
      line = i;
      break;
    }
  }

  if (line == -1)
  {
    line = *ctInfos;
    *infos = realloc(*infos, sizeof(ProductInfo) * (*ctInfos + 1));
    (*ctInfos)++;
  }

  printf("Digite o nome: ");
  scanf(" %[^\n]%*c", (*refs)[*ctRefs].name);

  printf("Digite o preco de compra: ");
  scanf("%f", &(*infos)[line].purchPrice);

  printf("Digite o preco de venda: ");
  scanf("%f", &(*infos)[line].salePrice);

  printf("Digite a quantidade no estoque: ");
  scanf("%d", &(*infos)[line].stockQty);

  printf("Digite a quantidade vendida: ");
  scanf("%d", &(*infos)[line].soldQty);

  (*infos)[line].deleted = 0;
  (*refs)[*ctRefs].keyRef = line + 1;
  (*ctRefs)++;

  quicksort(*refs, *ctRefs);

  return;
}

int increaseStock(ProductRef *refs, ProductInfo *infos, int ctRefs)
{
  char name[50];
  int i, line, posInfo, restock;

  printf("Digite o nome do produto: ");
  scanf(" %[^\n]%*c", name);

  line = binarySearchByName(refs, ctRefs, name);

  posInfo = refs[line].keyRef - 1;

  printf("Digite a quantidade de reestoque: ");
  scanf("%d", &restock);

  if (restock > 0)
  {
    infos[posInfo].stockQty += restock;
    printf("Reestoque efetuado! Estoque atual: %d\n", infos[posInfo].stockQty);

    return SUCCESS;
  }
  else
  {
    return INVALID_VALUE;
  }

  return PRODUCT_NOT_FOUND;
}

int removeProduct(ProductRef *refs, int *ctRefs, ProductInfo *infos, int *ctInfos)
{
  char name[50];
  int line, posInfo = 0;
  int i;

  printf("Digite o nome: ");
  scanf(" %[^\n]%*c", name);

  line = binarySearchByName(refs, *ctRefs, name);

  if (line != -1)
  {

    posInfo = (refs[line].keyRef) - 1;
    infos[posInfo].deleted = 1;

    for (i = line; i < *ctRefs; i++)
      refs[i] = refs[i + 1];

    (*ctRefs)--;

    quicksort(refs, *ctRefs);
    return SUCCESS;
  }

  return PRODUCT_NOT_FOUND;
}

void getStock(ProductInfo *infos, int ctInfos, ProductRef *refs, int ctRefs)
{
  int i, line, posInfo;
  float totalProduct = 0, totalStock = 0;

  for (i = 0; i < ctInfos; i++)
  {
    line = binarySearchByName(refs, ctRefs, refs[i].name);
    posInfo = refs[line].keyRef - 1;

    if (line != -1)
    {
      totalProduct = (float)(infos[posInfo].stockQty * infos[posInfo].purchPrice);
      totalStock += totalProduct;

      printf("Produto: %s\n", refs[i].name);
      printf("Estoque: %d\n", infos[posInfo].stockQty);
      printf("Valor total de cada produto do estoque: %.2f\n\n", totalProduct);
    }
  }

  printf("Valor total do estoque: %.2f\n\n", totalStock);

  return;
}

void getSaleValue(ProductInfo *infos, int ctInfos, ProductRef *refs, int ctRefs)
{
  int i, line, posInfo;

  for (i = 0; i < ctInfos; i++)
  {
    line = binarySearchByName(refs, ctRefs, refs[i].name);
    posInfo = refs[line].keyRef - 1;

    if (line != -1)
    {
      printf("Produto: %s\n", refs[i].name);
      printf("Total de venda: %.2f\n\n", (float)(infos[posInfo].salePrice * infos[posInfo].soldQty));
    }
  }

  return;
}

int binarySearchByName(ProductRef *D, int n, char k[50])
{
  int inf = 0, sup = n - 1;

  while (inf <= sup)
  {
    int half = (inf + sup) / 2;

    if (strcmp(k, D[half].name) < 0)
      sup = half - 1;
    else if (strcmp(k, D[half].name) > 0)
      inf = half + 1;
    else
    {
      return half;
    }
  }

  return -1;
}

int saveChanges(char *path, int cols, int which, ProductRef *refs, ProductInfo *infos, int ct)
{
  FILE *file = openFile(path);

  int i, j, k, ct2 = 0, length = 0;
  char *parser;

  parser = malloc(sizeof(char));

  for (i = 0; i < ct; i++)
  {
    for (j = 0; j < cols; j++)
    {
      convert(parser, j, infos[i], refs[i], which);
      length += strlen(parser);

      if (j != (cols - 1))
        length += 1;
    }

    if (i != (ct - 1))
      length += 1;
  }

  char str[length];

  for (i = 0; i < ct; i++)
  {
    for (j = 0; j < cols; j++)
    {
      convert(parser, j, infos[i], refs[i], which);

      for (k = 0; k < strlen(parser); k++)
      {
        str[ct2] = *(parser + k);
        ct2++;
      }

      if (j != (cols - 1))
      {
        str[ct2] = ' ';
        ct2++;
      }
    }

    if (i != (ct - 1))
    {
      str[ct2] = '\n';
      ct2++;
    }
  }

  str[ct2] = '\0';

  if (fputs(str, file) == EOF)
    return PROCCESS_ERROR;

  fclose(file);

  return SUCCESS;
}

void convert(char *parser, int j, ProductInfo info, ProductRef ref, int which)
{
  if (which == INFS_BIN)
  {
    switch (j)
    {
    case DELETED:
      sprintf(parser, "%d", info.deleted);
      break;
    case PURCHASE_PRICE:
      sprintf(parser, "%.2f", info.purchPrice);
      break;
    case SALE_PRICE:
      sprintf(parser, "%.2f", info.salePrice);
      break;
    case STOCK_QTY:
      sprintf(parser, "%d", info.stockQty);
      break;
    case SOLD_QTY:
      sprintf(parser, "%d", info.soldQty);
      break;
    }
  }
  else if (which == KEY_BIN)
  {
    if (j == 0)
      sprintf(parser, "%d", ref.keyRef);
    else
      strcpy(parser, ref.name);
  }

  return;
}

void exitApp(ProductInfo *infos, ProductRef *refs, int ctInfos, int ctRefs)
{
  int i, line, posInfo;
  int salesTotal = 0;
  float profit, profitTotal;

  for (i = 0; i < ctInfos; i++)
  {
    line = binarySearchByName(refs, ctRefs, refs[i].name);
    posInfo = refs[line].keyRef - 1;

    if (line != -1)
    {

      salesTotal += infos[posInfo].soldQty;
      profit = infos[posInfo].salePrice - infos[posInfo].purchPrice;
      profitTotal += (float)(profit * infos[posInfo].soldQty);

      printf("*----------%s----------*\n\n", refs[i].name);

      printf("Quantidade vendida: %d\n", infos[posInfo].soldQty);
      printf("Quantidade em estoque: %d\n", infos[posInfo].stockQty);

      printf("Valor de compra: %.2f\n", infos[posInfo].purchPrice);
      printf("Valor de venda: %.2f\n", infos[posInfo].salePrice);

      printf("Valor total no estoque: %.2f\n\n", i, (float)(infos[posInfo].stockQty * infos[posInfo].purchPrice));
    }
  }

  printf("*--------------------------------------------*\n");
  printf("Total de vendas efetuadas: %d\n", salesTotal);
  printf("Lucro total do dia: %.2f\n", profitTotal);
  printf("*--------------------------------------------*\n");
}

void partition(int left, int right, int *i, int *j, ProductRef *v)
{
  ProductRef x, w;
  *i = left;
  *j = right;

  x = v[(*i + *j) / 2];

  do
  {
    while (strcmp(x.name, v[*i].name) > 0)
      (*i)++;
    while (strcmp(x.name, v[*j].name) < 0)
      (*j)--;

    if (*i <= *j)
    {
      w = v[*i];
      v[*i] = v[*j];
      v[*j] = w;

      (*i)++;
      (*j)--;
    }

  } while (*i <= *j);
}

void sort(int left, int right, ProductRef *v)
{
  int i, j;

  partition(left, right, &i, &j, v);

  if (left < j)
    sort(left, j, v);
  if (i < right)
    sort(i, right, v);
}

void quicksort(ProductRef *v, int n)
{
  sort(0, n - 1, v);
}
