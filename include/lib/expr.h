#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <lib/list.h>
#include <lib/stack.h>

typedef enum { ASSOC_LEFT, ASSOC_RIGHT, } ASSOC;

typedef struct
{
    char  op;
    int   value;
    ASSOC asso;
} PACKED precedence_asso_t;

static const precedence_asso_t EXPR_PRECEDENCES[] =
{
    (precedence_asso_t){ .op = '+', .value = 2, .asso = ASSOC_LEFT  },
    (precedence_asso_t){ .op = '-', .value = 2, .asso = ASSOC_LEFT  },
    (precedence_asso_t){ .op = '*', .value = 3, .asso = ASSOC_LEFT  },
    (precedence_asso_t){ .op = '/', .value = 3, .asso = ASSOC_LEFT  },
    (precedence_asso_t){ .op = '^', .value = 4, .asso = ASSOC_RIGHT },
};

int expr_execute(char* input);
int expr_evaluate(int result1, char opr, int result2);

ASSOC expr_association(char opr);
precedence_asso_t expr_getprec(char opr);
int expr_precedence(char opr1, char opr2);
int expr_typecast_id(char* input);
char expr_typecast_op(char* opr);

bool expr_isnoise(char* input);
bool expr_isid(char* input);
bool expr_isop(char opr);

bool expr_stack_pushc(stack_t* stk, char c);
bool expr_stack_push(stack_t* stk, char* str);
char* expr_stack_pop(stack_t* stk);