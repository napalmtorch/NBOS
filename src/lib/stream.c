#include <lib/stream.h>
#include <kernel/core/kernel.h>

stream_t stream_create(uint32_t size)
{
    stream_t stream = (stream_t){ .size = size, .position = 0 };
    stream.data = tmalloc(size, MEMTYPE_STRING);
    return stream;
}

bool stream_dispose(stream_t* stream)
{
    if (stream == NULL) { return false; }
    if (stream->data != NULL) { free(stream->data); }
    stream->data     = NULL;
    stream->size     = 0;
    stream->position = 0;
}

bool stream_seek(stream_t* stream, int pos)
{
    if (stream == NULL) { return false; }
    if (pos < 0 || pos >= stream->size) { return false; }
    stream->position = pos;
}

char stream_peek(stream_t* stream)
{
    if (stream == NULL) { return 0; }
    if (stream->position - 1 < 0) { return 0; }
    return stream->data[stream->position - 1];
}

char stream_peekat(stream_t* stream, int pos)
{
    if (stream == NULL) { return 0; }
    if (pos < 0 || pos >= stream->position) { return 0; }
    return stream->data[pos];
}

char stream_next(stream_t* stream)
{
    if (stream == NULL) { return 0; }
    if (stream_isdone(stream)) { return 0; }
    char c = stream->data[stream->position];
    stream->position++;
    return c;
}

bool stream_isdone(stream_t* stream)
{
    if (stream == NULL || stream->data == NULL) { return true; }
    if (stream->position >= stream->size) { return true; }
    if (stream->data[stream->position] == 0) { return true; }
    return false;
}