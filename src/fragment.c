/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "fragment.h"
    
/*---------------------------------------------------------------------------*/

void frag_init
(frag_engine_t *engine, bool fragmenting,
 uint8_t *data, size_t data_size,
 uint32_t rule_id, size_t rule_id_bitsize
 )
{
    buffer_init(&engine->data, data, data_size);
    engine->rule_id = rule_id;
    engine->rule_id_bitsize = rule_id_bitsize;
    engine->ack_mode = SCHC_NO_ACK;
}

/*---------------------------------------------------------------------------*/

void frag_sender_prepare_noack(frag_engine_t *engine,
                               uint8_t *data,    size_t data_size,
                               uint32_t rule_id, size_t rule_id_bitsize,
                               uint32_t dtag,    size_t dtag_bitsize,
                               size_t max_frag_size)
{
    buffer_init(&engine->data, data, data_size);
    engine->ack_mode = SCHC_NO_ACK;
    engine->is_sender = true;

    assert(rule_id_bitsize < sizeof(uint32_t)*BITS_PER_BYTE);
    engine->rule_id = rule_id;
    engine->rule_id_bitsize = rule_id_bitsize;

    assert(dtag_bitsize < sizeof(uint32_t)*BITS_PER_BYTE);
    engine->dtag = dtag;
    engine->T = dtag_bitsize;
}

/*---------------------------------------------------------------------------*/

int frag_sender_generate(frag_engine_t *engine,
                         uint8_t *res_data, size_t res_data_max_size)
{
    assert(engine->is_sender);
 
    buffer_t packet;
    buffer_init(&packet, res_data, res_data_max_size);
    if (res_data_max_size < engine->max_frag_size) {
        DEBUG("fragment buffer too small %zu<%zu\n",
              res_data_max_size, engine->max_frag_size);
        return -1;
    }
    
    bit_buffer_t bit_packet;
    
}

/*---------------------------------------------------------------------------*/

