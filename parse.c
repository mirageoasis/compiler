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

#define DEBUG

#ifdef DEBUG
  #define DTOKEN(token) fprintf(stdout, "currnet token is %d\n", token);
#else
  #define DTOKEN
#endif

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
args 쪽 보기
call 함수 구현하기
factor 에서 call 함수 구현하는게 현재 todo 이다.

*/
static TreeNode * declaration_list(void);
static TreeNode * params(void);
static TreeNode * param_list(ExpType);
static TreeNode * param(ExpType);

static TreeNode * compound_stmt(void);
static TreeNode * return_stmt(void);
static TreeNode * var_declarations(void);
static TreeNode * local_declarations(void);
static TreeNode * statement_list(void);
static TreeNode * expression_stmt(void);


static TreeNode * var(char *);

static TreeNode * additive_expression(TreeNode *);

static TreeNode * declaration(void);
static TreeNode * statement(void);
static TreeNode * selection_stmt(void);
static TreeNode * iteration_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * expression(void);
static TreeNode * simple_expression(TreeNode *);
static TreeNode * term(TreeNode *t);
static TreeNode * factor(TreeNode *t);
static TreeNode * addop(TokenType token);
static TreeNode * mulop(TokenType token);
static TreeNode * relop(TokenType token);

static TreeNode * call(void);
static TreeNode * args(void);
static TreeNode * args_list(void);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(stdout, "print error occured!\n");
  fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing, "\nSyntax tree:\n");

  fclose(listing);
  exit(-1);
  Error = TRUE;
}

