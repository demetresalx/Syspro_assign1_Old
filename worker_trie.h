typedef struct line_info {
  int dore_kurai;	//se poses GRAMMES emfanisthke h leksh mesa sto keimeno
  int * line_numbers;	//pinakas me tous arithmous twn grammwn
  char ** lines;	//pinakas apo deiktes pros ta lines
}line_info;  //domh poy xrhsimopoieitai gia print


typedef struct results_node{
	int perasmata;	//auto tha bei ws orofh sth for tou goniou gia to read. arithmos arxeiwn poy bre8hke h leksh ousiastika
	char ** file_paths;	//ta paths twn arxeiwn poy bre8hke h leksh
	line_info ** linfos;	//mia seira apo tetoia antikeimena	
}results_node;



typedef struct token_list {
  char* array;
  int size;
}token_list;  //domh poy xrhsimopoieitai gia print

typedef struct posting_list_node{
	struct posting_list_node * next;	//deikths ston epomeno komvo o opoios afora to epomeno keimeno sto opoio bre8hke h leksh
	//int doc_id;	//to id tou keimenou sto opoio bre8hke h leksh
	char * file_path;	//to path tou arxeiou sto opoio bre8hke h leksh
	line_info * linfo;	//domh poy perigrafh th sxesh lekshs kai keimenoy. shmantiko gia th search
	int times_encountered;	//poses fores bre8hke sto keimeno auto
}posting_list_node;


typedef struct trie_node{
	posting_list_node * post_list;	//to posting list to fylloy, EAN exei.
	char token;	//to gramma poy afora ayto to fyllo
	struct trie_node * next;	//o epomenos komvos, sto IDIO epipedo
	struct trie_node * child; //to paidi autou tou komvou
}trie_node;


void inverted_index_init(trie_node ** );
void inverted_index_insert(trie_node ** , doc_map_node * , int , int * , int * );
int inverted_index_leaf_ins(trie_node ** , char *, char * , char *, int );
int posting_list_update(posting_list_node **, char *, char * , int );
int posting_list_count(trie_node * );

void destroy_posting_list(posting_list_node ** );
int destroy_trie(trie_node ** );

int print_trie(trie_node * , token_list * , int );
void user_print_trie(trie_node * );

void insert_at_level(token_list * , char , int );
void print_list(token_list * , int );

int line_info_update(line_info ** , char * , int );
int search_spec(trie_node * , char *  , char *, results_node * );
