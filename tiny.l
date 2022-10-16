/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"          {return VOID;}
"while"         {return WHILE;}
{identifier}    {return ID;}
{number}        {return NUM;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return DIV;}
"<"             {return LT;}
"<="             {return LTE;}
">"             {return GT;}
">="             {return GTE;}
"=="             {return EQ;}
"!="             {return NEQ;}
"="            {return ASSIGN;}
";"             {return SEMI;}
","             {return COMMA;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"{"             {return LCURL;}
"}"             {return RCURL;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"             { char c; int state = 0; // 0이면 비활성화
                  do
                  { c = input();
                    //fprintf(stdout, "%c\n", c);
                    //fprintf(stdout, "%d\n", c);
                    if (c == EOF) break;
                    if (c == 0) break;
                    if (c == '\n') lineno++;
                    if (c == '*') state = 1;
                    else if (state == 1 && c == '/') state = 1;
                    // 전에 state가 1인 상태에서 / 가 나오게 되면 state 는 1로 고정
                    else state = 0;
                    //usleep(50000);
                  } while (state != 1 || c != '/');
                }
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d\t\t\t",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

int yywrap()
{
        /* actions for reaching EOF */
}

void yyerror(char const *msg)
{
        strcpy(yytext, msg);
}