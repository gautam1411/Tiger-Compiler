/*
 * util.c - commonly used utility functions.
 */
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
void *checked_malloc(int len)
{void *p = malloc(len);
 if (!p) {
    fprintf(stdout,"\nRan out of memory!\n");
    exit(1);
 }
 return p;
}

string String(char *s)
{string p = checked_malloc(strlen(s)+1);
 strcpy(p,s);
 return p;
}

void clrscr(){

  printf("\x1B[2J\x1B[H"); 
}


/*U_boolList U_BoolList(bool head, U_boolList tail)
{ U_boolList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
} */

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = checked_malloc(n);
  
  strcpy(t,s);
  return t;
}

int strcmpi(char *s1, char *s2)
{
  int i;
  for (i = 0; s1[i] && s2[i]; ++i)
    {
      /* If characters are same or inverting the 
	 6th bit makes them same */
      if (s1[i] == s2[i] || (s1[i] ^ 32) == s2[i])
	continue;
      else
	break;
    }
 
  /* Compare the last (or first mismatching in 
     case of not same) characters */
  if (s1[i] == s2[i])
    return 0;
 
  // Set the 6th bit in both, then compare
  if ((s1[i] | 32) < (s2[i] | 32)) 
    return -1;
  return 1;
}


void printToken( TokenType token, char* tokenString )
{ switch (token)
  {
	case IF:
    case THEN:
    case ELSE:
    case END:
    case ARRAY:
    case BREAK:
    case DO:
    case FOR:
    case FUNCTION:
    case IN:
    case LET:
    case NIL:
    case OF:
    case TO:
    case TYPE:
    case VAR:
    case WHILE:
					fprintf(listing,"reserved word: %s\n",tokenString);	break;
    case ASSIGN:	fprintf(listing,":=\n"); break;
    case LT: 		fprintf(listing,"<\n"); break;
    case EQ: 		fprintf(listing,"=\n"); break;
    case LPAREN:	fprintf(listing,"(\n"); break;
    case RPAREN:	fprintf(listing,")\n"); break;
    case SEMICOLON: fprintf(listing,";\n"); break;
    case PLUS: 		fprintf(listing,"+\n"); break;
    case MINUS: 	fprintf(listing,"-\n"); break;
    case TIMES: 	fprintf(listing,"*\n"); break;
    case DIVIDE: 	fprintf(listing,"/\n"); break;
    case COMMA: 	fprintf(listing,",\n"); break;
    case COLON: 	fprintf(listing,":\n"); break;
    case LBRACK: 	fprintf(listing,"[\n"); break;
    case RBRACK: 	fprintf(listing,"]\n"); break;
    case LBRACE: 	fprintf(listing,"{\n"); break;
    case RBRACE: 	fprintf(listing,"}\n"); break;
    case DOT: 		fprintf(listing,".\n"); break;
    case NEQ: 		fprintf(listing,"<>\n"); break;
    case LE: 		fprintf(listing,"<=\n"); break;
    case GE: 		fprintf(listing,">=\n"); break;
    case GT: 		fprintf(listing,">\n"); break;
    case AND: 		fprintf(listing,"&\n"); break;
    case OR: 		fprintf(listing,"|\n"); break;
    case ENDFILE: 	fprintf(listing,"EOF\n"); break;
    case INT:       fprintf(listing,"INT, val= %s\n",tokenString);	 break;
    case STRING:    fprintf(listing,"STRING, val= %s\n",tokenString); break;
    case ID:        fprintf(listing,"ID, name= %s\n",tokenString);    break;
    case ERROR:     fprintf(listing,"ERROR: %s\n",tokenString);      break;
    default:        fprintf(listing,"Unknown token: %d\n",token);
  }
}


/* Function newStmtNode creates a new
 * node for syntax tree construction
 */
TreeNode * newStmtNode(ExpKind exp)
{ TreeNode * t = (TreeNode *) checked_malloc(sizeof(TreeNode));
  int i;
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->expkind = exp;
    t->lineno = lineno;
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
 void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}


/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();

     switch (tree->expkind)
	 {
	  case IfK:
				fprintf(listing,"If\n");
				break;
	  case ForK:
				fprintf(listing,"For   %s\n",tree->attr.id.name);
				break;
	  case LetK:
				fprintf(listing,"Let\n");
				break;
	  case WhileK:
				fprintf(listing,"While\n");
				break;
	  case Str_constK:
				fprintf(listing,"Str_const    %s\n",tree->attr.id.name);
				break;
	  case IntK:
				fprintf(listing,"Int : %d\n",tree->attr.val);
				break;
	  case NilK:
				fprintf(listing,"Nil\n");
				break;
	  case Par_expK:
				fprintf(listing,"par_exp\n");
				break;
	  case Decl_listK:
				fprintf(listing,"Decl_list\n");
				break;
	  case Var_declK:
				if(tree!=NULL) //if there are any var declarations
				  fprintf(listing,"Var_decl   %s\n",tree->attr.id.name);
				break;
	  case Type_declK:
				if(tree!=NULL) //if there are any type declarations
				  fprintf(listing,"Type_decl\n");
				break;
	  case Func_declK:
				if(tree!=NULL) //if there are any function declarations
				  fprintf(listing,"Func_decl   %s\n",tree->attr.id.name);
				break;
	  case TypeidK:
				fprintf(listing,"Typeid   %s\n",tree->attr.id.name);
				//if(tree->exptype == ETYPE_INTEGER)
				       //	printf("   type: integer");
			       //	else if(tree->exptype == ETYPE_STRING)
				       //	printf("   type: string");
				
				break;
      case TypeK:
				fprintf(listing,"Type   %s\n",tree->attr.id.name);
				break;
	  case TypefieldK:
				fprintf(listing,"Typefield   %s\n",tree->attr.id.name);
				break;
	  case BreakK:
				fprintf(listing,"Break   %s\n",tree->attr.loopaddr);
				break;
	  case Comp_expK:
				fprintf(listing,"Comp_exp\n");
				break;
	  case Bin_expK:
				fprintf(listing,"Bin_exp\n");
				break;
	  case Bin1_expK:
				fprintf(listing,"Binary_exp\n");
				break;
	  case OpK:
				fprintf(listing,"Op: ");
				printToken(tree->attr.op,"\0");
				break;
	  case NegatedK:
				fprintf(listing,"Negated\n");
				break;

	case IdK:	switch(tree->attr.id.datatype)
				{
				 case ARRAYID:  fprintf(listing,"ARRAY:   %s\n",tree->attr.id.name);
								break;
				 case SIMPLEID: fprintf(listing,"ID:   %s\n",tree->attr.id.name);
								break;
				 case FUNCTIONCALL:
								fprintf(listing,"FUNCTION:   %s\n",tree->attr.id.name);
								break;
				 default: fprintf(listing,"Invalid datatype\n");
								break;
				}
	
	  break;
	case LvalueK:	switch(tree->attr.id.datatype)
				{
				 case ARRAYID:  fprintf(listing,"ARRAY:   %s\n",tree->attr.id.name);
								break;
				 case SIMPLEID: fprintf(listing,"ID:   %s\n",tree->attr.id.name);
								break;
				 case FUNCTIONCALL: 
								fprintf(listing,"FUNCTION:   %s\n",tree->attr.id.name);
								break;
				 default: fprintf(listing,"Invalid datatype\n");
								break;
				}
	  	  break;
	default:
				fprintf(stdout,"Unknown expression kind\n");
				break;
      
   } 
    
   for (i=0;i<MAXCHILDREN;i++)
	 printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}

