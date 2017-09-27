#include<stdio.h>
#include "globals.h"

#define  pc 7
#define fp 2
#define  mp 6
#define gp 5
#define  ac 0
#define  ac1 1
FILE *tcode;

static int emitLoc = 0 ;


static int highEmitLoc = 0;

#define NO_PARSE FALSE
#define NO_ANALYZE TRUE

#define SHIFT 4


#define NO_CODE TRUE
#define MAXTOKENLEN 40


#define ofpFO 0
#define retFO -1
#define initFO -2
char fn[40];
char cfn[40];

  int frameoffset;
int p =-2;
  int temp = 0;
int incallk =0;
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;


typedef struct BucketListRec
   { char * name;
     int memloc ;
    int isarray; 
     struct BucketListRec * next;
   } * BucketList;


 static BucketList hashTablegb[SIZE];
static int nextavailgbloc = 0;
typedef struct symtablefn
{
  BucketList hashTablefn[SIZE];
  char  fnname[40];
 int nextavailloc;
} symtabfnlist;
static symtabfnlist * symtablist[20]= {NULL}; 
static int location = 0;
static int nextavail_gbloc = 0;
static char currentfnname[40];


void emitComment( char * c )
{ if (TraceCode)
 fprintf(tcode,"* %s\n",c);}


void emitRO( char *op, int r, int s, int t, char *c)
{ fprintf(tcode,"%3d:  %5s  %d,%d,%d ",emitLoc++,op,r,s,t);
  if (TraceCode) 
  fprintf(tcode,"\t%s",c) ;
  fprintf(tcode,"\n") ;
  if (highEmitLoc < emitLoc) highEmitLoc = emitLoc ;
} 
void emitRM( char * op, int r, int d, int s, char *c)
{ fprintf(tcode,"%3d:  %5s  %d,%d(%d) ",emitLoc++,op,r,d,s);
  if (TraceCode)
 fprintf(tcode,"\t%s",c) ;
  fprintf(tcode,"\n") ;
  if (highEmitLoc < emitLoc)  highEmitLoc = emitLoc ;
} 
int emitSkip( int howMany)
{  int i = emitLoc;
   emitLoc += howMany ;
   if (highEmitLoc < emitLoc)  highEmitLoc = emitLoc ;
   return i;
} 
void emitBackup( int loc)
{ if (loc > highEmitLoc) emitComment("BUG in emitBackup");
  emitLoc = loc ;
} 
void emitRestore(void)
{ emitLoc = highEmitLoc;}


void emitRM_Abs( char *op, int r, int a, char * c)
{ fprintf(tcode,"%3d:  %5s  %d,%d(%d) ",
               emitLoc,op,r,a-(emitLoc+1),pc);
  ++emitLoc ;
  if (TraceCode)
 fprintf(tcode,"\t%s",c) ;
  fprintf(tcode,"\n") ;
  if (highEmitLoc < emitLoc) highEmitLoc = emitLoc ;
} 


void emitRM_Abs1( char *op, int r, int a, char * c)
{ fprintf(tcode,"%3d:  %5s  %d,%d(%d) ",
               emitLoc,op,r,a,pc);
  ++emitLoc ;
  if (TraceCode)
  fprintf(tcode,"\t%s",c) ;
  fprintf(tcode,"\n") ;
  if (highEmitLoc < emitLoc) highEmitLoc = emitLoc ;
} 

static int islocal =0 ;
static int isaddress = 0;
static int tmpOffset = 0;
char cfunction[40];
struct fntable
{
	char func_name[40];
	int addr;
	int frameoffset;
} fnentry[50];
static int countfn= 0;



static void cGen (TreeNode * tree);


int st_lookuparray(TreeNode * q,char *fn)
{
    int h = hash(q->attr.id.name);
     int i;
     BucketList l,n;
   char name[50]; 
	strcpy(name, q->attr.id.name);
  
   for(i=0;i<maxfn;i++)
     {
         if(strcmp(fn,(symtablist[i])->fnname)== 0) 
        {
            l = (symtablist[i])->hashTablefn[h];
           
             break;
        }
      }
   
while ((l != NULL) && (strcmp(name,l->name) != 0))
         l = l->next;
if(l!=NULL)  { return l ->isarray;}
   else
   { 
   l =  hashTablegb[h]; 
   
   while ((l != NULL) && (strcmp(name,l->name) != 0))
         l = l->next;
  if(l==NULL) return -1;
  else{ 
  return l->isarray;}
}
}




