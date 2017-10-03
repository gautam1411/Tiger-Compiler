/*   
     File: parse.c  The parser implementation for the TIGER  compiler  
     Authors : Gautam ,Reuben                             
*/

#include "globals.h"
#include "util.c"
#include "scan.c"
#include "parse.h"

extern TraceScan;
static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * Let_exp(void);
static TreeNode * If_exp(void);
static TreeNode * For_exp(void);
static TreeNode * While_exp(void);
static TreeNode * Id_exp(void);
static TreeNode * Str_const(void);
static TreeNode * Int_exp(void);
static TreeNode * Nil_exp(void);
static TreeNode * Par_exp(void);
static TreeNode * Expr_seq(void);
static TreeNode * Decl_list(void);
static TreeNode * Typeid(void);
static TreeNode * Type(void);
static TreeNode * Typefields(void);
TreeNode * expr(void);
static TreeNode * exp4break(TreeNode *p);
static TreeNode * Comp_exp(void);
static TreeNode * Bin_exp(void);
static TreeNode * Bin1_exp(void);
static TreeNode * Simp_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * Paran_exp(void);
static TreeNode * Exp_list(void);
static TreeNode * Lvalue_exp(void);

#define MAXTYPES 100
static int typecount=0;
struct Types{
  char *typeid;
  ExpType type;
  Datatype datatype;
}types[MAXTYPES];


void syntaxError(char * message){ 
  fprintf(stdout,"\n>>> ");
  fprintf(stdout,"Syntax error at line %d: %s",lineno,message);
  Error=TRUE;
  //exit(0);
}

void match(TokenType expected){
  /*
  int old_line;
  TokenType temp1;
  */

  if (token == expected){
    token = getToken();
    printf ("FIle : %s  line :  %d.\n",__FILE__, __LINE__); 
  }
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(stdout,"      ");
    exit(0);
    /*
      old_line=lineno;
      while(lineno==old_line)
      temp1=getToken();
      token = getToken(); */
  }
}


TreeNode * Let_exp(void){ 
  TreeNode * t = newStmtNode(LetK),*temp = NULL;
  match(LET);
  printf("Returned from match  at line : %d \n", __LINE__);
  if(t!=NULL)  
    t->child[0]=Decl_list();
  match(IN);
 printf("Returned from match  at line : %d \n", __LINE__);
  if(t!=NULL &&token!=END)
    t->child[1]=Expr_seq();
  temp = t->child[1];
  while(temp->sibling != NULL)
    temp = temp->sibling;
  t->type = temp->type;
  match(END);
 printf("Returned from match  at line : %d \n", __LINE__);
  return t;
}

TreeNode * If_exp(void){ 

  TreeNode * t = newStmtNode(IfK);
  match(IF);
 printf("Returned from match  at line : %d \n", __LINE__);
  if (t!=NULL) 
    t->child[0] = expr();
 printf("Returned from  line : %d \n", __LINE__);
  match(THEN);
 printf("Returned from match  at line : %d \n", __LINE__);
  if (t!=NULL) 
    t->child[1] = expr();
 printf("Returned from  line : %d \n", __LINE__);
  t->type = (t->child[1])->type;
  //t->attr.id.datatype = (t->child[1])->attr.id.datatype; 	TODO
  if (token==ELSE) {
    match(ELSE);
 printf("Returned from match  at line : %d \n", __LINE__);
    if (t!=NULL) 
      t->child[2] = expr();
 printf("Returned from  line : %d \n", __LINE__);
  }
  return t;
}

TreeNode * For_exp(void){ 

  TreeNode * t = newStmtNode(ForK);
  match(FOR);
 printf("Returned from match  at line : %d \n", __LINE__);
  if ((t!=NULL) && (token==ID)){
    t->attr.id.name = copyString(tokenString);
    t->attr.id.datatype=SIMPLEID;
  }
  match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
  match(ASSIGN);
 printf("Returned from match  at line : %d \n", __LINE__);
  if (t!=NULL) 
    t->child[0] = expr();
 printf("Returned from  line : %d \n", __LINE__);
  match(TO);
 printf("Returned from match  at line : %d \n", __LINE__);
  if (t!=NULL) 
    t->child[1] = expr();
 printf("Returned from  line : %d \n", __LINE__);
  match(DO);
 printf("Returned from match  at line : %d \n", __LINE__);
  if(t!=NULL)  
    t->child[2] = exp4break(t);
  t->type = (t->child[2])->type;
  return t;
}


