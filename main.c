#include "globals.h"
//#include <conio.h>
#include "parse.c"
#include "semant.c"
#include "table.c"
#include "icodegen.c"
#include "tcodegen.c"


/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE *scanlisting;
FILE *semlisting;
FILE * code;
FILE *tinycode;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;


int main()
{
  TreeNode *x;
  clrscr();
  source=fopen("max.tig","r");
  scanlisting = fopen("scanop.txt","w");
  
  code = fopen("code.txt","w");
  if(TraceScan == TRUE)
    listing = scanlisting;
  else
    listing = fopen("parserop.txt","w");
  
  x=parse();
  printTree(x);
  
  SEM_transProg(x);
  if(Error==FALSE)
    {
      genCode(x);
      codeGen(x,"tcode");
    }
  fclose(tinycode);
  fclose(source);
  fclose(scanlisting);
  fclose(listing);
  //getch();
  return 0;
}
