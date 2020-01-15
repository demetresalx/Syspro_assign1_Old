#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include "doc_map.h"
#include "worker_trie.h"
#include "server.h"
#include "worker_client.h"
#include "worker_utils.h"
#define PERMS 0666
#define FIFO1 "./seitutsi/myfifo"


void inverted_index_init(trie_node ** root)
{
  if(*root == NULL)
  {
    *root = malloc(sizeof(trie_node));
    (*root)->token = '\0';
    (*root)->next = NULL;
    (*root)->child = NULL;
    (*root)->post_list = NULL;
  }
}


void inverted_index_insert(trie_node ** root, doc_map_node * doc_map, int doc_map_size, int * bytes_counter ,int * words_counter)
{
  int j;
  char a;
  char * pch;
  char delimiter[]=" \t\r\n\v\f"; // POSIX whitespace characters
  char * strtool = malloc(2*sizeof(char));
  for(int i=0; i<doc_map_size; i++)	//thewrw doc_map_size grammes tha exei to keimeno, ara pernaw kai arithmo grammhs
  {
    strtool = realloc(strtool, strlen(doc_map[i].text)*sizeof(char)+sizeof(char));
    strcpy(strtool, doc_map[i].text);
    pch = strtok(strtool, delimiter);
    *words_counter += 1;
    //*bytes_counter += strlen(pch);
    while (pch != NULL)//pairnoyme leksh leksh
    {
      //puts(pch);
      inverted_index_leaf_ins(&((*root)->child), pch, doc_map[i].path, doc_map[i].text, i);	//doc_map[i].id ws teleutaio orisma eixa
      pch = strtok (NULL, delimiter); //phrame  epomenh leksh
      *words_counter += 1;
	//*bytes_counter += strlen(pch);

    }
  }
  free(strtool);
}

int inverted_index_leaf_ins(trie_node ** nd, char * word,char * file_path, char * lineptr, int ln)
{
  if(*nd == NULL){ //den yparxei kombos se auto to epipedo
    *nd = malloc(sizeof(trie_node));
    (*nd)->token = word[0];
    (*nd)->next = NULL;
    (*nd)->child = NULL;
    (*nd)->post_list = NULL;
    if(strlen(word) ==1){  //teleutaio char
      posting_list_update(&((*nd)->post_list), file_path, lineptr, ln);
      return 0;
    }
    else if(strlen(word)>1){
      word++ ;
      inverted_index_leaf_ins(&((*nd)->child), word, file_path, lineptr, ln);
    }
  }
  else{ //yparxoyn komboi se auto to epipedo
      if(word[0] > (*nd)->token)  //alfabhtikos elegxos
        inverted_index_leaf_ins(&((*nd)->next), word, file_path, lineptr, ln);
      else if(word[0] == (*nd)->token){ //pare epomeno xarakthra kai pame sto katw epipedo
        if(strlen(word) ==1){  //teleutaio char
          posting_list_update(&((*nd)->post_list), file_path, lineptr, ln);
          return 0;
        }
        else if(strlen(word)>1){
          word++ ;
          inverted_index_leaf_ins(&((*nd)->child), word, file_path, lineptr, ln);
        }
      }
      else{ //periptwsh poy prohgeitai alfabhtika
        trie_node * new_head = malloc(sizeof(trie_node));
        new_head->token = word[0];
        new_head->child = NULL;
        new_head->post_list = NULL;
        new_head->next = *nd;
        *nd = new_head;
        if(strlen(word) ==1){  //teleutaio char
          posting_list_update(&((*nd)->post_list), file_path, lineptr, ln);
          return 0;
        }
        else if(strlen(word)>1){
          word++ ;
          inverted_index_leaf_ins(&((*nd)->child), word, file_path, lineptr, ln);
        }
      }

  }


  return 0;
}

int posting_list_update(posting_list_node ** node, char * fpath, char * ptrline, int ln)
{
  if(*node == NULL){
    *node = malloc(sizeof(posting_list_node));
    (*node)->next = NULL;
    (*node)->file_path = fpath;
    (*node)->times_encountered =1;
    (*node)->linfo = NULL;
    line_info_update(&((*node)->linfo), ptrline,  ln);
    return 0;
  }
  else{  //exei ksanabre8ei h leksh
    posting_list_node *cur = *node;
    while(cur != NULL){
      if(strcmp(fpath, cur->file_path)==0)
        {cur->times_encountered++; line_info_update(&(cur->linfo), ptrline,  ln); return 0;}
      cur = cur->next;
    }
    posting_list_update(&((*node)->next), fpath, ptrline, ln); //se periptwsh poy den to brhke paei na to ftiaksei
  }
}



