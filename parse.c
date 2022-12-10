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
decl: // funcion 남음
var_decl:
fun_decl: 
params: // paramlist 든지, void 든지
param_list: // 이거도 declare 과 비슷한거 아님?
param: //  type-specifier Id 다음에 [] bracket match 를 해준다.
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

/*
TODO

함수를 개발
함수에 들어갈 parameter 개발

1. compound statement 개발
2. local-declarations statement-list 따져보자
3. local-declations 개발
4. statement-list 개발
5. expression | coumpound_statement | selection_statement| iteration_statement | return stmt
6. 일단 return stmt 만 개발해서 함수가 되는지 확인

*/
static TreeNode * declaration_list(void);
static TreeNode * params(void);
static TreeNode * param_list(ExpType);
static TreeNode * param(ExpType);
static TreeNode * compound_statement(void);



static TreeNode * declaration(void);
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * expression(void);
static TreeNode * simple_expression(void);
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
  first_sibling_pointer = first;
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
  printf("in function declaration\n");
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
    // 함수 parameter 가 비었다고 가정한다.
    t = newExpNode(FunctionDeclare);
    printf("function in declartion function\n");
		if (t != NULL)
		{
			t->attr.name = name;
			t->type = type;
		}
		// statement 가 없고 parameter 가 있다고 가정한다.
    match(LPAREN);
		// 파라미터 받는다.
    if (t != NULL)
			t->child[0] = params();
		match(RPAREN);
		
    // 함수 statement부분 일단 return 부분 먼저 개발한다.

    match(LPAREN);
    if (t != NULL)
			t->child[0] = compound_statement();
		match(RPAREN);

  }else{
    printToken(token, tokenString);
		token = getToken();
  }
	
	return t;
}

TreeNode* params(void){
  TreeNode * ret;
  // parameter 함수 선언하고 // semicolon 있는지 확인
  ExpType type;
  printf("in params function\n");
  type = type_specifier();
  // 
  if (type == Void && token == RPAREN){
    // parameter 가 없는 케이스
    printf("no parameter!\n");
    ret = newExpNode(VarDelcare);
    ret->IsParameter = TRUE;
    ret->type = Void;
  }else{
    printf("yes parameter!\n");
    ret = param_list(type);
  }
  return ret;
}

TreeNode* param_list(ExpType type){
  TreeNode * ret;
  // parameter 함수 선언하고 // semicolon 있는지 확인
  TreeNode *first;
  TreeNode *first_sibling_pointer;
	TreeNode *now;
  printf("in param_list function!\n");
  first = param(type);
  first_sibling_pointer = first;
  while(token == COMMA){
    /// 여러개 있는 경우
    // comma 맞춰주자
    match(COMMA);

    ExpType next_type = type_specifier(); // 다음 나오는 친구의 type 을 구해준다.
    now = param(next_type);
    if (now!=NULL) {
      if (first==NULL)
      {
        first = first_sibling_pointer = now;
        fprintf(stdout, "error has occured\n");
      }
      else
      { 
        /// 형제의 위치로 이동하는 상황
        printf("has sibling!\n");
        first_sibling_pointer->sibling = now;
        first_sibling_pointer = now;
      }
    }
  }
  return first;
}

TreeNode* param(ExpType type){
  TreeNode * ret;
  // parameter 함수 선언하고 // semicolon 있는지 확인
  char *name = copyString(tokenString);// id 이름 베끼고
  match(ID);
  printf("in param function!\n");
  // id 까지는 동일
  
  if (token == LBRACKET)
	{
		match(LBRACKET);
		match(RBRACKET);
		ret = newExpNode(ArrayDeclare);
	}
	else{
    fprintf(stdout, "var declare\n");
		ret = newExpNode(VarDelcare);
  }
	
  if (ret != NULL)
	{
		ret->attr.name = name;
		ret->type = type;
		ret->IsParameter = TRUE;
	}

  return ret;
}

TreeNode * compound_statement(void){
  // 일단 local declartion 개발 하자
  // 


}

TreeNode * local_declarations(void){

}

TreeNode * var_declarations(void){
  ExpType type = type_specifier();
	char* name = copyString(tokenString);
  TreeNode *ret;
  // id를 맞추고
	match(ID);
  printf("in function variance declaration\n");
  // id를 확인하면 다음 토큰은 
	
  if(token == SEMI){
    ret = newExpNode(VarDelcare);
		if (ret != NULL)
		{
			ret->attr.name = name;
			ret->type = type;
		}
  }else if(token == LBRACKET){
    ret = newExpNode(ArrayDeclare);
		if (ret != NULL)
		{
			ret->attr.name = name;
			ret->type = type;
		}
    //[
		match(LBRACKET);
    // number
    if (ret != NULL)
			ret->arraySize = atoi(tokenString);
    match(NUM);
    // ]
    match(RBRACKET);
    //;
  }else{
    printToken(token, tokenString);
		token = getToken();
    return NULL;
  }
  match(SEMI);
  return ret;
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
  if (t!=NULL) t->child[0] = expression();
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
  if (t!=NULL) t->child[1] = expression();
  return t;
}

TreeNode * assign_stmt(void)
{ TreeNode * t = newStmtNode(AssignK);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  match(ASSIGN);
  if (t!=NULL) t->child[0] = expression();
  return t;
}

TreeNode * expression(void)
{ TreeNode * t = simple_expression();
  if ((token==LT)||(token==EQ)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_expression();
  }
  return t;
}

TreeNode * simple_expression(void)
{ TreeNode * t = term();
  // additive expression 먼저 호출 이후에 

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
      t = expression();
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