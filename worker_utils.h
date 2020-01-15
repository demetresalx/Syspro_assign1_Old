//gia thn ektypwsh tou trie prepei na xrhsimopoihsoume stoiva gia na mh xanoyme xarakthres


typedef struct search_list_node{
  int id; //document id
  int number_of_words;  //to |D| ston typo
  double score;
  struct search_list_node * next;
}search_list_node;


int update_search_list(search_list_node ** ,int , int , int);



int df_single_word(trie_node * , char * );
int tf_single_word(trie_node * , int ,char * );
void pro_search2(trie_node * , char * , int );
//int search_spec(trie_node * , char *  , char *, results_node * ); //METEFERA TH DHLWSH THS STO WORKERTRIE.H

void destroy_score_list(search_list_node ** );

void pro_search(trie_node * , char * , char *, log_data_node ** , unsigned int );

int find_max_number_of_times(trie_node * , char * , char **, int *);
int find_min_number_of_times(trie_node * , char * , char **, int *);
