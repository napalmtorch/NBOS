#pragma once
#include <lib/stdint.h>

typedef struct
{
    char*    data;
    uint32_t size;
    int      position;
} stream_t;

stream_t stream_create(uint32_t size);
bool     stream_dispose(stream_t* stream);
bool     stream_seek(stream_t* stream, int pos);
char     stream_peek(stream_t* stream);
char     stream_peekat(stream_t* stream, int pos);
char     stream_next(stream_t* stream);
bool     stream_isdone(stream_t* stream);