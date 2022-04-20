#include <lib/expr.h>
#include <kernel/core/kernel.h>

OPTIMIZE("O0") int expr_execute(char* input)
{
    int toks_count = 0;
    char** toks = strsplit(input, ' ', &toks_count);
    if (toks_count == 0 || toks == NULL) { return 0; }

    strstack_t inter = strstack_create();
    charstack_t opr  = charstack_create();

    for (int i = 0; i < toks_count; i++)
    {
        char* s = toks[i];
        if (expr_isnoise(s)) { continue; }

        char o = expr_typecast_op(s);
        if (expr_isop(o))
        {
            while (!stack_isdone(&opr))
            {
                char ot = opr.items[opr.position - 1];
                debug_info("OT: %c", ot);

                if (expr_isop(ot) && ((expr_association(o) == ASSOC_LEFT && expr_precedence(o, ot) <= 0) || (expr_association(o) == ASSOC_RIGHT && expr_precedence(o, ot) < 0)))
                { expr_stack_pushc(&inter, charstack_pop(&opr)); } else { break; }
            }
            charstack_push(&opr, o);
        }
        else if (!strcmp(s, "(")) { charstack_push(&opr, '('); }
        else if (!strcmp(s, ")"))
        {
            bool pe = false;
            while (!stack_isdone(&opr))
            {
                char sc = opr.items[opr.position - 1];
                debug_info("SC: %c", sc);
                if (sc == '(') { pe = true; break; }
                else { expr_stack_pushc(&inter, charstack_pop(&opr)); }
            }
            if (!pe) { debug_error("EXPR - Expected left parenthesis"); return 0; }
            charstack_pop(&opr);
        }
        else if (expr_isid(s)) { expr_stack_push(&inter, s); }
        else
        {
            if (!expr_isnoise(s)) { debug_error("EXPR - Unknown token '%s'", s); return 0; }
        }
    }

    while (!stack_isdone(&opr)) { expr_stack_pushc(&inter, charstack_pop(&opr)); }

    strqueue_t res = strqueue_create();
    res.items    = inter.items;
    res.position = inter.position;
    stack_t var = stack_create(sizeof(int));

    while (true)
    {
        char* o = strqueue_dequeue(&res);
        if (o == NULL) { break; }
        if (expr_isid(o)) { int num = expr_typecast_id(o); stack_push(&var, &num); }
        if (strlen(o) == 1 && expr_isop(o[0]))
        {
            void* rv = stack_pop(&var);
            void* lv = stack_pop(&var);
            int r = *(int*)rv;
            int l = *(int*)lv;
            int res = expr_evaluate(l, o[0], r);
            stack_push(&var, &res);
        }
        free(o);
    }

    int* vals = var.items;
    int num = vals[var.position - 1];

    stack_clear(&var);
    stack_clear(&inter);
    stack_clear(&opr);
    freearray(res.items, inter.position);
    return num;
}

int expr_evaluate(int result1, char opr, int result2)
{
    debug_info("EXPR: %d %c %d", result1, opr, result2);
    switch (opr)
    {
        case '+': { return result1 + result2; }
        case '-': { return result1 - result2; }
        case '*': { return result1 * result2; }
        case '/': { return result1 / result2; }
        case '^': { return (int)pow((double)result1, result2); }
    }
    debug_error("EXPR - Unable to evaluate expression %d %c %d", result1, opr, result2);
    return 0;
}

ASSOC expr_association(char opr)
{
    precedence_asso_t o = expr_getprec(opr);
    if (o.op == 0) { debug_error("EXPR - Invalid operator association"); return ASSOC_LEFT; }
    debug_info("RET ASSOC %d", o.asso);
    return o.asso;
}

precedence_asso_t expr_getprec(char opr)
{
    if (opr == 0) { return (precedence_asso_t){ .op = 0 }; }
    const int count = sizeof(EXPR_PRECEDENCES) / sizeof(precedence_asso_t);
    for (int i = 0; i < count; i++)
    {
        if (EXPR_PRECEDENCES[i].op == opr) { return EXPR_PRECEDENCES[i]; }
    }
    return (precedence_asso_t){ .op = 0 };
}

int expr_precedence(char opr1, char opr2)
{
    if (!expr_isop(opr1) || !expr_isop(opr2)) { debug_error("EXPR - Invalid operators %c and %c", opr1, opr2); return 0; }
    precedence_asso_t o1 = expr_getprec(opr1);
    precedence_asso_t o2 = expr_getprec(opr2);
    if (o1.op == 0 || o2.op == 0) { debug_error("EXPR - Failed to get precedence for operators %c and %c", opr1, opr2); return 0; }
    if (o1.value > o2.value) { return 1; }
    if (o1.value < o2.value) { return -1; }
    return 0;
}


int expr_typecast_id(char* input)
{
    int result = 0;
    if (!expr_isid(input)) { return 0; }
    return atoi(input);
}

char expr_typecast_op(char* opr)
{
    if (opr == NULL || strlen(opr) == 0) { return 0; }
    switch (opr[0])
    {
        case '+': { return '+'; }
        case '-': { return '-'; }
        case '*': { return '*'; }
        case '/': { return '/'; }
        case '^': { return '^'; }
    }
    return 0;
}

bool expr_isnoise(char* input)
{
    return false;
}

bool expr_isid(char* input)
{
    for (uint32_t i = 0; i < strlen(input); i++)
    {
        if (!isdigit(input[i])) { return false; }
    }
    return true;
}

bool expr_isop(char opr)
{
    switch (opr)
    {
        case '+': { return true; }
        case '-': { return true; }
        case '*': { return true; }
        case '/': { return true; }
        case '^': { return true; }
    }
    return false;
}

bool expr_stack_pushc(stack_t* stk, char c)
{
    char* push = tmalloc(4, MEMTYPE_STRING);
    push[0] = c;
    push[1] = 0;
    strstack_push(stk, push);
    return true;
}

bool expr_stack_push(stack_t* stk, char* str)
{
    char* push = tmalloc(strlen(str), MEMTYPE_STRING);
    strcpy(push, str);
    strstack_push(stk, push);
    return true;
}

char* expr_stack_pop(stack_t* stk)
{
    return strstack_pop(stk);
}