int posting_list_count(trie_node * leaf)
{
  posting_list_node * cur = leaf->post_list ;
  int i =0;
  while(cur != NULL){
    //i += cur->times_encountered ; //GIA ARITHMO MESA STO KEIMENO
    i++ ; //SE POSA KEIMENA BRE8HKE
    cur = cur->next;
  }

  return i;
}

void destroy_posting_list(posting_list_node ** plst)
{

  if( (*plst)->next != NULL)
    destroy_posting_list(&((*plst)->next));

  free(*plst);
}

int line_info_update(line_info ** node, char * ptrline, int ln)
{
	if(*node == NULL){
    	*node = malloc(sizeof(line_info));
    	(*node)->dore_kurai = 1;
		(*node)->line_numbers = malloc(1*sizeof(int));
		(*node)->line_numbers[0] = ln;
		(*node)->lines = malloc(1*sizeof(char *));
		(*node)->lines[0] = ptrline;
    	return 0;
  	}
	else{  //exei data mesa o komvos
    	for(int i=0; i< (*node)->dore_kurai ; i++)
			if( (*node)->line_numbers[i] == ln )
				return 0;	//yparxei hdh sth grammh auth, mhn asxolh8eis allo
		
		//den eixame return 0 gia na ftasoume edw, ara pame na kanoyme update
		(*node)->dore_kurai += 1;	//aykshsh kata 1
		(*node)->line_numbers = realloc((*node)->line_numbers, (*node)->dore_kurai *sizeof(int));
		(*node)->line_numbers[ (*node)->dore_kurai -1 ] = ln;
		(*node)->lines = realloc((*node)->lines, (*node)->dore_kurai *sizeof(char *));
		(*node)->lines[ (*node)->dore_kurai -1 ] = ptrline;
		return 0;
  	}
}



int destroy_trie(trie_node ** root)
{
  if(*root ==NULL)  //adeio
    return 0;

  if((*root)->token =='\0')
    destroy_trie(&((*root)->child));

  if((*root)->next != NULL)
    destroy_trie(&((*root)->next));

  if((*root)->post_list != NULL)
    destroy_posting_list(&((*root)->post_list));

  if((*root)->child != NULL)
    destroy_trie(&((*root)->child));

  free(*root);
  return 0;
}

void user_print_trie(trie_node * root)
{
  token_list * stk = malloc(sizeof(token_list));
  stk->size = 4;
  stk->array = malloc(stk->size * sizeof(char));
  char brd[200] ;
  int i =0;
  int *size = malloc(sizeof(int));
  *size = 4;
  print_trie(root, stk, i);
  free(stk->array);
  free(stk);
}

int print_trie(trie_node * root, token_list * stri, int lvl)
{
  char tok[2];
  tok[1] ='\0';

  if(root == NULL)
    return 0;

  if(root->token == '\0') //h riza
    return print_trie(root->child, stri, lvl);

  if(root->next != NULL)
    print_trie(root->next,stri, lvl);

  tok[0] = root->token;
  insert_at_level(stri , tok[0], lvl);
  if(root->post_list != NULL){ //exoyme leksh

      print_list(stri , lvl );
    printf(" %d\n" ,posting_list_count(root));
  }
  if(root->child == NULL){ //bottom-most komvos
    //tipota?
  }

   print_trie(root->child, stri, lvl+1);

}

void insert_at_level(token_list * Lst, char a, int level)
{
  int crap =0;
  int oldsize = Lst->size;
  while(level >= Lst->size){
    crap =1;
    Lst->size =   Lst->size * 2 + level;
    Lst->array = realloc(Lst->array, Lst->size * sizeof(char));
  }
  int i;
  if(crap ==1){
    for(i=oldsize-1; i<Lst->size -1; i++)
      Lst->array[i] = ' ';
  }
  Lst->array[level] = a;
  Lst->array[strlen(Lst->array)] = '\0';

}

void print_list(token_list * thelist, int level)
{
  int i;
  for(i=0; i<level+1; i++)
    printf("%c", thelist->array[i]);
}
