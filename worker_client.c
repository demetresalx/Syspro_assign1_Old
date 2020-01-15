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
#include <signal.h>
#include <poll.h>
#include "doc_map.h"
#include "worker_trie.h"
#include "server.h"
#include "worker_client.h"
#include "worker_utils.h"
#include <time.h>
#define PERMS 0666
#define FIFO1 "./seitutsi/myfifo"

int flag =0;
int childDead = 0;

static void catch_function(int signum) {
  if (signum == SIGUSR1){
      printf("Received SIGUSR1!\n");
      flag = 1;
  }
}

int worker_j(char * pipename, int cats_num)
{
  signal(SIGUSR1, catch_function);  //to signal poy tha parei ap ton patera
  DIR *d;  //gia directory browsing
  struct dirent *dir; //gia directory browsing
  log_data_node * log_data= NULL; //gia ta logs sto telos
  int total_words =0; //gia th wc
  int total_bytes =0; //gia th wc
  int total_lines =0; //gia th wc
  trie_node * root= NULL; // to trie gia tis lekseis
  doc_map_node * doc_map; //boh8htika ergaleia
	int doc_map_size, levels;  //boh8htika ergaleia
  unsigned int timestamp;
  //gia to log
  struct stat st = {0};
  if (stat("./log", &st) == -1) {
    mkdir("./log", PERMS);
  }

  inverted_index_init(&root);
  char tool[256];
  char tool2[256];
  char *got[cats_num];
  char pipename2[256];
  strcpy(pipename2, pipename);
  pipename2[strlen(pipename2)-1] +=1;   //to 2o pipe, tha grapsoyme s auto
  int rfd, wfd, retval, i;
  struct pollfd fds[1]; //gia thn poll
  //printf( "[son] pid %d from pid %d\n", getpid(), getppid() );
  //printf("tha anoiksw to : %s\n", pipename);
  //printf( "[son] pid %d anoigw ena pipe\n", getpid());
  rfd = open(pipename, O_RDONLY );
  //printf( "[son] pid %d anoiksa ena pipe\n", getpid());
  fds[0].fd = rfd;
  fds[0].events = 0;
  fds[0].events |= POLLIN ;
  int timeout = 5000;
  //int pret = poll(fds, wfd, timeout);
  //printf("TOSO %d\n", fd);
  for(i = 0; i<cats_num;i++){
		int size;
		read(rfd, &size, sizeof(int));
		//printf("size = %d\n", size);
		got[i] = malloc(size);
		read(rfd, got[i], size);
		//read(fd, buffer, sizeof(buffer));

		//printf("%s\n", got[i]);
		//write(1,buffer,sizeof(buffer));
	}
  for (i=0; i<cats_num; i++){
		//printf("child got: %s\n", got[i]);
    d = opendir(got[i]);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        strcpy(tool, dir->d_name);
        if(tool[0] == '.')
          continue; //de theloyme ta . kai ..

        printf("\t%s\n", dir->d_name);  //name of entry
        strcpy(tool2, "./\0");
        strcat(tool2,got[i]);
        strcat(tool2, "/\0");
        strcat(tool2, dir->d_name); //etsi exw parei to swsto path gia open
        doc_map_init(tool2, &doc_map, &doc_map_size);
        total_lines += doc_map_size;
        inverted_index_insert(&root, doc_map, doc_map_size, &total_bytes, &total_words);
      }
      closedir(d);
    }
	}
  //user_print_trie(root);
  //tha grapsw kati sto gonio
  //char * test1 = "i am your son";
