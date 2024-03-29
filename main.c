/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE TRUE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE
#define WRITE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

main(int argc, char *argv[])
{
  TreeNode *syntaxTree;
  char pgm[120]; /* source code file name */
  char file_name[128];
  char *dot_loc;
  // 인수 개수 check
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }

  // c minus 문법으로 불러오는 파일
  strcpy(pgm, argv[1]);
  if (strchr(pgm, '.') == NULL)
    strcat(pgm, ".tny");
  source = fopen(pgm, "r");
  if (source == NULL)
  {
    fprintf(stderr, "File %s not found\n", pgm);
    exit(1);
  }
  // 써줄 파일 이름 찾기
  listing = stdout; /* send listing to screen */

  strcpy(file_name, pgm);

  if ((dot_loc = strchr(file_name, '.')) == NULL)
    strcat(file_name, "_20171628.txt");

  strcpy(dot_loc, "_20171628.txt");

  //fprintf(stdout, "file name is %s\n", file_name);

#if WRITE
  FILE* write_file = fopen(file_name, "w");
  listing = write_file; /* to file */
#endif

  // listing 을 고치면 파일에 쓰는게 가능하다.
  fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
  //fprintf(listing, "line number\t\t\ttoken\t\t\tlexeme\n");
  //fprintf(listing, "-------------------------------------------------------------------------------\n");
  // 위의 형식 맞춰주기
#if NO_PARSE
  while (getToken() != ENDFILE);
#else
  syntaxTree = parse();
  if (TraceParse)
  {
    //fprintf(stdout, "on Tracing\n");
    if(Error == TRUE)
      listing = fopen(file_name, "a");
    fprintf(listing, "\nSyntax tree:\n");
    if(Error == FALSE)
      printTree(syntaxTree);
  }
#if !NO_ANALYZE
  if (!Error)
  {
    if (TraceAnalyze)
      fprintf(listing, "\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nType Checking Finished\n");
  }
#if !NO_CODE
  if (!Error)
  {
    char *codefile;
    int fnlen = strcspn(pgm, ".");
    codefile = (char *)calloc(fnlen + 4, sizeof(char));
    strncpy(codefile, pgm, fnlen);
    strcat(codefile, ".tm");
    code = fopen(codefile, "w");
    if (code == NULL)
    {
      printf("Unable to open %s\n", codefile);
      exit(1);
    }
    codeGen(syntaxTree, codefile);
    fclose(code);
  }
#endif
#endif
#endif
  fclose(source);
#if WRITE
  fclose(write_file);
#endif
  return 0;
}
