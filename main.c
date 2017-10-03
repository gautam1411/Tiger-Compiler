#include "globals.h"
//#include <conio.h>
#include "parse.c"
#include "semant.c"
#include "table.c"
#include "icodegen.c"
#include "tcodegen.c"


/* allocate global variables */
int lineno = 0;
char tiger_source_file[256] = {0, };
FILE * source;
FILE * listing;
FILE *scanlisting;
FILE *semlisting;
FILE * code;
FILE *tinycode;

/* allocate and set tracing flags */
#ifndef DEBUG
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

#else
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;
#endif 

int Error = FALSE;


int main(int argc, char * argv[])
{
  if(argc !=2){
    fprintf(stderr,"Usage : Enter name of tiger source file \n");
    exit(0);	 
  }else{
    printf("Compiling tiger source file : %s\n",argv[1]);	
  }
  TreeNode *x;
  clrscr();
  source=fopen(argv[1],"r");
  scanlisting = fopen("scanop.txt","w");
  code = fopen("code.txt","w");
  if(TraceScan == TRUE)
    listing = scanlisting;
  else
    listing = fopen("parserop.txt","w");
  
  x=parse();
 // printTree(x);
  if( x == NULL)
	printf("What the fuck !\n");
  SEM_transProg(x);
  if(Error==FALSE)
  {
      printf(" File : %s Line : %d \n", __FILE__, __LINE__);	
     genCode(x);
      printf(" File : %s Line : %d \n", __FILE__, __LINE__);	
      codeGen(x,"tcode");
      printf(" File : %s Line : %d \n", __FILE__, __LINE__);	
  }
  //fclose(tinycode);
  //fclose(source);
  //fclose(scanlisting);
  //fclose(listing);
  //getch();
  return 0;
}
