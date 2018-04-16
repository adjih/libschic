/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <stdio.h>

#include "buffer.h"
#include "fragment.h"

void buffer_repr(buffer_t *buffer, FILE *out)
{
    fprintf(out, "{'type':'buffer'");
    fprintf(out, ", 'position':%u", buffer->position);    
    fprintf(out, ", 'capacity':%u", buffer->capacity);
    fprintf(out, ", 'has_bound_error':%u", buffer->has_bound_error);
    fprintf(out, ", 'content':'");
    for (size_t i=0; i<buffer->capacity; i++) {
        if (i == buffer->position) {
            fprintf(out, "|");
        }
        fprintf(out, "%02x", buffer->data[i]);
    }
    fprintf(out, "'");
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
    fprintf(out, ", 'ack_mode':'");
    ack_mode_repr(engine->ack_mode, out);
    fprintf(out, "'");
    fprintf(out, ", 'rule_id':%u", engine->rule_id);
    fprintf(out, ", 'rule_id_bitsize':%u", engine->rule_id_bitsize);
    fprintf(out, ", 'dtag':%u", engine->dtag);
    fprintf(out, ", 'R':%u", engine->R);
    fprintf(out, ", 'T':%u", engine->T);
    fprintf(out, ", 'W':%u", engine->W);
    fprintf(out, ", 'M':%u", engine->M);
    fprintf(out, ", 'frag_index':%u", engine->frag_index);
    fprintf(out, ", 'frag_count':%u", engine->frag_count);
    fprintf(out, ", 'frag_size':%zu", engine->frag_size);
    fprintf(out, ", 'bit_buffer':");
    bit_buffer_repr(&engine->bit_data, out);
    //fprintf(out, ", 'buffer':");
    //buffer_repr(&engine->data, out);
    fprintf(out, "}");
}

/*---------------------------------------------------------------------------*/
