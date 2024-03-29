/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char *tokenString)
{
  switch (token)
  {
  case IF:
    fprintf(listing,
            "IF\t\t\t%s\n", tokenString);
    break;
  case ELSE:
    fprintf(listing,
            "ELSE\t\t\t%s\n", tokenString);
    break;
  case INT:
    fprintf(listing,
            "INT\t\t\t%s\n", tokenString);
    break;
  case RETURN:
    fprintf(listing,
            "RETURN\t\t\t%s\n", tokenString);
    break;
  case VOID:
    fprintf(listing,
            "VOID\t\t\t%s\n", tokenString);
    break;
  case WHILE:
    fprintf(listing,
            "WHILE\t\t\t%s\n", tokenString);
    break;
  case ID:
    fprintf(listing,
            "ID\t\t\t%s\n", tokenString);
    break;
  case NUM:
    fprintf(listing,
            "NUM\t\t\t%s\n", tokenString);
    break;
  case PLUS:
    fprintf(listing, "+\t\t\t+\n");
    break;
  case MINUS:
    fprintf(listing, "-\t\t\t-\n");
    break;
  case TIMES:
    fprintf(listing, "*\t\t\t*\n");
    break;
  case DIV:
    fprintf(listing, "/\t\t\t/\n");
    break;
  case LT:
    fprintf(listing, "<\t\t\t<\n");
    break;
  case LTE:
    fprintf(listing, "<=\t\t\t<=\n");
    break;
  case GT:
    fprintf(listing, ">\t\t\t>\n");
    break;
  case GTE:
    fprintf(listing, ">=\t\t\t>=\n");
    break;
  case EQ:
    fprintf(listing, "==\t\t\t==\n");
    break;
  case NEQ:
    fprintf(listing, "!=\t\t\t!=\n");
    break;
  case ASSIGN:
    fprintf(listing, "=\t\t\t=\n");
    break;
  case SEMI:
    fprintf(listing, ";\t\t\t;\n");
    break;
  case COMMA:
    fprintf(listing, ",\t\t\t,\n");
    break;
  case LPAREN:
    fprintf(listing, "(\t\t\t(\n");
    break;
  case RPAREN:
    fprintf(listing, ")\t\t\t)\n");
    break;
  case LCURL:
    fprintf(listing, "{\t\t\t{\n");
    break;
  case RCURL:
    fprintf(listing, "}\t\t\t}\n");
    break;
  case LBRACKET:
    fprintf(listing, "[\t\t\t[\n");
    break;
  case RBRACKET:
    fprintf(listing, "]\t\t\t]\n");
    break;
  case ENDFILE:
    fprintf(listing, "EOF\n");
    break;
  case COMMENT_ERROR:
    fprintf(listing,
            "ERROR\t\t\tComment Error\n");
    break;
  case ID_ERROR:
    fprintf(listing,
            "ERROR\t\t\tIdentifier Error\n");
    break;
  case ERROR:
    fprintf(listing,
            "ERROR: %s\n", tokenString);
    break;
  default: /* should never happen */
    fprintf(listing, "Unknown token: %d\n", token);
  }
}

