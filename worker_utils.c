#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/errno.h>
#include "doc_map.h"
#include "worker_trie.h"
#include "server.h"
#include "worker_client.h"
#include "worker_utils.h"
#define PERMS 0666
#define FIFO1 "./seitutsi/myfifo"




int df_single_word(trie_node * root, char * word)
{
  if(root == NULL)
    return 0;

  if(root->token == '\0') //riza
    return df_single_word(root->child, word);

  if(word[0] == root->token){  //brhkame gramma
    if(strlen(word) ==1)
      return posting_list_count(root);
    else{   //psaxnoyme ta epomena chars
      word++ ;
      df_single_word(root->child, word);
    }
  }
  else  //den to brhke
    df_single_word(root->next, word);
}



void pro_search(trie_node * root, char * sentence, char * pipename, log_data_node ** log, unsigned int timestamp)
{
  int i;
  results_node rage;
	rage.file_paths = NULL;
	rage.linfos = NULL;
  char comtool[400];
  int deadline =-1;
	//printf("anio mikre deadline = %d\n", deadline);
  char delimiter[]=" \t\r\n\v\f";
  strcpy(comtool, sentence );
	//puts(comtool);
  char * wstr = strtok(comtool, delimiter);	//na fygei to /search
  for(i=0;i<10;i++){
	  rage.perasmata = 0;
          wstr = strtok(NULL, delimiter);
	  if(wstr == NULL)
            break;
          if(strcmp(wstr, "-d")==0){
            wstr = strtok(NULL, delimiter);
            deadline = atoi(wstr);  //phrame deadline
	    continue;			//na mhn psaksei th leksh -d kai ton arithmo
          }
          
	  search_spec(root, wstr, pipename, &rage);
          if(rage.perasmata == 0){//psaxnei kai se periptwsh poy den to brei tha grapsei oti bre8hke 0 fores 
		int wfd = open(pipename, O_WRONLY | O_NONBLOCK);
		int timesf =0;
  		write(wfd, &timesf, sizeof(int));
		close(wfd);
	  }
	  else{	//kati brhkame
		//stelnw ta apotelesmata!
		//log_insert_word(log, wstr,  timestamp, "search" );	//GIA TO LOG
		int wfd = open(pipename, O_WRONLY | O_NONBLOCK);
		write(wfd, &(rage.perasmata), sizeof(int));
		for(int j=0; j< rage.perasmata; j++){
			/*tha grapsw ton arithmo grammwn, basikotath parametros*/
  			write(wfd, &(rage.linfos[j]->dore_kurai), sizeof(int));
			/*grafw tous arithmous grammwn*/
			for(int q=0; q< rage.linfos[j]->dore_kurai ;q++){
				write(wfd, &(rage.linfos[j]->line_numbers[q]), sizeof(int));
				/*pernaw mhkos grammhs poy akoly8ei*/
				int cmplsize = strlen(rage.linfos[j]->lines[q]) +1;
				write(wfd, &cmplsize, sizeof(int));
				/*pernaw th grammh*/
				write(wfd, rage.linfos[j]->lines[q], cmplsize);
			}
			/*prepei na perasw to monopati arxeiou, to megethos prwta k epeita to string*/
			//log_insert_pathname(log, rage.file_paths[j]);	//GIA TO LOG
			int sizestr = strlen(rage.file_paths[j]) +1;
			write(wfd, &sizestr, sizeof(int));
			write(wfd, rage.file_paths[j], sizestr);
			/*prepei na perasw to monopati arxeiou, to megetxto string*/
		
		}
		
		
		close(wfd);
	  } 
   }
	//teleiwse
	//kill(getppid(), SIGUSR1);
	free(rage.file_paths);
	free(rage.linfos);

}

/*H DHLWSH THS BRISKETAI STO WORKERTRIE.H*/
int search_spec(trie_node * root, char * word, char * pipename, results_node * rage)
{
  if(root == NULL)
    return 1;

  if(root->token == '\0') //riza
    return search_spec(root->child, word, pipename, rage);

  if(word[0] == root->token){  //brhkame gramma
    if(strlen(word) ==1){
      posting_list_node * curpt = root->post_list;
      while(curpt != NULL){
        //syllogh plhroforias
	rage->perasmata += 1;
        rage->file_paths = realloc(rage->file_paths, (rage->perasmata)*sizeof(char *));
        rage->file_paths[rage->perasmata -1] = curpt->file_path;
	rage->linfos = realloc(rage->linfos, (rage->perasmata)*sizeof(line_info *));
	rage->linfos[rage->perasmata -1] = curpt->linfo;

        curpt = curpt->next;
		
      }
      return 0;
    }
    else{   //psaxnoyme ta epomena chars
      word++ ;
      search_spec(root->child, word, pipename, rage);
    }
  }
  else  //den to brhke
    search_spec(root->next, word, pipename, rage);
}


int find_max_number_of_times(trie_node * root, char * word, char ** fpath, int * max)
{
  if(root == NULL)
    return 1;

  if(root->token == '\0') //riza
    return find_max_number_of_times(root->child, word, fpath, max);

  if(word[0] == root->token){  //brhkame gramma
    if(strlen(word) ==1){
      posting_list_node * curpt = root->post_list;
      while(curpt != NULL){
        //syllogh plhroforias
	if(curpt->times_encountered > *max){
		*max = curpt->times_encountered ;
		*fpath = curpt->file_path;
	}
	else if(curpt->times_encountered == *max){
		if(strcmp(*fpath, curpt->file_path) > 0)
			*fpath = curpt->file_path;
	}

        curpt = curpt->next;
		
      }
      return 0;
    }
    else{   //psaxnoyme ta epomena chars
      word++ ;
      find_max_number_of_times(root->child, word, fpath, max);
    }
  }
  else  //den to brhke
    find_max_number_of_times(root->next, word, fpath, max);
}


int find_min_number_of_times(trie_node * root, char * word, char ** fpath, int * min)
{
  if(root == NULL)
    return 1;

  if(root->token == '\0') //riza
    return find_min_number_of_times(root->child, word, fpath, min);

  if(word[0] == root->token){  //brhkame gramma
    if(strlen(word) ==1){
      posting_list_node * curpt = root->post_list;
	*min = curpt->times_encountered ;
	*fpath = curpt->file_path;
      while(curpt != NULL){
        //syllogh plhroforias
	
	if(curpt->times_encountered < *min){
		*min = curpt->times_encountered ;
		*fpath = curpt->file_path;
	}
	else if(curpt->times_encountered == *min){
		if(strcmp(*fpath, curpt->file_path) > 0)
			*fpath = curpt->file_path;
	}

        curpt = curpt->next;
		
      }
      return 0;
    }
    else{   //psaxnoyme ta epomena chars
      word++ ;
      find_min_number_of_times(root->child, word, fpath, min);
    }
  }
  else  //den to brhke
    find_min_number_of_times(root->next, word, fpath, min);
}





void destroy_score_list(search_list_node ** slist)
{
  if((*slist)->next != NULL)
    destroy_score_list(&((*slist)->next));

  free(*slist);
}
