#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

int main(int argc, char** argv)
{
  // create and name rules
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* ShortLisp = mpc_new("shortlisp");

  // define rules
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number    : /-?[0-9]+/ ;                            \
      operator  : '+' | '-' | '*' | '/' ;                 \
      expr      : <number> | '(' <operator> <expr>+ ')' ; \
      shortlisp : /^/ <operator> <expr>+ /$/ ;            \
    ",
    Number, Operator, Expr, ShortLisp);

  // repl
  puts("ShortLisp Version 0.2");
  puts("Press Ctrl+c to Exit\n");

  while(1)
  {
    char* input = readline("shortlisp> ");
    add_history(input);

    // parse input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, ShortLisp, &r))
    {
      // on success print the ast
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      // otherwise print the error
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  
  // undefine and delete parsers
  mpc_cleanup(4, Number, Operator, Expr, ShortLisp);

  return 0;
}
