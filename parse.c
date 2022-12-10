/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */

/*
구현할  함수는 다음과 같다.
decl_list:
decl:
var_decl:
type_specifier:
fun_decl: 
params:
param_list:
param:
compound_stmt:
local_decl:
stmt_list:
stmt:
selection_stmt:
iteration_stmt:
return_stmt:
expr_stmt:
expr:
var:
simple_expr:
relop:
additive_expr:
addop:
term:
mulop:
factor:
call:
args:
args_list:
*/

static TreeNode * declaration_list(void);
static TreeNode * declaration(void);
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}

ExpType type_specifier(void)
{
	ExpType ret;

  if (token == INT){
    ret = Integer;
  }else if(token == VOID){
    ret = Void;
  }else{
    syntaxError("type undefined!");
    printToken(token, tokenString);
    ret = Void;
  }

  token = getToken();
  return ret;
}

TreeNode * declaration_list(void)
{
	TreeNode *first;
  TreeNode *first_sibling_pointer;
	TreeNode *now;
  first = declaration();
  while(token != ENDFILE){
    now = declaration();
    if (now!=NULL){
      printf("not null");
    }
    if (now!=NULL) {
      if (first==NULL)
      {
        first = first_sibling_pointer = now;
        fprintf(stdout, "error has occured\n");
      }
      else
      { 
        /// 형제의 위치로 이동하는 상황
        first_sibling_pointer->sibling = now;
        first_sibling_pointer = now;
      }
    }
  }
  return first;
}


TreeNode * declaration(void)
{
	TreeNode *t;
	ExpType type;
	char *name;
  // 일단 type 맞추고
	type = type_specifier();
	name = copyString(tokenString);
  // id를 맞추고
	match(ID);
  printf("function decl\n");
  // id를 확인하면 다음 토큰은 
	
  if(token == SEMI){
    t = newExpNode(VarDelcare);
		if (t != NULL)
		{
			t->attr.name = name;
			t->type = type;
		}
		match(SEMI);
  }else if(token == LBRACKET){
    t = newExpNode(ArrayDeclare);
		if (t != NULL)
		{
			t->attr.name = name;
			t->type = type;
		}
    //[
		match(LBRACKET);
    // number
    if (t != NULL)
			t->arraySize = atoi(tokenString);
    match(NUM);
    // ]
    match(RBRACKET);
    //;
    match(SEMI);
  }
  else if(token == LPAREN){
    // 아직 함수 PARAMETER 완성 안됨
  }else{
    printToken(token, tokenString);
		token = getToken();
  }
	
	return t;
}

TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  while ((token!=ENDFILE)&&(token!=ELSE))
  { TreeNode * q;
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else // now p cannot be NULL either 
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode * statement(void)
{ TreeNode * t = NULL;
  switch (token) {
    case IF : t = if_stmt(); break;
    case ID : t = assign_stmt(); break;
    default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}

TreeNode * if_stmt(void)
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  if (t!=NULL) t->child[0] = exp();
  if (t!=NULL) t->child[1] = stmt_sequence();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = stmt_sequence();
  }
  return t;
}

TreeNode * repeat_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  //match(REPEAT);
  if (t!=NULL) t->child[0] = stmt_sequence();
  //match(UNTIL);
  if (t!=NULL) t->child[1] = exp();
  return t;
}

TreeNode * assign_stmt(void)
{ TreeNode * t = newStmtNode(AssignK);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  match(ASSIGN);
  if (t!=NULL) t->child[0] = exp();
  return t;
}

TreeNode * exp(void)
{ TreeNode * t = simple_exp();
  if ((token==LT)||(token==EQ)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_exp();
  }
  return t;
}

TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
        t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN :
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = declaration_list();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
