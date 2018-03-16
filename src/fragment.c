/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "fragment.h"

#include "fragment_repr.c"

/*---------------------------------------------------------------------------*/

void frag_engine_init(frag_engine_t *engine,
                      uint8_t *data,    size_t max_data_size,
                      uint32_t rule_id, size_t rule_id_bitsize,
                      uint32_t dtag,    size_t dtag_bitsize)
{
    buffer_init(&engine->data, data, max_data_size);
    bit_buffer_init(&engine->bit_data, &engine->data);
    memset(engine->mic, 0, MIC_SIZE);

    assert(rule_id_bitsize < sizeof(uint32_t)*BITS_PER_BYTE);
    engine->rule_id = rule_id;
    engine->rule_id_bitsize = rule_id_bitsize;

    assert(dtag_bitsize < sizeof(uint32_t)*BITS_PER_BYTE);
    engine->dtag = dtag;
    engine->T = dtag_bitsize;    
}

/*---------------------------------------------------------------------------*/

void frag_sender_prepare_noack(frag_engine_t *engine, size_t max_frag_size)
{
    engine->ack_mode = SCHC_NO_ACK;
    engine->is_sender = true;
    compute_mic(engine->data.data, engine->data.capacity,
                engine->mic, MIC_SIZE);
    engine->frag_size = max_frag_size;    

    /*&
      1349    <------------ R ----------->
      1350                 <--T--> <--N-->
      1351     +-- ... --+- ...  -+- ... -+--------...-------+
      1352     | Rule ID |  DTag  |  FCN  | Fragment payload |
      1353     +-- ... --+- ...  -+- ... -+--------...-------+
    */
    (void)engine->T; /* already set in frag_engine_init */
    engine->N = 1;
    engine->R = engine->rule_id_bitsize + engine->T + engine->N;

    /* We need k | kS - (kR + MIC) >= data_size
       e.g. k (S-R) >= data_size+MIC
     */
    assert(BITS_PER_BYTE*max_frag_size > engine->R);
    size_t frag_payload_bitsize = (BITS_PER_BYTE*max_frag_size - engine->R);
    size_t payload_bitsize = BITS_PER_BYTE*engine->data.capacity + MIC_BITSIZE;

    size_t rounding = frag_payload_bitsize-1;
    engine->frag_count = (payload_bitsize + rounding) / frag_payload_bitsize;
    engine->frag_index = 0;

}

/*---------------------------------------------------------------------------*/

int frag_engine_generate_noack(frag_engine_t *engine, bit_buffer_t* bit_buffer)
{
    assert(engine->frag_index <= engine->frag_count);
    if (engine->frag_count == 0) {
        /* Normal operation, "EOF" */
        return 0;
    }    
    if (bit_buffer->buffer->has_bound_error) {
        return -1;
    }
    
    bool is_last = (engine->frag_index == engine->frag_count-1);
    size_t remaining_bitsize
        = bit_buffer_get_available_bitsize(&engine->bit_data);
    size_t available_bitsize
        = bit_buffer_get_available_bitsize(bit_buffer);
    if (available_bitsize < engine->R) {
        DEBUG("bit_buffer too small %zu < %u\n", available_bitsize, engine->R);
        return -1;
    }
    
    /*&
      1349    <------------ R ----------->
      1350                 <--T--> <--N-->
      1351     +-- ... --+- ...  -+- ... -+--------...-------+
      1352     | Rule ID |  DTag  |  FCN  | Fragment payload |
      1353     +-- ... --+- ...  -+- ... -+--------...-------+
    */    
    bit_buffer_put_several(bit_buffer, engine->rule_id,
                           engine->rule_id_bitsize);
    bit_buffer_put_several(bit_buffer, engine->dtag, engine->T);
    if (is_last) {
        bit_buffer_put_bit(bit_buffer, 1);
    }
    else {
        bit_buffer_put_bit(bit_buffer, 0);
    }
    assert(engine->N == 1);
    assert(engine->rule_id_bitsize + engine->T + engine->N == engine->R);
    available_bitsize -= engine->R; /* always > 0, checked before */

    assert(engine->M == MIC_BITSIZE);
    if (available_bitsize < remaining_bitsize + engine->M) {
        assert(!is_last);
        bit_buffer_copy_several(bit_buffer, &engine->bit_data,
                                remaining_bitsize);
    } else {
        assert(is_last);
        /*
          1405    <------------ R ----------->
          1406                  <- T -> <N=1> <---- M ---->
          1407    +---- ... ---+- ... -+-----+---- ... ----+---...---+
          1408    |   Rule ID  | DTag  |  1  |     MIC     | payload |
          1409    +---- ... ---+- ... -+-----+---- ... ----+---...---+
        */
        bit_buffer_copy_several(bit_buffer, &engine->bit_data,
                                remaining_bitsize);
        bit_buffer_put_data(bit_buffer, engine->mic, engine->M);
    }
    assert(!bit_buffer->buffer->has_bound_error); /* because we checked size */
    assert(!bit_buffer->buffer->has_bound_error); /* because we checked size */
    engine->frag_index++;

    bit_buffer_add_padding(bit_buffer);
    if (bit_buffer->buffer->has_bound_error) {
        return -1;
    }
    
    return bit_buffer->buffer->position; /* in bytes */
}