static void match(TokenType expected)
{ 
  char msg[100];
  if (token == expected){
    token = getToken();
  }
  else {
    // 여기가 1번 예제 걸리는 곳
    sprintf(msg ,"unexpected token -> %s", tokenString);
    fprintf(stdout, "unexpected token -> %s", tokenString);
    syntaxError(msg);
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
      printf("not null\n");
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

TreeNode* array_size(TreeNode *t){
  
  if (t == NULL)
    return NULL;
	
  TreeNode *ret = newExpNode(ConstK);
  ret->arraySize = atoi(tokenString);
  
  match(NUM);
  
  return ret;
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
    t->child[0] = array_size(t);
    // ]
    match(RBRACKET);
    //;
    match(SEMI);
  }
  else if(token == LPAREN){
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

    if (t != NULL)
			t->child[1] = compound_stmt();
    

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
  //printf("in params function\n");
  type = type_specifier();
  // 
  if (type == Void && token == RPAREN){
    // parameter 가 없는 케이스
    //printf("no parameter!\n");
    //ret = newExpNode(VarDelcare);
    //ret->IsParameter = TRUE;
    //ret->type = Void;
    ret = NULL;
  }else{
    //printf("yes parameter!\n");
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
  //printf("in param_list function!\n");
  first = param(type);
  first_sibling_pointer = first;
  while(token == COMMA){
    match(COMMA);
    ExpType next_type = type_specifier(); // 다음 나오는 친구의 type 을 구해준다.
    now = param(next_type);
    if (now!=NULL) {
      if (first==NULL)
      {
        first = first_sibling_pointer = now;
        //fprintf(stdout, "error has occured\n");
      }
      else
      { 
        /// 형제의 위치로 이동하는 상황
        //printf("has sibling!\n");
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
  //printf("in param function!\n");
  // id 까지는 동일
  
  if (token == LBRACKET)
	{
		match(LBRACKET);
		match(RBRACKET);
		ret = newExpNode(ArrayDeclare);
	}
	else{
    //fprintf(stdout, "var declare\n");
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

TreeNode * compound_stmt(void){
  // 일단 local declartion 개발 하자
  // 
  TreeNode *ret = newStmtNode(CompoundK);
  // 괄호 맞춰주기
  fprintf(stdout,"in compound statement function!\n");
  match(LCURL);
  ret->child[0] = local_declarations();
  ret->child[1] = statement_list();
  
  match(RCURL);
  // 괄호 맞춰주기
  return ret;
}

TreeNode *local_declarations(void)
{
  // declartion list와 비슷할꺼라고 예상
  // empty 를 잘 처리해야하는데
  TreeNode *ret = NULL;
  TreeNode *ret_sibling_pointer = NULL;
  //fprintf(stdout, "token type is! %d\n", token);
  if (token == INT || token == VOID)
    ret = var_declarations();
  ret_sibling_pointer = ret;
  if (ret != NULL)
  {
    while (token == INT || token == VOID)
    {
      TreeNode *q;
      q = var_declarations();
      if (q == NULL)
        break;

      if (ret == NULL)
        ret = ret_sibling_pointer = q;
      else /* now p cannot be NULL either */
      {
        ret_sibling_pointer->sibling = q;
        ret_sibling_pointer = q;
      }
    }
  }
  return ret;
}

TreeNode * var_declarations(void){
  ExpType type = type_specifier();
	char* name = copyString(tokenString);
  TreeNode *ret;
  // id를 맞추고
	match(ID);
  //printf("in function variance declaration\n");
  // id를 확인하면 다음 토큰은 
	
  if(token == SEMI){
    ret = newExpNode(VarDelcare);
		if (ret != NULL)
		{
			ret->attr.name = name;
			ret->type = type;
		}
    //fprintf(stdout, "변수 선언\n");
    match(SEMI);
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
    match(SEMI);
    fprintf(stdout, "배열 선언\n");
    //;
  }else{
    printToken(token, tokenString);
    fprintf(stdout, "error while var_declarations!\n");
		token = getToken();
    return NULL;
  }
  
  return ret;
}

TreeNode * statement_list(void)
{ 
  TreeNode *ret = NULL;
  TreeNode *ret_sibling_pointer = NULL;
  // TODO 
  // 완성한거 따라서 if 문에 token 추가하기


	if (token == RCURL)
		return NULL;
	ret = statement();
	ret_sibling_pointer = ret;
	while (token != RCURL)
	{
		TreeNode * q;
		q = statement();
		if (q != NULL) {
			if (ret == NULL) ret = ret_sibling_pointer = q;
			else /* now p cannot be NULL either */
			{
				ret_sibling_pointer->sibling = q;
				ret_sibling_pointer = q;
			}
		}
	}
	
	return ret;
}

TreeNode * statement(void)
{ 

  fprintf(stdout, "in statement function\n"); 
  fprintf(stdout, "%s\n", tokenString);  
  DTOKEN(token)
  TreeNode * t = NULL;
  char msg[100];
  switch (token) {
    case ID :
      t = expression_stmt(); 
      break;
    case LPAREN:
      t = expression_stmt(); 
      break;
	  case NUM:
      t = expression_stmt(); 
      break;
	  case SEMI: 
      t = expression_stmt(); 
      break;
    case LCURL:
      t = compound_stmt();
      break;
    case IF : 
      t = selection_stmt(); 
      break;
    case WHILE : 
      t = iteration_stmt(); 
      break;
    case RETURN: 
      match(RETURN);
      t =  return_stmt();
      break;
    
    default : 
              sprintf(msg ,"unexpected token -> %s", tokenString);
              syntaxError(msg);
              printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}

TreeNode * expression_stmt(void){
  TreeNode *ret = NULL;
  fprintf(stdout, "in expression statement function!\n");
	if (token == SEMI)
		match(SEMI);
	else if (token != RCURL)
	{
		ret = expression();
		match(SEMI);
	}
	return ret;
}

TreeNode * return_stmt(void)
{ 
  TreeNode * t = newStmtNode(ReturnK);
  // expression
  if (token != SEMI){
    t->child[0] = expression();
  }

  // 세미콜론 ; 
  match(SEMI);
  return t;
}

TreeNode * selection_stmt(void)
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  match(LPAREN);
  if (t!=NULL) t->child[0] = expression();
  match(RPAREN);
  if (t!=NULL) t->child[1] = statement();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = statement();
  }
  return t;
}

TreeNode * iteration_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  fprintf(stdout, "in iteration stmt\n");
  // while (
  match(WHILE);
  match(LPAREN);
  // expression
  if (t!=NULL) 
    t->child[0] = expression();
  // )
  match(RPAREN);
  if (t!=NULL) 
    t->child[1] = statement();
  //match(UNTIL);
  return t;
}

TreeNode *expression(void)
{
  TreeNode *ret;
  TreeNode *temp = NULL;
  // 일단 var = expresssion 을 만든다.
  fprintf(stdout,"in expression function\n");
  fprintf(stdout,"current token %d\n", token);
  TreeNode *t;
	TreeNode *q=NULL;
	int flag = FALSE;

	if (token == ID)
	{
		// call 함수도 모두 고쳐야한다.
      q = call();
		  flag = TRUE;
    // ASSIGN 이 아닌 경우 문제가 된다.
	}

	if (flag == TRUE && token == ASSIGN)
	{
		if (q != NULL && q->nodekind == ExpK && q->kind.exp == IdK)
		{
			match(ASSIGN);
			t = newExpNode(AssignK);
			if (t != NULL)
			{
				t->child[0] = q;
				t->child[1] = expression();
			}
		}
		else
		{
			syntaxError("attempt to assign to something not an lvalue\n");
			token = getToken();
		}
	}
	else
		t = simple_expression(q);
	return t;

  return ret;
}



TreeNode * simple_expression(TreeNode *f){
  fprintf(stdout,"in function simple_expression\n");
  TreeNode * ret = additive_expression(f);

  if (ret == NULL){
    fprintf(stdout,"null\n");
  }

  int flag = FALSE;

  TokenType temp[6] = {LT ,LTE ,GT ,GTE ,EQ ,NEQ};
  
  for(int i = 0; i < 6; i++){
    if(token == temp[i])
      flag = TRUE;
  }

  if(flag){
    // 식이 여러개 있는 경우
    TokenType prev_token = token;
    TreeNode * back; // 만약에 뒤로 밀리면 이거로
    match(token); // 현재 token 소모
    back = ret; // back 이 ret 의 연산자를 받는다.
    ret = newStmtNode(SimpK);
    if (ret != NULL){
      ret->child[0] = back;
      // operator 추가해야할꺼 같음
      ret->child[1] = relop(prev_token);
      ret->child[2] = additive_expression(NULL);
      ret->attr.op = prev_token;
    }
  }

  return ret;
}

TreeNode * var(char * name)
{ 
  TreeNode * ret;
  fprintf(stdout, "in function var!\n");
  DTOKEN(token)
  // 여기는 이름을 할당해주지 못한다.

	if(token==LBRACKET)
	{
		ret = newExpNode(IdK);
		if (ret != NULL)
		{
			ret->attr.name = name;
			ret->type = Integer;
			match(LBRACKET);
			ret->child[0] = expression();
			match(RBRACKET);
		}
	}
	else
	{
    //fprintf(stdout,"current name is %s\n", name);
		ret = newExpNode(IdK);
		if (ret != NULL)
		{
			ret->attr.name = name;
			ret->type = Integer;
		}
	}
	return ret;
}

TreeNode *additive_expression(TreeNode *f)
{
  TreeNode *t = term(f);
  fprintf(stdout,"in function additive expression\n");
  // additive expression 먼저 호출 이후에
  // 이거 수정 필요함
  if (t != NULL)
  {
    while ((token == PLUS) || (token == MINUS))
    {
      TreeNode *p = newExpNode(OpK);
      if (p != NULL)
      {
        p->child[0] = t;
        p->attr.op = token;
        t = p;
        t->child[1] = addop(token);
        match(token);
        t->child[2] = term(NULL);
      }
    }
  }
  return t;
}

TreeNode * addop(TokenType token){
  TreeNode * t = newExpNode(calK);
  t->attr.op = token;

  return t;
}

TreeNode * mulop(TokenType token){
  TreeNode * t = newExpNode(calK);
  t->attr.op = token;

  return t;
}

TreeNode * relop(TokenType token){
  TreeNode * t = newExpNode(calK);
  t->attr.op = token;

  return t;
}


TreeNode *term(TreeNode *f)
{
  TreeNode *t = factor(f);
  if (t != NULL)
  {
    while ((token == TIMES || token == DIV))
    {
      TreeNode *p = newExpNode(OpK);
      if (p != NULL)
      {
        p->child[0] = t;
        p->attr.op = token;
        t = p;
        match(token);
        p->child[1] = factor(NULL);
      }
    }
  }
  return t;
}

TreeNode *factor(TreeNode *f)
{
  fprintf(stdout,"in function factor!\n");
  // call 구현 안함
  char msg[100];
  DTOKEN(token)
  if (f != NULL)
    return f;
  fprintf(stdout,"f is not null!\n");
  TreeNode *t = NULL;
  char *str;
  switch (token)
  {
  case NUM:
    t = newExpNode(ConstK);
    if ((t != NULL) && (token == NUM)){
      t->attr.val = atoi(tokenString);
      t->type = Integer;
    }
    match(NUM);
    break;
  case ID:
  fprintf(stdout,"before var and call token is %d\n", token);
    if(token == ID)
      str = copyString(tokenString);
    
    // 토큰 matching 하고 
    t = call();
    break;
  case LPAREN:
    match(LPAREN);
    t = expression();
    match(RPAREN);
    break;
  default:
    printf("unexpected token%d\n", token);
    sprintf(msg ,"unexpected token -> %s", tokenString);
    syntaxError(msg);
    printToken(token, tokenString);
    token = getToken();
    break;
  }
  return t;
}

TreeNode * call(void)
{
	TreeNode *t;
	char *name;

	if(token==ID)
		name = copyString(tokenString);
	match(ID);

	if (token == LPAREN)
	{
		match(LPAREN);
		t = newStmtNode(CallK);
		if (t != NULL)
		{
			t->attr.name = name;
			t->child[0] = args();
		}
		match(RPAREN);
	}
	else if(token==LBRACKET)
	{
		t = newExpNode(IdK);
		if (t != NULL)
		{
			t->attr.name = name;
			t->type = Integer;
			match(LBRACKET);
			t->child[0] = expression();
			match(RBRACKET);
		}
	}
	else
	{
		t = newExpNode(IdK);
		if (t != NULL)
		{
			t->attr.name = name;
			t->type = Integer;
		}
	}
	return t;
}


TreeNode * args(void)
{
	if (token == RPAREN)
		return NULL;
	else
		return args_list();
}

TreeNode * args_list(void)
{
	TreeNode * t;
	TreeNode * p;

	t = expression();
	p = t;
	if (t != NULL)
	{
		while (token == COMMA)
		{
			match(COMMA);
			TreeNode * q = expression();
			if (q != NULL) {
				if (t == NULL) t = p = q;
				else /* now p cannot be NULL either */
				{
					p->sibling = q;
					p = q;
				}
			}
		}
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
