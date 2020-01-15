int worker_j(char * ,int );

typedef struct log_data_node{ //ayth h domh anaparista MIA GRAMMH logfile
  struct log_data_node * next;

  unsigned int q_timestamp;  //timestamp th stigmh poy diabasthke to query
  char * query_type;  //an einai search ktl
  char * query_object;  //enas apo tous orous tou search
  char * pathnames; //ena string poy prokyptei apo concatenation twn pathnames
}log_data_node;

int log_insert_command(log_data_node ** , unsigned int , char * , int );
int log_insert_pathname(log_data_node ** ,char * );
int log_insert_word(log_data_node ** , char * , unsigned int , char * );