int st_lookupoffset(char *fn)
{
   
     int i;
    
   for(i=0;i<maxfn;i++)
     {
         if(strcmp(fn,(symtablist[i])->fnname)== 0) 
        {
  
             break;
        }
      }
   

 return symtablist[i]->nextavailloc-1;
 
}






static void add_entry(TreeNode * tree,int address)
{	
	int countarg=0;
	TreeNode * p;
	p = tree->child[0];
	
	while(p!=NULL)
	{
		countarg++;
		p = p->sibling;
	}

	strcpy(fnentry[countfn].func_name,tree->attr.id.name);
	fnentry[countfn].addr = address;
	fnentry[countfn].frameoffset = ((st_lookupoffset(tree->attr.id.name)+2)*-1);
	countfn++;
}

static void add_entry_1(char *name,int address)
{
	strcpy(fnentry[countfn].func_name,name);
	fnentry[countfn].addr = address;
	//fnentry[countfn].frameoffset = ((st_lookupoffset(tree->attr.id.name)+2)*-1);
	countfn++;
	
}

static int findFrameoffset(char function_name[])
{
	int i;
	for(i=0;i<countfn;i++)
	{
		if(strcmp(fnentry[i].func_name,function_name)==0)
		{
			return fnentry[i].frameoffset ;
		}
	}
	fprintf(stdout,"function undeclared before usage \n");
	exit (0);
}

static int findAddr(char function_name[])
{
	int i;
	for(i=0;i<countfn;i++)
	{
		if(strcmp(fnentry[i].func_name,function_name)==0)
		{
			return fnentry[i].addr ;
		}
	}

	fprintf(stdout,"function undeclared before usage \n");
	exit (0);
}





