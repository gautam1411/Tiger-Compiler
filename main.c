#include "globals.h"
/*#include <conio.h>*/
/* parse.c transitively pulls in util.c and scan.c */
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
  TreeNode *x;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <tiger-source-file>\n", argv[0]);
    return 2;  /* 2 == usage error, by convention */
  }
  printf("Compiling tiger source file: %s\n", argv[1]);

  clrscr();
  source = fopen(argv[1], "r");
  if (source == NULL) {
    fprintf(stderr, "error: could not open source file '%s'\n", argv[1]);
    return 1;
  }
  scanlisting = fopen("scanop.txt", "w");
  code        = fopen("code.txt",   "w");
  if (TraceScan == TRUE)
    listing = scanlisting;
  else
    listing = fopen("parserop.txt", "w");

  x = parse();
  if (x == NULL) {
    fprintf(stderr, "error: parser returned a NULL syntax tree (check source for syntax errors)\n");
    return 1;
  }

  SEM_transProg(x);

  if (Error == FALSE) {
    genCode(x);
    codeGen(x, "tcode");
  } else {
    fprintf(stderr, "Compilation aborted: errors reported above.\n");
    return 1;
  }

  if (source)      fclose(source);
  if (scanlisting) fclose(scanlisting);
  if (code)        fclose(code);
  if (listing && listing != scanlisting) fclose(listing);
  return 0;
}
