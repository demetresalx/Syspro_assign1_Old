typedef struct doc_map_node {
  //int id;
  char * path;	//to path aytou tou textfile
  char * text;
}doc_map_node;



int doc_map_init(char * ,doc_map_node ** , int * );
void doc_map_destroy(doc_map_node ** , int );