static void genExp( TreeNode * tree)
{ int loc;

  int addr;
  
  TreeNode * p1, * p2, * p3;
  int savedLoc1,savedLoc2,currentLoc;
  
  switch (tree->expkind) {
	
	case Var_declK:
		
		cGen(tree->child[1]);
		add_entry_1( tree->attr.id.name,tmpOffset);
		
		emitRM("ST",ac,tmpOffset--,mp,"op: push left");
		
		break;
	case LetK:
		currentLoc = emitSkip(0);
		add_entry_1( "in",currentLoc);
		strcpy(cfunction,"in");
		cGen(tree->child[0]);
		/*currentLoc = emitSkip(0);
		add_entry_in(currentLoc);
		strcpy(cfunction,"in");*/
		cGen(tree->child[1]);
		
	
		break;
	case Decl_listK:
		cGen(tree->child[1]);
		if(tree->child[2] != NULL)
		{
		cGen(tree->child[2]);
		}
		break;
	case Func_declK:
		currentLoc = emitSkip(0);
		add_entry(tree, currentLoc);
		strcpy(cfunction,tree->attr.id.name);
		emitRM("ST",ac,retFO,fp,"save return address");
		strcpy(cfn,tree->attr.id.name);
		cGen(tree->child[2]);
		
		break;
	case IfK :
         if (TraceCode) 
		emitComment("-> if") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         p3 = tree->child[2] ;
         
         cGen(p1);
         savedLoc1 = emitSkip(1) ;
         emitComment("if: jump to else belongs here");
         
         cGen(p2);
         savedLoc2 = emitSkip(1) ;
         emitComment("if: jump to end belongs here");
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc1) ;
         emitRM_Abs("JEQ",ac,currentLoc,"if: jmp to else");
         emitRestore() ;
         
         cGen(p3);
         currentLoc = emitSkip(0) ;
         emitBackup(savedLoc2) ;
         emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
         emitRestore() ;
         if (TraceCode)  
		 emitComment("<- if") ;
         break; 

      case WhileK:
         if (TraceCode) 
		 emitComment("-> while") ;
         p1 = tree->child[0] ;
         p2 = tree->child[1] ;
         savedLoc1 = emitSkip(0);
         
         cGen(p1);
	savedLoc2 = emitSkip(1);
         
         cGen(p2);
	 emitRM_Abs("LDA",pc,savedLoc1,"");
	 currentLoc = emitSkip(0);
	 emitBackup(savedLoc2);
         emitRM_Abs("JEQ",ac,currentLoc,"while: jmp back to condition");
	 emitRestore();         
	 if (TraceCode) 
	 emitComment("<- endwhile") ;
         break; 

     
    case IntK :
      if (TraceCode) 
	 emitComment("-> Const") ;
      
      emitRM("LDC",ac,tree->attr.val,0,"load const");
      if (TraceCode) 
	 emitComment("<- Const") ;
      break; 
    case Bin1_expK:
			cGen(tree->child[0]);
			break;
    case IdK :
	case LvalueK:

      
	 switch(tree->attr.id.datatype)
	{
	case SIMPLEID:
		
	 emitComment("-> Id") ;
      loc = findAddr(tree->attr.id.name);
	

 emitRM("LD",ac,loc,mp,"load id value");//}}
     if (TraceCode)
	  emitComment("<- Id") ;
      break; 
	

    case ARRAYID :
      loc = st_lookup_pos(tree);
        
	if(st_lookuparray(tree,cfn)==2)
	{	if(islocal==0 ) emitRM("LD",ac1,loc,mp,"load id value");else emitRM("LD",ac1,loc,mp,"load id value");
		cGen(tree->child[0]);
		if(islocal==0 ) { emitRO("ADD",ac1,ac1,ac,"op +");	}
		else {emitRO("SUB",ac1,ac1,ac,"op +"); }
		if(isaddress) emitRM("LDA",ac,0,ac1,"");
		else emitRM("LD",ac,0,ac1,"");
	}	
				
else
{


      cGen(tree->child[0]);

if(islocal==1 ) { emitRM("LDC",ac1,-1,0,"load const"); emitRO("MUL",ac,ac1,ac,"op *");}
else { }
emitRM("LDC",ac1,loc,0,"load const");
	emitRO("ADD",ac,ac1,ac,"op +");

emitRM("ST",ac,tmpOffset--,mp,"op: push left");
emitRM("LD",ac1,++tmpOffset,mp,"op: load left");

if(isaddress &&islocal)
	{emitRO("ADD",ac1,ac1,mp,"load id value");
	emitRM("LDA",ac,0,ac1,"");}
if(isaddress && !islocal)
	{emitRO("ADD",ac1,ac1,mp,"load id value");
	emitRM("LDA",ac,0,ac1,"");}
if(!isaddress &&islocal)
	{emitRO("ADD",ac1,ac1,mp,"load id value");
	emitRM("LD",ac,0,ac1,"");}
if(!isaddress && !islocal)
	{emitRO("ADD",ac1,ac1,mp,"load id value");
	emitRM("LD",ac,0,ac1,"");}

}
	break;



    case FUNCTIONCALL :
	if(!strcmpi(tree->attr.id.name,"print"))
	{
		cGen(tree->child[0]);
        
         emitRO("OUT",ac,0,0,"write ac");
		break;
	}
	else if(!strcmpi(tree->attr.id.name,"read"))
	{
		cGen(tree->child[0]);
        
        emitRO("IN",ac1,0,0,"read integer value"); 	
		loc = findAddr(tree->child[0]->attr.id.name);
		emitRM("ST",ac1,loc,mp,"read: store value");
		break;
	
	}
	temp = 1;
	 incallk =0;
	frameoffset = findFrameoffset(cfunction);
	
	addr = findAddr(tree->attr.id.name);
        p1= tree->child[0];
	incallk =1;
	
		cGen(p1);
		
		 emitComment("computing args ") ;
	
	
		  

	emitRM("ST",fp,frameoffset+ofpFO-1,fp,"store current frame");
	emitRM("LDA",fp,frameoffset-1,fp,"push neew frame") ;
	emitRM("LDA",ac,1,pc,"") ;
	
	 emitRM_Abs("LDA",pc,addr,"jump to fn");
	emitRM("LD",fp,ofpFO,fp,"load return address");
	incallk = 0;
	break;
	}
	break;
    case OpK :
         if (TraceCode) 
		 emitComment("-> Op") ;
         p1 = tree->child[0];
         p2 = tree->child[1];
         
         switch (tree->attr.op) {
            case PLUS :/* gen tinycode for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("ADD",ac,ac1,ac,"op +");
               break;
            case MINUS :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op -");
               break;
            case TIMES :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("MUL",ac,ac1,ac,"op *");
               break;
	    case DIVIDE :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("DIV",ac,ac1,ac,"op /");
               break;
            case LT :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op <") ;
               emitRM("JLT",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
 	   case GT :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op <") ;
               emitRM("JGT",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
 	   case LE :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op <") ;
               emitRM("JLE",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
 	   case GE :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op <") ;
               emitRM("JGE",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case EQ :/* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op ==") ;
               emitRM("JEQ",ac,2,pc,"br if true");
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
	   case NEQ :/* gen code for ac = left arg */
	 cGen(p1);
	 /* gen code to push left operand */
	 emitRM("ST",ac,tmpOffset--,mp,"op: push left");
	 /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
               emitRO("SUB",ac,ac1,ac,"op ==") ;
               emitRM("JNE",ac,2,pc,"br if true");
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
		
	   case ASSIGN :
		if (TraceCode) 
		
		emitComment("-> assign") ;
         /* generate code for rhs */
         //cGen(tree->child[1]);
         /* now store value */
         //loc = st_lookup_pos(tree);
	/* isaddress =1;
	 cGen(tree->child[0]);
	 isaddress =0;
	 emitRM("ST",ac,tmpOffset--,mp,"op: push left");*/
	
	//isaddress = 1;
	if((tree->child[0]->expkind==IdK) && tree->child[0]->attr.id.datatype == ARRAYID)
	{
	isaddress=1;
	 cGen(tree->child[0]);
	isaddress = 0;
		 emitRM("ST",ac,tmpOffset--,mp,"op: push left");
		 cGen(tree->child[1]);
		emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
		if(islocal==1){ emitRM("ST",ac,0,ac1,"");}
	 else { emitRM("ST",ac,0,ac1,"");}
	} 
	else
	{
		
		 cGen(tree->child[1]);
		 //printf("mc: %s",tree->child[0]->attr.id.name);
		loc = findAddr(tree->child[0]->attr.id.name);
		
	/* emitRM("LD",ac1,++tmpOffset,mp,"op: load left");*/
          if(islocal==1){ emitRM("ST",ac,loc,mp,"");}
	 else { emitRM("ST",ac,loc,mp,"");}
 	}       
 	if (TraceCode)  
	emitComment("<- assign") ;
	
         break;
	      


 default:
               emitComment("BUG: Unknown operator");
               break;
         } 
         if (TraceCode)  
		emitComment("<- Op") ;
         break; 
	case Par_expK:
			cGen(tree->child[0]);
			break;
    default: emitComment("BUG: U");
      break;
  }
 }