/* Function StmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind)
{
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
  {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s)
{
  int n;
  char *t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = malloc(n);
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++){
    fprintf(listing, " ");
    fprintf(listing, " ");
  }
}

static void printbackSpaces(void)
{
  int i;
  for (i = 0; i < indentno; i++){
    //fseek(listing, -8, SEEK_END);
  }
}


char *printType(ExpType type)
{
  if (type == Integer){
    return "int";
  }else if (type == Void){
    return "void";
  }else{
    return "type error";
  }
}


/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree)
{
  int i;
  INDENT;
  //printf("in function printTree\n");
  while (tree != NULL)
  {
    printSpaces();
    //fprintf(listing,"tree is not null!\n");
    if (tree->nodekind == StmtK)
    {
      //fprintf(listing, "in stmk %d\n", tree->kind.stmt);
      switch (tree->kind.stmt)
      {
      case IfK:
        fprintf(listing, "If\n");
        break;
      case RepeatK:
        fprintf(listing, "Repeat\n");
        break;
      case ReadK:
        fprintf(listing, "Read: %s\n", tree->attr.name);
        break;
      case WriteK:
        fprintf(listing, "Write\n");
        break;
      case ReturnK:
        if(tree->child[0]==NULL)
					fprintf(listing, "Return nothing\n");
				else
					fprintf(listing, "Return variable\n");
				break;
      case SimpK:
        fprintf(listing, "Simple Expression\n");
        //fprintf(stdout, "Simple Expression\n");
        break;
      case CompoundK:
        fprintf(listing, "Compound Statement\n");
        break;
      case CallK:
        fprintf(listing, "Calling Function : %s\n", tree->attr.name);
        break;
      default:
        fprintf(listing, "Unknown statement kind\n");
        break;
      }
    }
    else if (tree->nodekind == ExpK)
    {
      //fprintf(listing,"in expk %d\n", tree->kind.exp);
      switch (tree->kind.exp)
      {
      case OpK:
        switch (tree->attr.op)
        {
        case PLUS:
          fprintf(listing, "Additive Expression\n");
          break;
        case MINUS:
          fprintf(listing, "Additive Expression\n");
          //fprintf(listing, "MINUS Expression\n");
          break;
        case TIMES:
          fprintf(listing, "Mulop Expression\n");
          break;
        case DIV:
          fprintf(listing, "Mulop Expression\n");
          break;
        default:
          break;
        }
        break;
      case ConstK:
        if(tree->arraySize){
          fprintf(listing, "Array Size : %d\n", tree->arraySize);  
        }else{
          fprintf(listing, "Constant : %d\n", tree->attr.val);
        }


        //fprintf(stdout, "Constant : %d\n", tree->attr.val);
        break;
      case IdK:
        fprintf(listing, "Variable : %s\n", tree->attr.name);
        //fprintf(stdout, "Variable : %s\n", tree->attr.name);
        break;
      case calK:
        switch (tree->attr.op)
        {
        case PLUS:
          fprintf(listing, "Operator : +\n");
          //fprintf(stdout, "Additive Expression\n");
          break;
        case MINUS:
          fprintf(listing, "Operator : -\n");
          break;
        case TIMES:
          fprintf(listing, "Operator : *\n");
          break;
        case DIV:
          fprintf(listing, "Operator : /\n");
          break;
        case LT:
          fprintf(listing, "Operator : <\n");
          break;
        case LTE:
          fprintf(listing, "Operator : <=\n");
          break;
        case GT:
          fprintf(listing, "Operator : >\n");
          break;
        case GTE:
          fprintf(listing, "Operator : >=\n");
          break;
        case EQ:
          fprintf(listing, "Operator : ==\n");
          break;
        case NEQ:
          fprintf(listing, "Operator : !=\n");
          break;
        default:
          fprintf(listing, "in valid operator\n");
          break;
        }
        break;
      case ArrayDeclare:
					if (tree->IsParameter){
            fprintf(listing, "Array Parameter : %s\n", tree->attr.name);
            printSpaces();
            fprintf(listing, "    Type : %s\n", printType(tree->type));
            //fprintf(stdout, "parameter Var Declare : %s, type : %s\n",tree->attr.name, printType(tree->type));
          }else{
            fprintf(listing, "Array Declare : %s\n", tree->attr.name);
            printSpaces();
            fprintf(listing, "    Type : %s\n", printType(tree->type));
            //fprintf(stdout, "Var Declare, name : %s, type : %s\n",tree->attr.name, printType(tree->type));
          }
          break;
      case VarDelcare:
          if (tree->IsParameter){
            fprintf(listing, "Parameter : %s\n", tree->attr.name);
            printSpaces();
            fprintf(listing, "    Type : %s\n", printType(tree->type));
            //fprintf(stdout, "parameter Var Declare : %s, type : %s\n",tree->attr.name, printType(tree->type));
          }else{
            fprintf(listing, "Variable Declare : %s\n", tree->attr.name);
            printSpaces();
            fprintf(listing, "    Type : %s\n", printType(tree->type));
            //fprintf(stdout, "Var Declare, name : %s, type : %s\n",tree->attr.name, printType(tree->type));
          }
				break;
      case FunctionDeclare:
				fprintf(listing, "Function Declare : %s\n",tree->attr.name);
        printSpaces();
        fprintf(listing, "    Type : %s\n", printType(tree->type));
				break;
      case AssignK:
        fprintf(listing, "Assign : =\n");
        break;
      default:
        fprintf(listing, "Unknown ExpNode kind\n");
        break;
      }
    }
    else
      fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    
    tree = tree->sibling;
  }
  UNINDENT;
}