TreeNode * While_exp(void){ 

  TreeNode * t = newStmtNode(WhileK);
  match(WHILE);
 printf("Returned from match  at line : %d \n", __LINE__);
  if (t!=NULL) 
    t->child[0] = expr();
 printf("Returned from  line : %d \n", __LINE__);
  match(DO);
 printf("Returned from match  at line : %d \n", __LINE__);
  if (t!=NULL) 
    t->child[1] = exp4break(t);
  t->type = (t->child[1])->type;
  return t;
}


TreeNode * Id_exp(void){ 

  TreeNode * t = NULL,*p=NULL;
  TokenType temp1=lookahead(1,0);
	
  if(temp1==LBRACK){
    
    while(temp1!=RBRACK)
      temp1=lookahead(1,1);
    temp1=lookahead(1,1);
    if(temp1==PLUS||temp1==MINUS||temp1==TIMES||temp1==DIVIDE||temp1==LT||temp1==LE||temp1==GT||temp1==GE||temp1==EQ||temp1==NEQ){     
      t=Comp_exp();
      return t;
    }
  }
  t=newStmtNode(IdK);
  if ((t!=NULL) && (token==ID)){
    t->attr.id.name = copyString(tokenString);
    t->attr.id.datatype=SIMPLEID;
  }
  match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
  switch(token){
  case LBRACK: t->attr.id.datatype=ARRAYID;
    match(LBRACK);
 printf("Returned from match  at line : %d \n", __LINE__);
    if (t!=NULL)    t->child[0]=expr();
 printf("Returned from  line : %d \n", __LINE__);
    match(RBRACK);
 printf("Returned from match  at line : %d \n", __LINE__);
    if(token==OF)
      {
	match(OF);
 printf("Returned from match  at line : %d \n", __LINE__);
	t->child[1]=expr();
 printf("Returned from  line : %d \n", __LINE__);
      }
    else if(token==ASSIGN)
      {
	p=newStmtNode(OpK);
	p->attr.op=token;
	p->child[0]=t;
	t=p;
	match(ASSIGN);
 printf("Returned from match  at line : %d \n", __LINE__);
	t->child[1]=expr();
 printf("Returned from  line : %d \n", __LINE__);
      }
    
	       break;
  case  LPAREN : t->attr.id.datatype=FUNCTIONCALL;
    match(LPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
    if(t!=NULL&&token!=RPAREN)  t->child[0]=Exp_list();
    match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
    break;
  case  ASSIGN :  
    p=newStmtNode(OpK);
    p->attr.op=token;
    p->child[0]=t;
    t=p;
    match(ASSIGN);
 printf("Returned from match  at line : %d \n", __LINE__);
    if (t!=NULL) t->child[1] = expr();
 printf("Returned from  line : %d \n", __LINE__);
    break;
    
  default:  return t;
  }
  return t;
}

TreeNode * Lvalue_exp(void)
{ 
  TreeNode * t = newStmtNode(LvalueK),*p=NULL;
  if ((t!=NULL) && (token==ID))
    {
      t->attr.id.name = copyString(tokenString);
      t->attr.id.datatype=SIMPLEID;
    }
  
  match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
  switch(token)
    {
    case LBRACK:  t->attr.id.datatype=ARRAYID;
      match(LBRACK);
 printf("Returned from match  at line : %d \n", __LINE__);
      if (t!=NULL)    t->child[0]=expr();
 printf("Returned from  line : %d \n", __LINE__);
      match(RBRACK);
 printf("Returned from match  at line : %d \n", __LINE__);
      if(token==OF)
	{
	  match(OF);
 printf("Returned from match  at line : %d \n", __LINE__);
	  t->child[1]=expr();
 printf("Returned from  line : %d \n", __LINE__);
	}
      
      break;
    case  LPAREN : t->attr.id.datatype=FUNCTIONCALL;
      match(LPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
      if(t!=NULL&&token!=RPAREN)  t->child[0]=Exp_list();
      match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
      break;
    case  ASSIGN : p=newStmtNode(AssignK);
      p->child[0]=t;
      t=p;
      match(ASSIGN);
 printf("Returned from match  at line : %d \n", __LINE__);
      if (t!=NULL) t->child[1] = expr();
 printf("Returned from  line : %d \n", __LINE__);
      break;
      
    default:     p = newStmtNode(IdK);
      p->attr.id.name = t->attr.id.name;
      p->attr.id.datatype=t->attr.id.datatype;
      t=p;
      return t;
      
    }
  return t;
}


TreeNode * Str_const(void)                         
{ TreeNode * t = newStmtNode(Str_constK);
  
  if (t!=NULL)
    {
      t->attr.id.name = copyString(tokenString);
      t->attr.id.datatype=SIMPLEID;
      t->type = ETYPE_STRING;
    }
  match(STRING);
 printf("Returned from match  at line : %d \n", __LINE__);
  return t;
}

TreeNode * Int_exp(void)                        
{ TreeNode * t = newStmtNode(IntK);
  if (t!=NULL)
    {
      t->attr.val = atoi(tokenString);    
      t->type = ETYPE_INTEGER;
      //t->attr.id.datatype = SIMPLEID; TODO
      t->attr.id.name = copyString(tokenString);	
	}
  match(INT);
 printf("Returned from match  at line : %d \n", __LINE__);
  return t;
}

TreeNode * Nil_exp(void)
{ TreeNode * t = newStmtNode(NilK);
  t->type = ETYPE_NIL;
  match(NIL);
 printf("Returned from match  at line : %d \n", __LINE__);
  return t;
}

TreeNode * Par_exp(void)
{ TreeNode * t = newStmtNode(Par_expK);
   match(LPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
   if(token==RPAREN)
     {
       t=newStmtNode(NilK);
       match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
       return t;
     }
   if(t!=NULL)     t->child[0]=Expr_seq();
   match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
   return t;
}

TreeNode * Expr_seq(void)
{TreeNode *q=NULL,*temp=NULL;
TreeNode * t =expr();
 printf("Returned from  line : %d \n", __LINE__);
temp=t;
   while(token!=RPAREN &&token!=END&&token!=ENDFILE)
     {
       match(SEMICOLON);
 printf("Returned from match  at line : %d \n", __LINE__);
       q=expr();
 printf("Returned from  line : %d \n", __LINE__);
       if(q!=NULL) 
	 {
	   temp->sibling=q;
	   temp=temp->sibling;
	 }
       
     }
   return t;
}


TreeNode * Exp_list(void)
{ TreeNode *q=NULL;
  TreeNode * t =expr();
 printf("Returned from  line : %d \n", __LINE__);  
  while(token!=RPAREN &&token!=END&&token!=ENDFILE)
    {
      match(COMMA);
 printf("Returned from match  at line : %d \n", __LINE__);
      q=expr();
 printf("Returned from  line : %d \n", __LINE__);
      if(q!=NULL) t->sibling=q;
    }
  return t;
}

TreeNode * Decl_list(void){ 
  
  TreeNode * t=newStmtNode(Decl_listK);
  TreeNode * var=newStmtNode(Var_declK);
  TreeNode * type=newStmtNode(Type_declK);
  TreeNode * func=newStmtNode(Func_declK);
  t->child[1]=var;
  t->child[0]=type ;
  t->child[2]=func;
  
  while(token!=IN &&token!=ENDFILE){
    TreeNode *temp=NULL;
    switch(token){
    case TYPE:  
      match(TYPE);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp=newStmtNode(Type_declK);
      temp->child[0]=Typeid();
      match(EQ);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp->child[1]=Type();
      types[typecount].typeid = (temp->child[0])->attr.id.name; //store type definitions
      types[typecount].type = (temp->child[1])->type;
      types[typecount++].datatype = (temp->child[1])->attr.id.datatype;
      (temp->child[0])->type=(temp->child[1])->type; 	//type attribute added
      (temp->child[0])->attr.id.datatype=(temp->child[1])->attr.id.datatype;
      //if((temp->child[0])->type == ETYPE_INTEGER)
      //printf("dt: %d",(temp->child[0])->type);
      if((t->child[0])->child[0] ==NULL){ 
	type=temp; t->child[0]=type; 
      }else{
	type->sibling=temp;
	type=type->sibling;
      }
      match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
      break;
    case VAR:	
      match(VAR);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp=newStmtNode(Var_declK);
      temp->attr.id.name = copyString(tokenString);
      temp->attr.id.datatype=SIMPLEID;
      match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
      if(token==COLON){
	match(COLON);
 printf("Returned from match  at line : %d \n", __LINE__);
	temp->child[0]=Typeid();
	temp->type = (temp->child[0])->type;
	temp->attr.id.datatype = (temp->child[0])->attr.id.datatype;
	match(ASSIGN);
 printf("Returned from match  at line : %d \n", __LINE__);
	temp->child[1]=expr();
 printf("Returned from  line : %d \n", __LINE__);
      }else{
	match(ASSIGN);
 printf("Returned from match  at line : %d \n", __LINE__);
	temp->child[1]=expr();
 printf("Returned from  line : %d \n", __LINE__);
	temp->type = (temp->child[1])->type;
      }
      if(var->child[1]==NULL) {
	var=temp; t->child[1]=var; 
      }else{
	var->sibling=temp;
	var=var->sibling;
      }
      break;
    case FUNCTION:
      temp=newStmtNode(Func_declK);
      match(FUNCTION);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp->attr.id.name = copyString(tokenString);
      temp->attr.id.datatype=FUNCTIONCALL;
      match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
      match(LPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
      if(token!=RPAREN)
	temp->child[0]=Typefields();
      match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
      if(token==COLON){
	match(COLON);
 printf("Returned from match  at line : %d \n", __LINE__);
	temp->child[1]=Typeid();
	(t->child[2])->type = (temp->child[1])->type;
      }else
	t->child[2]->type = ETYPE_NIL;
      match(EQ);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp->child[2]=expr();
 printf("Returned from  line : %d \n", __LINE__);
	if(func->child[0]==NULL) {
	  func=temp; t->child[2]=func; 
	}else{
	  func->sibling=temp;
	  func=func->sibling;
	}
	break;
    default:	
      syntaxError("Expected type, var or function\n");
#ifdef DEBUG
      printf("TokenType : %d\n",token);
#endif
      exit(0);
      break;
    }   
  }
  if(t->child[0]->child[0] == NULL) //if no type declarations
    t->child[0] = NULL;
  if(t->child[1]->child[1] == NULL) //if no var declarations
    t->child[1] = NULL;
  if(t->child[2]->child[2] == NULL) //if no function declarations
    t->child[2] = NULL;
  return t;
}

TreeNode * Typeid(void){ 
  
  TreeNode * t=newStmtNode(TypeidK); ;
  if(token==INT)
   {
     t->type=ETYPE_INTEGER;
     t->attr.id.datatype = SIMPLEID;
     t->attr.id.name = "int";
   }
  else if(token==STRING)
    {
	t->type=ETYPE_STRING;
	t->attr.id.datatype = SIMPLEID;
	t->attr.id.name = "string";
    }
  else if(token==ID) {
    t->attr.id.name = copyString(tokenString);
    t->type = getType(tokenString);
    t->attr.id.datatype=getDataType(tokenString);
  }
  
  // else Error=TRUE;
  match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
  return t;
}

int getType(char *typeid)
{
  int i;
  for (i=0;i<typecount;i++)
    if (!strcmp(typeid,types[i].typeid))
      return types[i].type;
  return -1;
}

int getDataType(char *typeid)
{
  int i;
  for (i=0;i<typecount;i++)
    if (!strcmp(typeid,types[i].typeid))
      return types[i].datatype;
  return -1;
}

TreeNode * Type(void)
{ TreeNode * t=newStmtNode(TypeK);
  
  if(token==ARRAY)
    {
      // set attribute of typek node to array
      
      //t->attr.id.name = copyString(tokenString);
      t->attr.id.datatype=ARRAYID;
      
      match(ARRAY);
 printf("Returned from match  at line : %d \n", __LINE__);
      match(OF);
 printf("Returned from match  at line : %d \n", __LINE__);
      if(t!=NULL)
	{
	  if(token==INT)
	    {
	      t->type=ETYPE_INTEGER;
	      t->attr.id.name = "array of int";
	    }
	  else if(token==STRING) 
	    {
	      t->type=ETYPE_STRING;
	      t->attr.id.name = "array of string";
	    }
	}
      return t;
    }
  else
    {
      //set attribute of typek to simple
      
      t->attr.id.name = copyString(tokenString);
      t->attr.id.datatype=SIMPLEID;
      
      if(t!=NULL){
	if(token==INT)
	  {
	    t->type=ETYPE_INTEGER;
	    t->attr.id.name = "int";
	  }
	else if(token==STRING)
	  {
	    t->type=ETYPE_STRING;
	    t->attr.id.name = "string";
	  }
      }
    }
  return t;
}


TreeNode * Typefields(void)
{
  TreeNode * t=newStmtNode(TypefieldK),*p=NULL,*temp=NULL;
  t->attr.id.name = copyString(tokenString);
  match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
  match(COLON);
 printf("Returned from match  at line : %d \n", __LINE__);
  t->child[0]=Typeid();
  t->attr.id.datatype = (t->child[0])->attr.id.datatype;
  t->type = t->child[0]->type;
  p=newStmtNode(TypefieldK);
  
  while(token!=RPAREN&&token!=ENDFILE)
    {
      match(COMMA);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp=newStmtNode(TypefieldK);
      temp->attr.id.name = copyString(tokenString);
      match(ID);
 printf("Returned from match  at line : %d \n", __LINE__);
      match(COLON);
 printf("Returned from match  at line : %d \n", __LINE__);
      temp->child[0]=Typeid();
      temp->attr.id.datatype = (temp->child[0])->attr.id.datatype;
      temp->type = temp->child[0]->type;
      if(p->child[0]==NULL)
	{
	  p=temp;
	  t->sibling=p;
	}
      else
	{
	  p->sibling=temp;
	  p=p->sibling;
	}
      p->sibling=NULL;
    }
  
  return t;
}


TreeNode * expr(void)
{
  TreeNode * t=NULL;
  int old_line;
  TokenType temp1,temp2;
  switch(token)
    {
    case LET: 	
	t=Let_exp();
     printf("Return Let_Exp at line : %d\n", __LINE__);
      break;
    case IF: 	t=If_exp();
      break;
    case WHILE:	t=While_exp();
      break;
    case FOR:		t=For_exp();
      break;
    case STRING:	t=Str_const();
      break;
    case NIL:		t=Nil_exp();
      break;
    case LPAREN:	t=Par_exp();
      break;
    case ID:  	temp1=lookahead(1,0);
      if(temp1==PLUS||temp1==MINUS||temp1==TIMES||temp1==DIVIDE||temp1==LT||temp1==LE||temp1==GT||temp1==GE||temp1==EQ||temp1==NEQ)
	t=Comp_exp();
      else t=Id_exp();
      break;
    case INT:		temp1=lookahead(1,0);
      if(temp1==PLUS||temp1==MINUS||temp1==TIMES||temp1==DIVIDE||temp1==temp1==LT||temp1==LE||temp1==GT||temp1==GE||temp1==EQ||temp1==NEQ)
	t=Comp_exp();
      else t=Int_exp();
      break;
      
    default:	
      syntaxError("Expected let,while,for,string,int,nil,( or id\n");
      exit(0);
    }
  
  return t;
}

/* Function exp4break is called from the For_exp function only,
   It includes an extra case for the break statement */
TreeNode * exp4break(TreeNode *p)
{
  TreeNode * t=NULL;
  TokenType temp1;
  switch(token)
    {
    case LET: 	t=Let_exp();
     printf("Return from LetExp at line :  %d \n", __LINE__);
      break;
    case IF: 	t=If_exp();
      break;
    case WHILE:	t=While_exp();
      break;
    case FOR:		t=For_exp();
      break;
    case STRING:	t=Str_const();
      break;
    case NIL:		t=Nil_exp();
      break;
    case LPAREN:	t=Par_exp();
      break;
    case ID:		temp1=lookahead(1,0);
      if(temp1==PLUS||temp1==MINUS||temp1==TIMES||temp1==DIVIDE||temp1==temp1==LT||temp1==LE||temp1==GT||temp1==GE||temp1==EQ||temp1==NEQ)
	t=Comp_exp();
      else t=Id_exp();
      break;
    case INT:		temp1=lookahead(1,0);
      if(temp1==PLUS||temp1==MINUS||temp1==TIMES||temp1==DIVIDE||temp1==temp1==LT||temp1==LE||temp1==GT||temp1==GE||temp1==EQ||temp1==NEQ)
	t=Comp_exp();
      else t=Int_exp();
      break;
    case BREAK:	t=newStmtNode(BreakK);
      t->attr.loopaddr=p;
      break;
    default:	syntaxError("Expected let,while,for,string,int,nil,( or id");
      exit(0);
    }
  
  return t;
}


TreeNode * Comp_exp(void)
{ TreeNode * t = Bin_exp();
	
  while (token==OR)
    { TreeNode * p = newStmtNode(IfK);  //Ifk node created for short circuitry
      if (p!=NULL)
	{
	  // p->child[0] = t;
	  t = p;
	  match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
	  t->child[1] = newStmtNode(IntK);	/*	exp1 OR exp2 converted to: if exp1 then 1 else exp2 */
	  (t->child[1])->attr.val=1;
	  t->child[2]=Bin_exp();
    }
  }
  return t;
}


TreeNode * Bin_exp(void)
{ TreeNode * t = Bin1_exp();
  while (token==AND)
  { TreeNode * p = newStmtNode(IfK);
    if (p!=NULL)
	{
      p->child[0] = t;
      t = p;
      match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
      t->child[1] = Bin1_exp();
	  t->child[2] = newStmtNode(IntK);	/*	exp1 AND exp2 converted to: if exp1 then exp2 else 0 */
	  (t->child[2])->attr.val=0;
    }
  }
  return t;
}

TreeNode * Bin1_exp(void)
{ 
 TreeNode * t =newStmtNode(Bin1_expK),*p=NULL;
 if(token==LPAREN)
 {
  match(LPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
  t->child[0]=Bin1_exp();
  match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
  if(token==LT||token==LE||token==GT||token==GE||token==EQ||token==NEQ)
  {
   p=newStmtNode(OpK);
   p->attr.op=token;
   p->child[0]=t->child[0];
   t->child[0]=p;
   match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
   if(lookahead(1,0)==LPAREN)
	t->child[1]=Bin1_exp();
   else
	p->child[1]=Simp_exp();
    //t->child[1]=Simp_exp();
  }
  }
  else
  {
   t->child[0]=Simp_exp();
   if(token==LT||token==LE||token==GT||token==GE||token==EQ||token==NEQ)
   {
    p=newStmtNode(OpK);
    p->attr.op=token;
    p->child[0]=t->child[0];
    t->child[0]=p;
	match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
	//t->child[1]=Simp_exp();
	p->child[1]=Simp_exp();
   }
  }
  t->type = (t->child[0])->type; //or t->child[1]->type, both r same
 return t;
}



TreeNode * Simp_exp(void)
{ 
  TreeNode * t = term();
 while ((token==PLUS)||(token==MINUS))
   { TreeNode * p = newStmtNode(OpK);
    if (p!=NULL) 
      {
	  p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
      t->child[1] = term();
	}
    t->type = ETYPE_INTEGER;
  }
 return t;
}

TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES)||(token==DIVIDE))
    { TreeNode * p = newStmtNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
 printf("Returned from match  at line : %d \n", __LINE__);
      p->child[1] = factor();
    }
    t->type = ETYPE_INTEGER;
  }
  return t;
}

TreeNode * factor(void)
{ 
 TreeNode * t = NULL;
 if(token==MINUS)
    {
    t=newStmtNode(NegatedK);
   match(MINUS);
 printf("Returned from match  at line : %d \n", __LINE__);
   t->child[0]=Paran_exp();
   t->type = t->child[0]->type;
  }
  else t=Paran_exp();
  
  return t;
}


TreeNode * Paran_exp(void)
{ 
  TreeNode * t = NULL;
 switch (token) 
    {
    case INT :
      t = newStmtNode(IntK);
      if ((t!=NULL) && (token==INT))
        t->attr.val = atoi(tokenString);
      t->type = ETYPE_INTEGER;
		t->attr.id.name = copyString(tokenString);
		match(INT);
 printf("Returned from match  at line : %d \n", __LINE__);
      break;
    case ID :
      t = Lvalue_exp();               //t = Lvalue_exp();
      break;
    case LPAREN :
      match(LPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
      t = expr();
 printf("Returned from  line : %d \n", __LINE__);
      match(RPAREN);
 printf("Returned from match  at line : %d \n", __LINE__);
      break;
    case STRING:
	  t = newStmtNode(Str_constK);
		if ((t!=NULL) && (token==STRING))
	{
	    t->attr.id.name = copyString(tokenString);
	t->attr.id.datatype = SIMPLEID;
		t->type = ETYPE_STRING;
	  }
      match(STRING);
 printf("Returned from match  at line : %d \n", __LINE__);
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      printf ("FIle : %s  line :  %d.\n",__FILE__, __LINE__);
      token = getToken();
      break;
    }
  return t;
}
/* the primary function of the parser  ; Function parse returns the newly  constructed syntax tree   */
TreeNode * parse(void)
{ 
  TreeNode * t;
  printf ("FIle : %s  line :  %d.\n",__FILE__, __LINE__);
  token = getToken();
  t = expr();
 printf("Returned from  line : %d \n", __LINE__);
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
