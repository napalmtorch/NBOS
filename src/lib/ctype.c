#include <lib/ctype.h>
#include <kernel/core/kernel.h>

int isalnum(int c)
{
    if (isalpha(c)) { return true; }
    if (isdigit(c)) { return true; }
    return false;
}

int isalpha(int c)
{
    if (c >= 'a' && c <= 'z') { return true; }
    if (c >= 'A' && c <= 'Z') { return true; }
    return false;
}

int iscntrl(int c)
{
    if (c < 32) { return true; }
    if (c == 127) { return true; }
    return false;
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9') { return true; }
    return false;
}

int isgraph(int c)
{
    if (isalnum(c)) { return true; }
    if (ispunct(c)) { return true; }
    return false;
}

int islower(int c)
{
    if (c >= 'a' && c <= 'z') { return true; }
    return false;
}

int isprint(int c)
{
    if (isgraph(c)) { return true; }
    if (c == 32)  { return true; }
}

int ispunct(int c)
{
    if (c >= 33 && c <= 47)   { return true; }
    if (c >= 58 && c <= 64)   { return true; }
    if (c >= 91 && c <= 96)   { return true; }
    if (c >= 123 && c <= 126) { return true; }
    return false;
}

int isspace(int c)
{
    if (c == 32) { return true; }
    if (c >= 12 && c <= 15) { return true; }
    return false;
}

int isupper(int c)
{
    if (c >= 'A' && c <= 'Z') { return true; }
    return false;
}

int isxdigit(int c)
{
    if (c >= '0' && c <= '9') { return true; }
    if (c >= 'A' && c <= 'F') { return true; }
    if (c >= 'a' && c <+ 'f') { return true; }
    return false;
}

int tolower(int c)
{
    if (!isupper(c)) { return c; }
    return c + 32;
}

int toupper(int c)
{
    if (!islower(c)) { return c; }
    return c - 32;
}