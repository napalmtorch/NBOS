#include <lib/stdlib.h>
#include <kernel/core/kernel.h>

const char     PUNCT_CHARS[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
const char     HEX_VALUES[]  = "0123456789ABCDEF";
const char*    ITOA_STR      = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
const uint32_t LTOA_DIGITS   = 32;

void* malloc(size_t size)
{
    if (size < HEAP_LARGE.alignment) { return heap_allocate(&HEAP_SMALL, size, MEMTYPE_ALLOC); }
    return heap_allocate(&HEAP_LARGE, size, MEMTYPE_ALLOC);
}

void* tmalloc(size_t size, uint8_t type)
{
    if (size < HEAP_LARGE.alignment) { return heap_allocate(&HEAP_SMALL, size, type); }
    return heap_allocate(&HEAP_LARGE, size, type);
}

void free(void* ptr)
{
    if (ptr == NULL) { return; }
    if (!heap_free(&HEAP_LARGE, ptr)) { heap_free(&HEAP_SMALL, ptr); }
}

void freearray(void** ptr, uint32_t count)
{
    if (ptr == NULL) { return; }
    for (uint32_t i = 0; i < count; i++) { free(ptr[i]); }
    free(ptr);
}

int atoi(const char* str)
{
    return atol(str);
}

uint32_t atol(const char* str)
{
    int result = 0, sign = 0;
    uint32_t digit;
    while (*str == 0x20) { str += 1; }

    if (*str == '-') { sign = 1; str += 1; } 
    else 
    { 
        sign = 0;
        if (*str == '+') { str += 1; }
    }

    for (;; str += 1) 
    {
        digit = *str - '0';
        if (digit > 9) { break; }
        result = (10*result) + digit;
    }

    if (sign) { return -result; }
    return result;
}

uint32_t atox(const char* str)
{
    uint32_t num = 0;
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] >= 48 && str[i] <= 57) { num = (num << 4) | (str[i] - 48); }
        else if (str[i] == 'A' || str[i] == 'a') { num = (num << 4) | 0x0A; }
        else if (str[i] == 'B' || str[i] == 'b') { num = (num << 4) | 0x0B; }
        else if (str[i] == 'C' || str[i] == 'c') { num = (num << 4) | 0x0C; }
        else if (str[i] == 'D' || str[i] == 'd') { num = (num << 4) | 0x0D; }
        else if (str[i] == 'E' || str[i] == 'e') { num = (num << 4) | 0x0E; }
        else if (str[i] == 'F' || str[i] == 'f') { num = (num << 4) | 0x0F; }
        else { return 0 ; }
    }
    return num;
}

float atof(const char* str)
{
    return 0.0f;
}

int rand()
{
    return 0;
}

void exit(int code)
{

}

void swap(char *x, char *y) { char t = *x; *x = *y; *y = t; }

char* reverse(char *buffer, int i, int j)
{
    while (i < j) { swap(&buffer[i++], &buffer[j--]); }
    return buffer;
}

char* itoa(int value, char* buffer, int base)
{
    if (base < 2 || base > 32) { return buffer; }

    int i = 0, n = abs(value); 
    while (n)
    {
        int r = n % base;

        if (r >= 10) { buffer[i++] = 65 + (r - 10); }
        else { buffer[i++] = 48 + r; }
        n = n / base;
    }

    if (i == 0) { buffer[i++] = '0'; }
    if (value < 0 && base == 10) { buffer[i++] = '-'; }
    buffer[i] = '\0';
    return reverse(buffer, 0, i - 1);
}

void ultoa(unsigned long value, char* result, int base)
{
    unsigned char index;
    char buffer[LTOA_DIGITS];
    index = LTOA_DIGITS;
    do 
    {
        buffer[--index] = '0' + (value % base);
        if ( buffer[index] > '9') { buffer[index] += 'A' - ':'; }
        value /= base;
    } while (value != 0);

    do { *result++ = buffer[index++]; } while (index < LTOA_DIGITS);
    *result = 0;
}

char* ltoa(size_t num, char* str, int base)
{
    ultoa(num, str, base);
    return str;
}

char* strhex(uint32_t value, char* result, bool prefix, uint8_t bytes)
{
    char* orig = result;
    orig[0] = 0;
    if (prefix) { result[0] = '0'; result[1] = 'x'; result += 2; }
    if (bytes == 1)
    {
        *result = HEX_VALUES[(uint8_t)((value & 0xF0) >> 4)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0F))];
    }
    else if (bytes == 2)
    {
        *result = HEX_VALUES[(uint8_t)((value & 0xF000) >> 12)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0F00) >> 8)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x00F0) >> 4)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x000F))]; 
    }
    else if (bytes == 4)
    {
        *result = HEX_VALUES[(uint8_t)((value & 0xF0000000) >> 28)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0F000000) >> 24)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x00F00000) >> 20)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x000F0000) >> 16)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0000F000) >> 12)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x00000F00) >> 8)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x000000F0) >> 4)]; result++;
        *result = HEX_VALUES[(uint8_t)((value & 0x0000000F))];
    }
    return orig;
}

int system(char* command)
{
    return 0;
}