static void cGen( TreeNode * tree)
{ if (tree != NULL)
  { 
        genExp(tree);
       
     if(incallk ==1 && tree->prm==1)
	{emitRM("ST",ac,frameoffset+initFO-temp,fp,""); 
	/*emitRM("ST",ac,p+temp,fp,"");*/temp++;}
    cGen(tree->sibling);
  }
}


void codeGen(TreeNode * syntaxTree, char * codefile)
{  char * s = malloc(strlen(codefile)+7);
	int addr,savedLoc2;
   strcpy(s,"File: ");
   strcat(s,codefile);
   tcode = fopen("prj/tcode.tm","w");
   emitComment("Tiger Compilation to TM Code");
   emitComment(s);
  
   emitRM("LD",mp,0,ac,"load maxaddress from location 0");
    
   emitRM("LDC",fp,3000,ac,"stack....") ;
	
  
   emitRM("ST",ac,0,ac,"clear location 0");
	savedLoc2 = emitSkip(1);

	
   
   cGen(syntaxTree);
   
emitBackup(savedLoc2);

 addr = findAddr("in");
     emitRM_Abs("LDA",pc,addr,"jump to fn");
         
	 emitRestore(); 
   emitComment("End of execution.");
   emitRO("HALT",0,0,0,"");
}


int st_lookup_pos(TreeNode * q)
{
     int h = hash(q->attr.id.name);
     int i;
     BucketList l,n;
     char name[50] ;
     strcpy(name, q->attr.id.name);
  

     
     for(i=0;i<maxfn;i++)
     {
         if(strcmp(cfunction,(symtablist[i])->fnname)== 0) 
        {
            l = (symtablist[i])->hashTablefn[h];
           
             break;
        }
      }
     
    while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
if(l!=NULL) { islocal = 1; return (l->memloc+2)*-1;}
   else
   { islocal = 0;
   l =  hashTablegb[h]; 
    islocal = 0;
   while ((l != NULL) && (strcmp(name,l->name) != 0))
         l = l->next; 
  if(l==NULL) return -1;
  else 
  return l->memloc;
}
}
