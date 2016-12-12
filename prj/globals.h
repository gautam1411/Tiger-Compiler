/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TIGER compiler          */
/* must come before other include files          */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 19

typedef enum 
    /* book-keeping tokens */
   {ENDFILE,ERROR,
    /* reserved words */
    ARRAY,BREAK,ELSE,DO,END,FOR,FUNCTION,IF,IN,LET,NIL,OF,THEN,TO,TYPE,VAR,WHILE,
    /* multicharacter tokens */
    ID,INT,STRING,
    /* special symbols */
    COMMA,COLON,SEMICOLON,ASSIGN,LBRACK,RBRACK,LBRACE,RBRACE,DOT,EQ,LT,GT,LE,GE,NEQ,AND,OR,PLUS,MINUS,TIMES,DIVIDE,LPAREN,RPAREN
   } TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for MIPS simulator */
extern FILE* scanlisting;
extern FILE* semlisting;

extern int lineno; /* source line number for listing */
extern int EchoSource ;
extern int TraceScan ;
extern int Error;
extern int TraceCode ;

/***********   Syntax tree for parsing ************/

#define MAXCHILDREN 3

static int maxfn =0;

typedef enum { ARRAYID ,SIMPLEID ,FUNCTIONCALL } Datatype;
 typedef enum {OpK,IntK,IdK,LetK,IfK,ForK,WhileK,Str_constK,NilK,Par_expK,Decl_listK,Var_declK,Type_declK,Func_declK,TypeidK,TypeK,TypefieldK,BreakK,Comp_expK,Bin_expK,Bin1_expK,NegatedK,LvalueK,AssignK} ExpKind;
 /* ExpType is used for type checking */
typedef enum {ETYPE_INTEGER = 10,ETYPE_STRING =11,ETYPE_NIL =12} ExpType;
 
typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     struct{ TokenType op;
             int val;
             
			 struct treeNode * loopaddr;
			 struct {Datatype datatype;char * name;  } id; 
		   } attr;
	 ExpKind expkind;
     ExpType type; /* for type checking of exps */
	 int prm;
   } TreeNode;

#endif
