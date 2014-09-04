#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* copy = malloc(strlen(buffer)+1);
  strcpy(copy, buffer);
  copy[strlen(copy)-1] = '\0';
  return copy;
}

void add_history(char* unused) {}

#else

#include <editline/readline.h>
#include <editline/history.h>

#endif

int main(int argc, char** argv) {
  
  // Create Some Parsers
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  
  // Define them with the following Language
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' ;                  \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);
  
  puts("LearnLisp Version 0.1");
  puts("Press Ctrl+C to Exit\n");
  
  while (1) {
  
    char* input = readline("LearnLisp> ");
    add_history(input);
    
    // Attempt to parse the user input
    mpc_result_t result;
    if (mpc_parse("<stdin>", input, Lispy, &result)) {
      // On success print and delete the AST
      mpc_ast_print(result.output);
      mpc_ast_delete(result.output);
    } else {
      // Otherwise print and delete the Error
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }
    
    free(input);
  }
  
  // Undefine and delete our parsers
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}