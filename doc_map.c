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
#define PERMS 0666
#define FIFO1 "./seitutsi/myfifo"

int doc_map_init(char * filename, doc_map_node ** mappo, int * kappo)
{

	doc_map_node * doc_map;
	int doc_map_size;

	char *ln = NULL;            /* NULL forces getline to allocate  */
  size_t n = 0;               /* buf size, 0 use getline default  */
  ssize_t nchr = 0;           /* number of chars actually read    */
  size_t index = 0;           /* array index for number of lines  */
  size_t it = 0;              /* general iterator variable        */
  size_t lmax = 255;         /* current array pointer allocation */


	int maxsizee =2;
	char * a = malloc(maxsizee*sizeof(char));

  if (!(doc_map = calloc (255, sizeof(doc_map_node)))) {
        fprintf (stderr, "error: memory allocation failed.");
        return 1;
    }
	//printf("THA ANOIKSW TO %s\n", filename);

	  FILE * fp = fopen(filename, "r");
		if(fp == NULL)
			puts("boh8eiaaa");

	while((nchr = getline (&ln, &n, fp)) != -1)//diabazei grammh grammh to arxeio
  {
    //strcpy(id_extractor, "");
      while (nchr > 0 && (ln[nchr-1] == '\n' || ln[nchr-1] == '\r'))
          ln[--nchr] = 0; /* strip newline or carriage rtn    */

          if (strcmp(ln,"\0") == 0){  //de dinei shmasia se adeia grammh
            continue;
        }
          /* allocate & copy ln to array - this will create a block of memory
             to hold each character in ln and copy the characters in ln to that
             memory address. The address will then be stored in array[idx].
             (idx++ just increases idx by 1 so it is ready for the next address)
             There is a lot going on in that simple: array[idx++] = strdup (ln);
          */
          //gia na xwrisoume to id ap, to keimeno

      if(strlen(ln) > maxsizee){  //giati den kseroume megethos grammhs
        maxsizee = strlen(ln);
        a = realloc(a, maxsizee*sizeof(char));
      }

        strcpy(a, ln);
        /*int i=-1;
        do {
          i++;
          if((a[i] >= '0') && (a[i] <= '9'))//einai arithmos
            {token[0] = a[i]; strcat(id_extractor, token);}
          else
            {if(a[i] != ' ' && a[i] != '\t'){puts("problematc id detected..."); return 1;}}

        } while((a[i] != ' ') && (a[i] != '\t'));
        char * tok = a+(i+1)*sizeof(char);*/
        //strcpy(ln, tok);
		strcpy(ln, a);

        doc_map[index].path = malloc((strlen(filename) +1)*sizeof(char)); 
	strcpy(doc_map[index].path, filename);
      //  printf("toso einai %d %s\n",doc_map[index].id, id_extractor );
      doc_map[index++].text = strdup(ln);
      //puts(ln);
	//puts(doc_map[index-1].text);

      if (index == lmax) {     //  if lmax lines reached, realloc   
          doc_map_node* tmp = realloc(doc_map, lmax * 2*sizeof(doc_map_node));
          if (!tmp)
             { puts("eleos");return -1;}
          doc_map = tmp;
          lmax *= 2;
      }

  //if(atoi(id_extractor) != prev_id +1)//id ektos seiras
   // {printf("Error with text ids' number\n");return 1;}
  //prev_id = atoi(id_extractor);
    doc_map_size =index;

  }
	/*int i;
  for(i=0; i<doc_map_size; i++)
    {puts(doc_map[i].path);puts(doc_map[i].text);free(doc_map[i].text);}*/
	*mappo = doc_map;
	*kappo = doc_map_size;

}

void doc_map_destroy(doc_map_node ** mappo, int doc_map_size)
{
	doc_map_node * doc_map = *mappo ;
  int i;
  for(i=0; i<doc_map_size; i++)
    {free(doc_map[i].text);free(doc_map[i].path);}

  free(doc_map);
}