/*
  Return -1 in case of error, 0 if there is nothing to send, etc.
*/
int frag_engine_generate(frag_engine_t *engine,
                         uint8_t *res_data, size_t res_data_max_size)
{
    assert(engine->is_sender);
 
    if (res_data_max_size < engine->frag_size) {
        DEBUG("fragment buffer too small %zu<%zu\n",
              res_data_max_size, engine->frag_size);
        return -1;
    }

    buffer_t buffer;
    buffer_init(&buffer, res_data, engine->frag_size);
    bit_buffer_t bit_buffer;
    bit_buffer_init(&bit_buffer, &buffer);
    
    if (engine->ack_mode == SCHC_NO_ACK) {
        return frag_engine_generate_noack(engine, &bit_buffer);
    }
    else {
        DEBUG("ack_mode not implemented %u\n", engine->ack_mode);
        return -1;
    }
}

/*---------------------------------------------------------------------------*/

void frag_receiver_prepare_noack(frag_engine_t *engine)
{
    engine->ack_mode = SCHC_NO_ACK;
    engine->is_sender = false;

    /*&
      1349    <------------ R ----------->
      1350                 <--T--> <--N-->
      1351     +-- ... --+- ...  -+- ... -+--------...-------+
      1352     | Rule ID |  DTag  |  FCN  | Fragment payload |
      1353     +-- ... --+- ...  -+- ... -+--------...-------+
    */
    (void)engine->T; /* already set in frag_engine_init */
    engine->N = 1;
    engine->R = engine->rule_id_bitsize + engine->T + engine->N;

    engine->frag_count = 0; /* no meaning */
    engine->frag_index = 0; 
#if 0    
    /* We need k | kS - (kR + MIC) >= data_size 
       e.g. k (S-R) >= data_size+MIC
     */
    assert(BITS_PER_BYTE*max_frag_size > engine->R);
    size_t frag_payload_bitsize = (BITS_PER_BYTE*max_frag_size - engine->R);
    size_t payload_bitsize = BITS_PER_BYTE*data_size + MIC_BITSIZE;

    size_t rounding = frag_payload_bitsize-1;
    engine->frag_count = (payload_bitsize + rounding) / frag_payload_bitsize;
    engine->frag_index = 0;
#endif    
}



