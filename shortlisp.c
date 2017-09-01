#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

// use operator string to see which operator to perform
long eval_op(long x, char* op, long y)
{
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  return 0;
}

long eval(mpc_ast_t* t) {

  // if tagged as number return it directly
  if (strstr(t->tag, "number"))
  {
    return atoi(t->contents);
  }

  // the operator is always the second child
  char* op = t->children[1]->contents;

  // store the third child in 'x'
  long x = eval(t->children[2]);

  // iterate the remaining children and combining
  int i = 3;
  while (strstr(t->children[i]->tag, "expr"))
  {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

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
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  
  // undefine and delete parsers
  mpc_cleanup(4, Number, Operator, Expr, ShortLisp);

  return 0;
}
