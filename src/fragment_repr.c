
#include <stdio.h>

#include "buffer.h"
#include "fragment.h"

void buffer_repr(buffer_t *buffer, FILE *out)
{
    fprintf(out, "{'type':'buffer'");
    fprintf(out, ", 'position':%u", buffer->position);    
    fprintf(out, ", 'capacity':%u", buffer->capacity);
    fprintf(out, ", 'has_bound_error':%u", buffer->has_bound_error);
    fprintf(out, "}");
}

void bit_buffer_repr(bit_buffer_t *bit_buffer, FILE *out)
{
    fprintf(out, "{'type':'bit_buffer'");
    fprintf(out, ", 'buffer':");
    buffer_repr(bit_buffer->buffer, out);
    fprintf(out, ", 'pending':%u", bit_buffer->pending);
    fprintf(out, ", 'pending_count':%zu", bit_buffer->pending_bit_count);
    fprintf(out, "}");
}

#if 0
void bit_buffer_repr(buffer_t *bit_buffer, FILE *out)
{
    fprintf(out, "{'type':'bit_buffer'");
    fprintf(out, ", 'buffer':");
    buffer_repr(bit_buffer->buffer);
    fprintf(out, ", 'pending':%u", buffer->pending);
    fprintf(out, ", 'pending_count':%u", buffer->pending_count);
    fprintf(out, "}");
}
#endif

#define CASE_REPR(x, out)                       \
    case (x):                                   \
        fprintf(out, #x);                      \
        break;

void ack_mode_repr(ack_mode_t ack_mode, FILE *out)
{
    switch (ack_mode) {
        CASE_REPR(SCHC_NO_ACK, out);
        CASE_REPR(SCHC_ACK_ALWAYS, out);
        CASE_REPR(SCHC_ACK_ON_ERROR, out);
    default:
        fprintf(out, "<unknown:%u>", ack_mode);
    }
}

void frag_engine_repr(frag_engine_t* engine, FILE *out)
{
    fprintf(out, "{'type':'frag_engine'");
    fprintf(out, ", 'is_sender':%u", engine->is_sender);
    fprintf(out, ", 'ack_mode':");
    ack_mode_repr(engine->ack_mode, out);
    fprintf(out, "}");
}