/*
  Return -1 in case of error, 0 if there is nothing to send, etc.
*/
int frag_engine_process_noack(frag_engine_t *engine, bit_buffer_t *bit_buffer)
{
    assert(engine->ack_mode == SCHC_NO_ACK);
    
    if (bit_buffer_get_available_bitsize(bit_buffer) < engine->R) {
        DEBUG("bit_buffer content too small %u %u.\n",
              bit_buffer_get_available_bitsize(bit_buffer), engine->R);
        return -1;
    }
    /*&
      1349    <------------ R ----------->
      1350                 <--T--> <--N-->
      1351     +-- ... --+- ...  -+- ... -+--------...-------+
      1352     | Rule ID |  DTag  |  FCN  | Fragment payload |
      1353     +-- ... --+- ...  -+- ... -+--------...-------+
    */
    assert(engine->rule_id_bitsize <= sizeof(uint32_t));
    uint32_t rule_id = bit_buffer_get_several(bit_buffer,
                                              engine->rule_id_bitsize);
    assert(engine->rule_id == rule_id);
    assert(engine->T <= sizeof(uint32_t));
    bit_buffer_get_several(bit_buffer, engine->T);
    
}

/*
  Return -1 in case of error, 0 if there is nothing to send, etc.
*/
int frag_engine_process(frag_engine_t *engine,
                        uint8_t *res_data, size_t res_data_max_size)
{
    assert(!engine->is_sender);
 
    buffer_t buffer;
    buffer_init(&buffer, res_data, engine->frag_size);
    bit_buffer_t bit_buffer;
    bit_buffer_init(&bit_buffer, &buffer);
    
    if (engine->ack_mode == SCHC_NO_ACK) {
        return frag_engine_process_noack(engine, &bit_buffer);
    }
    else {
        DEBUG("ack_mode not implemented %u\n", engine->ack_mode);
        return -1;
    }
}

/*---------------------------------------------------------------------------*/

// XXX: use system crc32 instead:
#include "simple_crc32.c"

void compute_mic(uint8_t* data, size_t data_size,
                 uint8_t* result, size_t result_max_size)    
{
    assert(result_max_size >= MIC_BITSIZE);
    assert(MIC_BITSIZE >= sizeof(uint32_t)); 
    uint32_t data_crc = 0;
    basic_crc32(data, data_size, &data_crc);
    memset(result, 0, result_max_size);    
    memcpy(result, &data_crc, sizeof(data_crc));
}

/*---------------------------------------------------------------------------*/

/*

1337    In the No-ACK mode, SCHC fragments except the last one SHALL conform
1338    to the detailed format defined in Figure 10.  The total size of the
1339    fragment header is R bits.
1340
1349    <------------ R ----------->
1350                 <--T--> <--N-->
1351     +-- ... --+- ...  -+- ... -+--------...-------+
1352     | Rule ID |  DTag  |  FCN  | Fragment payload |
1353     +-- ... --+- ...  -+- ... -+--------...-------+

1389    In the No-ACK mode, the last SCHC fragment of an IPv6 datagram SHALL
1390    contain a SCHC fragment header that conforms to the detaield format
1391    shown in Figure 13.  The total size of this SCHC fragment header is
1392    R+M bits.
1396
1405    <------------ R ----------->
1406                  <- T -> <N=1> <---- M ---->
1407    +---- ... ---+- ... -+-----+---- ... ----+---...---+
1408    |   Rule ID  | DTag  |  1  |     MIC     | payload |
1409    +---- ... ---+- ... -+-----+---- ... ----+---...---+

---------------------------------------------------------------------------

1322    In ACK-Always or ACK-on-Error, SCHC fragments except the last one
1323    SHALL conform the detailed format defined in {{Fig- NotLastWin}}. The
1324    total size of the fragment header is R bits.  Where is R is not a
1325    multiple of 8 bits.
1326
1327     <------------ R ----------->
1328                <--T--> 1 <--N-->
1329     +-- ... --+- ... -+-+- ... -+--------...-------+
1330     | Rule ID | DTag  |W|  FCN  | Fragment payload |
1331     +-- ... --+- ... -+-+- ... -+--------...-------+

1364    The All-0 format is used for sending the last SCHC fragment of a
1365    window that is not the last window of the packet.
1366
1367         <------------ R ----------->
1368                    <- T -> 1 <- N ->
1369         +-- ... --+- ... -+-+- ... -+--- ... ---+
1370         | Rule ID | DTag  |W|  0..0 |  payload  |
1371         +-- ... --+- ... -+-+- ... -+--- ... ---+


*/

