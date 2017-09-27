
#ifndef _UTIL_H_
#define _UTIL_H_
typedef char *string;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);
//void printTree( TreeNode * tree );

/*typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);*/

TreeNode * newStmtNode(ExpKind exp);
void printToken( TokenType token, char* tokenString );
int strcmpi(char *s1, char *s2);
char * copyString(char * s);
#endif
