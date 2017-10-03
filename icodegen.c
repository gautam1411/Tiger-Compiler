/*
  icodegen.c intermediate code
*/
#include "globals.h"

static int tloc=1;
static int lloc=1;


char * getOp(TokenType tok)
{
  static char op[10];
  switch(tok)
    {
    case ASSIGN:  strcpy(op,"=");  return op;
    case LT: 	  strcpy(op,"<");  return op;
    case EQ: 	  strcpy(op,"=="); return op; 
    case PLUS: 	  strcpy(op,"+");  return op; 
    case MINUS:   strcpy(op,"-");  return op; 
    case TIMES:   strcpy(op,"*");  return op;
    case DIVIDE:  strcpy(op,"/");  return op;
    case NEQ: 	  strcpy(op,"!="); return op; 
    case LE: 	  strcpy(op,"<="); return op;
    case GE: 	  strcpy(op,">="); return op;
    case GT: 	  strcpy(op,">");  return op;
    case AND: 	  strcpy(op,"&&"); return op; 
    case OR: 	  strcpy(op,"||"); return op; 
    default: printf("default");    return NULL;
    }	
}

int genCode(TreeNode *tree)
{
  int temp1,temp2;
  char *s;
  TreeNode *t;
  switch (tree->expkind)
    {
    case IfK:
      temp1=tloc;
      temp2=lloc;
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      genCode(tree->child[0]); //exp1
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"if_false t%d goto L%d\n",temp1,lloc++);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      tloc=genCode(tree->child[1]); //exp2
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"goto L%d\n",temp2+1);
      fprintf(code,"L%d:\n",temp2);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      tloc=genCode(tree->child[2]); //exp3
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"L%d:\n",lloc++);
      break;
    case ForK:
      //fprintf(code,"For   %s\n",tree->attr.id.name);
      break;
    case LetK:
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      genCode(tree->child[0]); //decl_list
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"in:\n",temp1+1);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);    
      for(t=tree->child[1];t;t=t->sibling)  //expr_seq
	genCode(t);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      break;
    case WhileK:
      //fprintf(code,"While\n");
      temp2=lloc;
      fprintf(code,"L%d:\n",lloc++);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      temp1=genCode(tree->child[0]); //exp1
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"if_false t%d goto L%d\n",temp1,temp2+1);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      genCode(tree->child[1]); //exp2
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"goto L%d\n",temp2);
      fprintf(code,"L%d:\n",temp2+1);
      break;
    case Str_constK:
      //fprintf(code,"Str_const    %s\n",tree->attr.id.name);
      break;
    case IntK:
      fprintf(code,"t%d=%d\n",tloc++,tree->attr.val);
      break;
    case NilK:
      //fprintf(code,"Nil\n");
      break;
    case Par_expK:
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      genCode(tree->child[0]);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      //fprintf(code,"par_exp\n");
      break;
    case Decl_listK:
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      // printf("Adding a defence code :  File  %s Line : %d\n", __FILE__, __LINE__);
      //if(tree && tree->child[2] );
      printf(" Debug code  \n");
      if(tree == NULL)
	printf("Tree is NULL \n");
      if(tree->child[0] == NULL)
	printf("File : %s Line : %d\n", __FILE__, __LINE__);
      if(tree->child[1] == NULL)
	printf("File : %s Line : %d\n", __FILE__, __LINE__);
      if(tree->child[2] == NULL)
	printf("File : %s Line : %d\n", __FILE__, __LINE__);
      if(tree->sibling == NULL)
	printf("File : %s Line : %d\n", __FILE__, __LINE__);
      // if(tree->sibling == NULL)
      printf("File : %s Line : %d   Tiger line  %d \n", __FILE__, __LINE__, tree->lineno);
      printf("File : %s Line : %d   Tiger line  %d \n", __FILE__, __LINE__, tree->expkind);
      printf("File : %s Line : %d   Tiger line  %d \n", __FILE__, __LINE__, tree->type);
      printf("File : %s Line : %d   Tiger line  %d \n", __FILE__, __LINE__, tree->prm);
      printf("Defence code Adedd here \n");
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      if(tree && tree->child[2])
	genCode(tree->child[2]); //func_decl
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      break;
    case Var_declK:
      //if(tree!=NULL) //if there are any var declarations
      //fprintf(code,"Var_decl   %s\n",tree->attr.id.name);
      break;
    case Type_declK:
      //if(tree!=NULL) //if there are any type declarations
      //fprintf(code,"Type_decl\n");
      break;
    case Func_declK:
      //if(tree!=NULL) //if there are any function declarations ,only 1 possible for now
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      fprintf(code,"entry	:%s\n",tree->attr.id.name);
      //fprintf(code,"BeginFunc ;\n");
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      genCode(tree->child[2]); //exp
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      if(tree->child[1] != NULL)
	fprintf(code,"return t%d;\n",tloc-1);
      fprintf(code,"exit	:%s;\n",tree->attr.id.name);
      break;
    case TypeidK:
      //fprintf(code,"Typeid   %s\n",tree->attr.id.name);
      //if(tree->exptype == ETYPE_INTEGER)
      //	printf("   type: integer");
      //	else if(tree->exptype == ETYPE_STRING)
      //	printf("   type: string");  
      break;
    case TypeK:
      //fprintf(code,"Type   %s\n",tree->attr.id.name);
      break;
    case TypefieldK:
      //fprintf(code,"Typefield   %s\n",tree->attr.id.name);
      break;
    case BreakK:
      //fprintf(code,"Break   %s\n",tree->attr.loopaddr);
      break;
    case Comp_expK:
      //fprintf(code,"Comp_exp\n");
      break;
    case Bin_expK:
      //fprintf(code,"Bin_exp\n");
      break;
    case Bin1_expK:
      //fprintf(code,"Binary_exp\n");
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      genCode(tree->child[0]);
      printf("File : %s Line : %d\n", __FILE__, __LINE__);
      break;
    case OpK:
      //s=getOp(tree->attr.op);
      //printf("%s",s);
      if(tree->attr.op ==ASSIGN)
	{
	  printf("File : %s Line : %d\n", __FILE__, __LINE__);
	  genCode(tree->child[1]);
	  printf("File : %s Line : %d\n", __FILE__, __LINE__);
	  fprintf(code,"%s=t%d\n",tree->child[0]->attr.id.name,tloc-1);
	}
      //if(tree->attr.op == EQ ||tree->attr.op ==LT||tree->attr.op ==GT||tree->attr.op ==LE||tree->attr.op ==GE||tree->attr.op ==NEQ||tree->attr.op ==AND||tree->attr.op ==OR)
      else
	{
	  if(tree->child[0]->child[0]==NULL && tree->child[1]->child[0]==NULL)
	    {
	      if( tree->child[0]->attr.id.datatype==FUNCTIONCALL)
		{
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  temp1=genCode(tree->child[0]);
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  //fprintf(code,"t%d = t%d;\n",tloc++,temp1);
		  fprintf(code,"t%d = %s%st%d;\n",tloc++,temp1,getOp(tree->attr.op),tree->child[1]->attr.id.name);
		  break;
		}
	      else if( tree->child[1]->attr.id.datatype==FUNCTIONCALL)
		{
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  temp1=genCode(tree->child[1]);
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  //fprintf(code,"t%d = t%d;\n",tloc++,temp1);
		  fprintf(code,"t%d = %s%st%d;\n",tloc++,tree->child[1]->attr.id.name,getOp(tree->attr.op),temp1);
		  break;
		  
		}
	      else
		{
		  fprintf(code,"t%d = %s%s%s;\n",tloc++,tree->child[0]->attr.id.name,getOp(tree->attr.op),(tree->child[1])->attr.id.name);
		}
	    }
	  else if(tree->child[0]->child[0]==NULL)
	    {
	      printf("File : %s Line : %d\n", __FILE__, __LINE__);
	      temp1=genCode(tree->child[1]);
	      printf("File : %s Line : %d\n", __FILE__, __LINE__);
	      //fprintf(code,"t%d = t%d;\n",tloc++,temp1);
	      fprintf(code,"t%d = %s%st%d;\n",tloc++,tree->child[0]->attr.id.name,getOp(tree->attr.op),temp1);
	      break;
	    }
	  else if(tree->child[1]->child[0]==NULL)
	    {
	      printf("File : %s Line : %d\n", __FILE__, __LINE__);
	      temp1=genCode(tree->child[0]);
	      printf("File : %s Line : %d\n", __FILE__, __LINE__);
	      //fprintf(code,"t%d = t%d;\n",tloc++,temp1);
	      fprintf(code,"t%d = %s%st%d;\n",tloc++,temp1,getOp(tree->attr.op),tree->child[1]->attr.id.name);
	      break;
	    }	  
	}
      //fprintf(code,"t%d = %s%s%s;\n",tloc,tree->child[0]->attr.id.name,s,(tree->child[1])->attr.id.name);			
      break;
    case NegatedK:
      //fprintf(code,"Negated\n");
      break;
    case IdK:	
      switch(tree->attr.id.datatype)
	{
	case ARRAYID:  fprintf(code,"t%d=%s\n",tloc++,tree->attr.id.name);
	  break;
	case SIMPLEID: fprintf(code,"t%d=%s\n",tloc++,tree->attr.id.name);
	  break;
	case FUNCTIONCALL:
	  fprintf(code,"begin_args\n");
	  for(t=tree->child[0];t;t=t->sibling)
	    {
	      if(t->child[0]==NULL)
		{
		  fprintf(code,"arg  %s\n",t->attr.id.name);
		}
	      else
		{
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  temp1=genCode(t);
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  fprintf(code,"arg  t%d\n",temp1);
		}
	    }
	  fprintf(code,"call %s\n",tree->attr.id.name);
	  break;
	  //fprintf(code,"PopParams 4;\n");
	default: 
	  //fprintf(code,"Invalid datatype\n");
	  break;
	}
      break;
    case LvalueK:	
      switch(tree->attr.id.datatype)
	{
	case ARRAYID:   
	  fprintf(code,"t%d=%s\n",tloc++,tree->attr.id.name);
	  break;
	case SIMPLEID: 
	  fprintf(code,"t%d=%s\n",tloc++,tree->attr.id.name);
	  break;
	case FUNCTIONCALL: 
	  fprintf(code,"begin_args\n");
	  for(t=tree->child[0];t;t=t->sibling)
	    {
	      if(t->child[0]==NULL)
		{
		  fprintf(code,"arg  %s\n",t->attr.id.name);
		}
	      else
		{
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  temp1=genCode(t);
		  printf("File : %s Line : %d\n", __FILE__, __LINE__);
		  fprintf(code,"arg  t%d\n",temp1);
		}
	    }
	  fprintf(code,"call %s\n",tree->attr.id.name);
	  break;
	default: 
	  //fprintf(code,"Invalid datatype\n");
	  break;
	}
      break;
    default:
      //fprintf(stdout,"Unknown expression kind\n");
      break; 
    }
  return tloc-1;
}
