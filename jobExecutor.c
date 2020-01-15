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


int countlines(char *filename)
{
  // count the number of lines in the file called filename
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL);
  return 0;

  lines++;
  while(!feof(fp))
	{
  		ch = fgetc(fp);
 	 	if(ch == '\n')
  		{
    		lines++;
 	 	}
	}
  fclose(fp);
  return lines;
}

int main(int argc, char *argv[])
{
  int i, numWorkers=5; //5 an de dieukrinistei
  FILE * fp;
  DIR *d;  //gia directory browsing
  struct dirent *dir; //gia directory browsing
  for(i=0; i<argc; i++)
  {
    if(strcmp("-d",argv[i])==0)
      {fp = fopen(argv[i+1], "r");printf("input catalogs file : %s\n", argv[i+1]);} //pairnw to swsto onoma arxeiou apo to command line

    if(strcmp("-w",argv[i])==0)
      {numWorkers = atoi(argv[i+1]);/*printf("%d\n", numWorkers);*/}  //dinw timh sto K vasei command line
  }
  int catalogs_num = countlines(argv[i+1]);

  //DHMIOURGIA TWN NAMED PIPES bash arxikou onomatos defined kai meta stadiakh aukshsh
  int readfd, writefd;  //ta pipes
  char pipename[128];
  strcpy(pipename, FIFO1);
  strcat(pipename ,"a");
  for(int j=0; j< 2*numWorkers; j++){
    if ( (mkfifo(pipename, S_IRWXU | S_IRWXG | S_IRWXO) < 0) ) {
      perror("can't create fifo");
    }
    pipename[strlen(pipename)-1] += 1;

  }
  //readfd = open(FIFO1, O_RDONLY);   //anoigeoume to reading akro
  //writefd = open(FIFO2, O_WRONLY);   //anoigoume to writing akro


  //fp = fopen(argv[i+1], "r");
  char buf[256];  //thewrw 256 maximum length mias grammhs poy anaferetai se katalogo sto docfile
  char tool[256];
  char ** cat_names = malloc(catalogs_num*sizeof(char*)); //onomata katalogwn
  i =0; //metraei directories
  while(fgets(buf, sizeof(buf), fp ) ){
    buf[strlen(buf)-1]= '\0';
    char * ji = buf+1;
    d = opendir(ji);
    if (d) {
      cat_names[i] = malloc(256*sizeof(char));
      strcpy(cat_names[i], ji);
      /*while ((dir = readdir(d)) != NULL) {
        strcpy(tool, dir->d_name);
        if(tool[0] == '.')
          continue; //de theloyme ta . kai ..

        printf("%s\n", dir->d_name);  //name of entry
      }*/
      closedir(d);
    }
    i++;
    //printf("%s ", buf);
  }
  //printf("AI = %d\n", i); to i exei arithmo katalogwn
  catalogs_num = i;

  server_j(numWorkers, catalogs_num, cat_names); //des to server.c gia leptomereies

  

  //close(readfd);
  //close(writefd);

  return 0;
}