//  wfd = open(pipename2, O_WRONLY | O_NONBLOCK);
  //write(wfd, test1, strlen(test1)+1);

  //close(wfd);
	close(rfd);

  //kill(getppid(), SIGUSR1); // resume the father
  //puts("I DID IT");
  char comtool[400];
  char delimiter[]=" \t\r\n\v\f";
  while(1){
    //if(flag ==1){
      //puts("chilai");
      rfd = open(pipename, O_RDONLY);
      int size;
  		read(rfd, &size, sizeof(int));
      read(rfd, tool2, size);
      //printf("I am [son] pid %d from pid %d\n",getpid(), getppid());
      //puts(tool2);
      strcpy(comtool, tool2);
      char * wstr = strtok(comtool, delimiter); //apospw 1h leksh = entolh
      if(strcmp(wstr, "/search")==0){
        //timestamp = (unsigned)time(NULL) ;//xronos th stigmh poy phrame entolh
        pro_search(root, tool2, pipename2, &log_data, 0);

        //kill(getppid(), SIGUSR1);
      }
      else if(strcmp(wstr, "/maxcount")==0){
        wstr = strtok(NULL, delimiter);	//na fygei to /maxcount kai na paroyme keyword
        int tmes =0;
        char * thpath = NULL;
        find_max_number_of_times(root, wstr, &thpath, &tmes);
        wfd = open(pipename2, O_WRONLY | O_NONBLOCK);
        write(wfd, &tmes, sizeof(int)); //grafei arithmo prwta
        if(tmes !=0){
          /*grapse kai to path tou arxeiou*/
          int sizestr = strlen(thpath) +1;
          write(wfd, &sizestr, sizeof(int));
          write(wfd, thpath, sizestr);
        }
        close(wfd);
      }
      else if(strcmp(wstr, "/mincount")==0){
        wstr = strtok(NULL, delimiter);	//na fygei to /maxcount kai na paroyme keyword
        int tmes =0;
        char * thpath = NULL;
        find_min_number_of_times(root, wstr, &thpath, &tmes);
        wfd = open(pipename2, O_WRONLY | O_NONBLOCK);
        write(wfd, &tmes, sizeof(int)); //grafei arithmo prwta
        if(tmes !=0){
          /*grapse kai to path tou arxeiou*/
          int sizestr = strlen(thpath) +1;
          write(wfd, &sizestr, sizeof(int));
          write(wfd, thpath, sizestr);
        }
        close(wfd);
      }
      else if(strcmp(wstr, "/wc")==0){
        wfd = open(pipename2, O_WRONLY | O_NONBLOCK);
        write(wfd, &total_bytes, sizeof(int)); //grafei arithmo bytes prwta
        write(wfd, &total_words, sizeof(int)); //grafei arithmo leksewn
        write(wfd, &total_lines, sizeof(int)); //grafei arithmo grammwn
        close(wfd);
      }
      else if(strcmp(wstr, "/exit")==0){
        printf("Child terminating!\n");
        close(rfd);
        wfd = open(pipename2, O_WRONLY | O_NONBLOCK);
        write(wfd, &total_words, sizeof(int)); //grafei arithmo leksewn
        close(wfd);
        doc_map_destroy(&doc_map, doc_map_size);
        //destroy_trie(&root);
        childDead =1;
        return 0;
      }

      close(rfd);
      //puts("chilai2");
    //  flag = 0;
  //  }

  }

  //destroy_trie(&root);
	return 0;
}

int log_insert_command(log_data_node ** lg, unsigned int timestamp, char * qry, int level)
{
  if((*lg) == NULL){
    *lg = malloc(1*sizeof(log_data_node));
    (*lg)->q_timestamp = timestamp;
    (*lg)->query_type = malloc((strlen(qry)+1)*sizeof(char));
    strcpy((*lg)->query_type, qry);
    (*lg)->query_object = NULL;
    (*lg)->pathnames = NULL;
    (*lg)->next = NULL;
    return level;
  }
  else{ //sto telos na paei
    log_insert_command(&((*lg)->next), timestamp, qry, level+1);
  }
}


int log_insert_pathname(log_data_node ** lg, char * pathname)
{
  log_data_node * curpt = *lg;
  while(curpt->next != NULL)
    curpt = curpt->next;

  if(curpt->pathnames == NULL){
    curpt->pathnames = malloc((strlen(pathname)+1)*sizeof(char));
    strcpy(curpt->pathnames, pathname);
  }
  else{ //exoyme pathname
    int newsize = strlen(curpt->pathnames) +1 + 4;
    curpt->pathnames = realloc(curpt->pathnames, newsize*sizeof(char));
    strcat(curpt->pathnames, " : \0");
  }
}

int log_insert_word(log_data_node ** lg, char * word, unsigned int timestamp, char * type )
{
  log_data_node * curpt = *lg;
  while(curpt != NULL)
    curpt = curpt->next;

  curpt = malloc(1*sizeof(log_data_node));
  curpt->q_timestamp = timestamp;
  curpt->query_object = malloc((strlen(word)+1)*sizeof(char));
  strcpy(curpt->query_object, word);
  curpt->query_type = malloc((strlen(type)+1)*sizeof(char));
  strcpy(curpt->query_type, type);
  curpt->pathnames = NULL;
  curpt->next = NULL;

  return 0;

}
