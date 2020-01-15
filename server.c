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

extern int childDead;
int sflag =0;

static void catch_functiona(int signum) {
  if (signum == SIGUSR1){
      printf("Received SIGUSR1!\n");
      sflag += 1;
  }
}

int server_j(int numWorkers, int cats_num, char ** catl_names)
{
  char pipename[256];
  //signal(SIGUSR1, catch_functiona);
  char all_child_read_pipes[numWorkers][256];
   strcpy(pipename, FIFO1);
   strcat(pipename ,"a");
   char pipename2[256];
   strcpy(pipename2, pipename);
  int i, fd, fd2, pid, z, start, end;
  int child_pids[numWorkers];

  //ypologizw posa directories tha parei kathe worker


  for (i = 0; i < numWorkers; i++ ){
    //edw eixa mia sleep(1);
      pid = fork();
      if ( pid !=0 ){  //parent
        //puts("I AM PARENT");
        child_pids[i] = pid;

         //char testo[5];
         //strcpy(testo, "oya");
         //for(int j=0; j<numWorkers; j++){
          // if(readfd = open(pipename, O_RDONLY | O_NONBLOCK)<0)
              //perror("houston R:");
           //pipename[strlen(pipename)-1] += 1;

          // if(writefd = open(pipename, O_WRONLY )<0)
            //  perror("houston WR:");
          //  printf("tha grapsw sto %s\n", pipename2);
          fd = open(pipename2,O_WRONLY );
          strcpy(all_child_read_pipes[i], pipename2);
        /*  for(z = 0; z < cats_num; z++){
			         int size = strlen(catl_names[z])+1;
			            write(fd, &size, sizeof(int));
			               write(fd, catl_names[z], size);

		      }*/


          int w = numWorkers;
          if (cats_num < w){
    	       w = cats_num;
          }
          int bucket = cats_num / w;
 	        //for ( z = 0; z < w; z++) {
 	          start = i * bucket; //to i to xa z
 	          end = start + bucket - 1;
 	          if (i == w - 1){  //to i to xa z
 	            end = cats_num - 1;
 	          }
        //  }

 	          for (int zx = start; zx <= end; zx++){
 	            //printf("%s\n", catl_names[zx]);
              int size = strlen(catl_names[zx])+1;
                 write(fd, &size, sizeof(int));
                    write(fd, catl_names[zx], size);
 	          }
 	          //printf("\tsplit\n");


          //ena read ap to paidi
          /*strcpy(pipename, pipename2);
          pipename[strlen(pipename)-1] += 1;
          fd2 = open(pipename,O_RDONLY );
          char bufff[15];
          read(fd2, bufff, 14);
          puts(bufff);*/

           close(fd);
           pipename2[strlen(pipename2)-1] += 2; //aukshsh kata 2 gia na paei sto epomeno read pipe-gia to child
          // close(fd2);
        //  kill(child_pids[i] , SIGUSR1);
        // }
       }
       else{  //child
         int virtual_cats_num=0;
         char ch_pipename[256];
          strcpy(ch_pipename, FIFO1);
          strcat(ch_pipename ,"a");
          for(int j=0; j<i*2; j++)
            ch_pipename[strlen(ch_pipename)-1] += 1;  //pairnoyme swsto pipe onoma gia kathe worker
            /*ston i-osto worker antistoixoun ta 2i kai 2i+1 pipes*/


            int w = numWorkers;
            if (cats_num < w){
      	       w = cats_num;
            }
            int bucket = cats_num / w;
            //for ( z = 0; z < w; z++) {
   	          start = i * bucket; //to i to xa z
   	          end = start + bucket - 1;
   	          if (i == w - 1){  //to i to xa z
   	            end = cats_num - 1;
   	          }
              for (int zx = start; zx <= end; zx++){
   	            //printf("%s\n", catl_names[zx]);
                virtual_cats_num++;
   	          }
          //  }

         worker_j(ch_pipename, virtual_cats_num);
         break;
        //  return 0;
       }
     }
     /*while(1)
        if(sflag == numWorkers){
          sflag = 0;
          break;
        }*/

     sleep(1);
     //for(i=0; i<numWorkers; i++)
        //{waitpid(child_pids[i], NULL, WUNTRACED | WCONTINUED);}

     char command[400];
     char comtool[400];
     char pipename45[46];
     char delimiter[]=" \t\r\n\v\f\0";
     char * cmnd;

     if(childDead ==1)
      return 0;

     while(1)
     {
         puts("Please give a command : ");
         fgets(command, sizeof(command), stdin );
         strcpy(comtool, command);
         cmnd = strtok(comtool, delimiter);
         //puts(cmnd);
         if(strcmp(cmnd, "/exit")==0){
             //delete pipes kai enhmerwsh workers to stop
             for(i=0; i<numWorkers; i++){ //METABIBAZW THN ENTOLH STA PAIDIA NA KANOYN AUTO POY PREPEI
               fd = open(all_child_read_pipes[i], O_WRONLY | O_NONBLOCK);
               int size = strlen(command)+1;
               write(fd, &size, sizeof(int));
               write(fd, command, size);
               close(fd);
             }
             //for(i=0; i<numWorkers; i++)
                //kill(child_pids[i], SIGUSR1);

                int status;
              //for(i=0; i<numWorkers; i++)
                //waitpid(child_pids[i], &status, 0);

             delete_pipes(FIFO1, numWorkers);
             return 0;
         }
         else if(strcmp(cmnd, "/search")==0){
           for(i=0; i<numWorkers; i++){
             //printf("i will open %s\n", all_child_read_pipes[i]);
             fd = open(all_child_read_pipes[i], O_WRONLY | O_NONBLOCK);
             int size = strlen(command)+1;
             write(fd, &size, sizeof(int));
             write(fd, command, size);
             close(fd);
             //kill(child_pids[i], SIGUSR1);
             //while(1) //na perimenei ayto to paidi na teleiwsei
             //{
              //   printf("%d", sflag);
              //   if(sflag== 1){
              //   sflag =0;
              //   break;
               //}
             //}

             strcpy(pipename45, all_child_read_pipes[i]);
             pipename45[strlen(pipename45)-1] += 1;
             //printf("tha anouiksw to %s\n", pipename45);
             fd2 = open(pipename45,O_RDONLY );
             int lmit = wordtool(command);
             for(int xax =0; xax < lmit; xax++){
               read(fd2, &size, sizeof(int));
               //printf("exw na kanw tosa perasmata %d\n", size);
               int sztool, sztool2, sztool3;
               if(size != 0){

                 for(int zhz =0; zhz< size; zhz++){

                   read(fd2, &sztool, sizeof(int));
                   //printf("\tse toses grammes %d\n", sztool);
                   sztool3 = sztool;
                   int linenums[sztool];
                   char * thelines[sztool];
                   for(int yrd =0; yrd< sztool; yrd++){
                      read(fd2, &linenums[yrd], sizeof(int));
                      //printf("\tgrammh %d ",linenums[yrd] );
                      read(fd2, &sztool2, sizeof(int)); //mhkos grammhs poy akoloy8ei
                      thelines[yrd] = malloc(sztool2*sizeof(char));
                      read(fd2, thelines[yrd], sztool2);  //h grammh
                      //printf("\t%s \n",thelines[yrd] );
                   }
                   /*tha parw twra megethos path name*/
                   read(fd2, &sztool, sizeof(int));
                   //printf("se latreuw mwro mou\n %d", sztool);
                   char utilis[sztool];
                   read(fd2, utilis, sztool);
                   printf("Exists in file %s:\n", utilis);
                   for(int yrd =0; yrd< sztool3; yrd++){
                      printf("\tline %d :",linenums[yrd] );
                      printf("%s \n",thelines[yrd] );
                   }

                   // read(fd2, &size, sizeof(int));
                 }
                // if(sflag ==1)
                  //{sflag =0; break;}

             }
           }
             close(fd2);
           }
           //for(i=0; i<numWorkers; i++)

         }
         else if(strcmp(cmnd, "/maxcount")==0){
           int childrenmaxcount[numWorkers];
           char *maxcountfilenames[numWorkers];
           for(i=0; i<numWorkers; i++){
             fd = open(all_child_read_pipes[i], O_WRONLY | O_NONBLOCK);
             int size = strlen(command)+1;
             write(fd, &size, sizeof(int));
             write(fd, command, size);
             close(fd);
             strcpy(pipename45, all_child_read_pipes[i]);
             pipename45[strlen(pipename45)-1] += 1;
             fd2 = open(pipename45,O_RDONLY );
             read(fd2, &childrenmaxcount[i], sizeof(int));
             if(childrenmaxcount[i] ==0)
                 maxcountfilenames[i] = NULL;
             else{  //brhkame
               read(fd2, &size, sizeof(int)); //pairnei megethos filepath
               maxcountfilenames[i] = malloc(size*sizeof(char));
               read(fd2, maxcountfilenames[i], size);
               //printf("max count for this kid is %d in %s\n", childrenmaxcount[i], utilis);
             }
             close(fd2);
           }
           int truemax=0;
           //int placej=0;
           char * truepath = NULL;
           for(i=0; i<numWorkers; i++){
             if(childrenmaxcount[i] > truemax)
               {truemax = childrenmaxcount[i]; truepath = maxcountfilenames[i]; }
             else if(childrenmaxcount[i] == truemax){
               if(strcmp(truepath, maxcountfilenames[i]) > 0)
			            truepath = maxcountfilenames[i];
             }
           }
            if(truemax == 0)
              printf("This keyword was not found...\n");
            else{
              printf("File %s contains this keyword the MOST times (%d)\n", truepath, truemax);
           }
           for(i=0; i<numWorkers; i++)
              free(maxcountfilenames[i]);
         }
         else if(strcmp(cmnd, "/mincount")==0){
           int childrenmincount[numWorkers];
           char *mincountfilenames[numWorkers];
           for(i=0; i<numWorkers; i++){
             fd = open(all_child_read_pipes[i], O_WRONLY | O_NONBLOCK);
             int size = strlen(command)+1;
             write(fd, &size, sizeof(int));
             write(fd, command, size);
             close(fd);
             strcpy(pipename45, all_child_read_pipes[i]);
             pipename45[strlen(pipename45)-1] += 1;
             fd2 = open(pipename45,O_RDONLY );
             read(fd2, &childrenmincount[i], sizeof(int));
             if(childrenmincount[i] ==0)
                 mincountfilenames[i] = NULL;
             else{  //brhkame
               read(fd2, &size, sizeof(int)); //pairnei megethos filepath
               mincountfilenames[i] = malloc(size*sizeof(char));
               read(fd2, mincountfilenames[i], size);
               //printf("max count for this kid is %d in %s\n", childrenmaxcount[i], utilis);
             }
             close(fd2);
           }
           int truemin =0;
           char * truepath = NULL;
           for(i=0; i<numWorkers; i++)
              if(childrenmincount[i] != 0)
                {truemin = childrenmincount[i]; truepath=mincountfilenames[i]; break;}
           //twra to truemin mporei na exei mia mh mhdenikh timh wste na sygkrinw
           for(i=0; i<numWorkers; i++){
             if((childrenmincount[i] < truemin) && (childrenmincount[i] != 0))
               {truemin = childrenmincount[i]; truepath = mincountfilenames[i]; }
             else if(childrenmincount[i] == truemin){
               if(strcmp(truepath, mincountfilenames[i]) > 0)
			            truepath = mincountfilenames[i];
             }
           }
            if(truemin == 0)
              printf("This keyword was not found...\n");
            else{
              printf("File %s contains this keyword the LEAST times (%d)\n", truepath, truemin);
           }
           for(i=0; i<numWorkers; i++)
              free(mincountfilenames[i]);
         }
         else if(strcmp(cmnd, "/wc")==0){
           int sumwords =0; int sumbytes =0; int sumlines =0;
           int adder;
            for(i=0; i<numWorkers; i++){
              fd = open(all_child_read_pipes[i], O_WRONLY | O_NONBLOCK);
              int size = strlen(command)+1;
              write(fd, &size, sizeof(int));
              write(fd, command, size);
              close(fd);

                strcpy(pipename45, all_child_read_pipes[i]);
                pipename45[strlen(pipename45)-1] += 1;
                fd2 = open(pipename45,O_RDONLY );
                read(fd2, &adder, sizeof(int));
                //printf("toso %d\n", adder);
                sumbytes += adder;
                read(fd2, &adder, sizeof(int));
                //printf("toso %d\n", adder);
                sumwords += adder;
                read(fd2, &adder, sizeof(int));
                //printf("toso %d\n", adder);
                sumlines += adder;
                close(fd2);
            }
            printf("Texts total of %d bytes, %d words, %d lines\n", sumbytes, sumwords, sumlines);
         }
      }

}

int delete_pipes(char * name_base, int numWorkers )
{
  int j;
  char pipename[256];
  //diagrafh pipes
  strcpy(pipename, name_base);
  strcat(pipename ,"a");
  for(int j=0; j< 2*numWorkers; j++){
    if ( unlink(pipename) < 0) {
      perror("client: can't unlink \n");
      ;
    }
    pipename[strlen(pipename)-1] += 1;

  }
}


int wordtool(char * command)
{
  char comtool[400];
  char delimiter[]=" \t\r\n\v\f";
  strcpy(comtool, command );
  int anio =0;
  int deadline;
	//puts(comtool);
  char * wstr = strtok(comtool, delimiter);	//na fygei to /search
  for(int i=0;i<10;i++){
        wstr = strtok(NULL, delimiter);
        anio++ ;
	  if(wstr == NULL)
            {anio--;break;}
          if(strcmp(wstr, "-d")==0){
            wstr = strtok(NULL, delimiter);
            deadline = atoi(wstr);  //phrame deadline
	           continue;			//na mhn psaksei th leksh -d kai ton arithmo
          }
  }
  return anio;
}
