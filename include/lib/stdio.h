#pragma once
#include <lib/stdarg.h>
#include <lib/stdint.h>
#include <lib/stddef.h>

int vprintf(const char* fmt, va_list args);
int printf(const char* fmt, ...);
int scanf(const char* fmt, ...);