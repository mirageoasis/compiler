/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

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
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6


extern FILE *source;  /* source code text file */
extern FILE *listing; /* listing output text file */
extern FILE *code;    /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum
{
   StmtK,
   ExpK
} NodeKind;
typedef enum
{
   IfK,
   RepeatK,
   ReadK,
   WriteK,
   ReturnK,
   CompoundK
} StmtKind;
typedef enum
{
   OpK,
   ConstK,
   IdK,
   AssignK,
   VarDelcare,
   ArrayDeclare,
   FunctionDeclare
} ExpKind;

/* ExpType is used for type checking */
typedef enum
{
   Void,
   Integer
} ExpType;

#define MAXCHILDREN 3

typedef enum
/* book-keeping tokens */
{
   ENDFILE,
   ERROR,
   COMMENT_ERROR,
   ID_ERROR,
   /* reserved words */
   IF,
   ELSE,
   INT,
   RETURN,
   VOID,
   WHILE,
   /* multicharacter tokens */
   ID,
   NUM,
   /* special symbols */
   // 사칙연산
   PLUS,
   MINUS,
   TIMES,
   DIV,
   // 왼쪽 숫자가 작은지 여부 < <=
   LT,
   LTE,
   // 왼쪽 숫자가 큰지 여부 > >=
   GT,
   GTE,
   // 같은지 여부
   EQ,
   NEQ,
   // 할당
   ASSIGN,
   // 세미콜론
   SEMI,
   // 콤마
   COMMA,
   // 괄호
   LPAREN,
   RPAREN,
   // 중괄호
   LCURL,
   RCURL,
   // 대괄호
   LBRACKET,
   RBRACKET
} TokenType;


typedef struct treeNode
{
   struct treeNode *child[MAXCHILDREN];
   struct treeNode *sibling;
   int lineno;
   int arraySize;
   int IsParameter;
   NodeKind nodekind;
   union
   {
      StmtKind stmt;
      ExpKind exp;
   } kind;
   union
   {
      TokenType op;
      int val;
      char *name;
   } attr;
   ExpType type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;