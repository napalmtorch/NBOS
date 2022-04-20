#pragma once
#include <lib/stdint.h>
#include <lib/stddef.h>

void* malloc(size_t size);
void* tmalloc(size_t size, uint8_t type);
void free(void* ptr);
void freearray(void** ptr, uint32_t count);
int atoi(const char* str);
uint32_t atol(const char* str);
uint32_t atox(const char* str);
float atof(const char* str);
int rand();
void exit(int code);
char* itoa(int num, char* str, int base);
char* ltoa(size_t num, char* str, int base);
char* strhex(uint32_t value, char* result, bool prefix, uint8_t bytes);
int system(char* command);