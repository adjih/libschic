/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "buffer.h"

/*---------------------------------------------------------------------------*/

void bit_buffer_init(bit_buffer_t *bit_buffer, buffer_t *base_buffer)
{
    bit_buffer->buffer = base_buffer;
    bit_buffer->pending = 0;
    bit_buffer->pending_bit_count = 0;
}

/* bit ordering: RFC 1700,  XXX:
   
Whenever an octet represents a numeric quantity the left most bit in the
diagram is the high order or most significant bit.  That is, the bit
labeled 0 is the most significant bit.  For example, the following
diagram represents the value 170 (decimal).


                          0 1 2 3 4 5 6 7
                         +-+-+-+-+-+-+-+-+
                         |1 0 1 0 1 0 1 0|
                         +-+-+-+-+-+-+-+-+

                        Significance of Bits

Similarly, whenever a multi-octet field represents a numeric quantity
the left most bit of the whole field is the most significant bit.
*/

#if 0 
#define BIT_MASK(x) ((1ul << (x)) - 1)

uint8_t bit_buffer_get_u8(bit_buffer_t *bb)
{
    if (bb->pending_bit_count < BITS_PER_BYTE) {
        bb->pending = (bb->pending << BITS_PER_BYTE)
            | buffer_get_u8(bb->buffer);
        bb->pending_bit_count += BITS_PER_BYTE;
    }
    uint8_t result = bb->pending >> (bb->pending_bit_count - BITS_PER_BYTE);
    bb->pending = bb->pending & BITMASK(bb->pending_bit_count)
    
    return result;
}
#endif

uint8_t bit_buffer_get_bit(bit_buffer_t *bb)
{
    if (bb->pending_bit_count == 0) {
        bb->pending = buffer_get_u8(bb->buffer);
        bb->pending_bit_count += BITS_PER_BYTE;
    }

    uint8_t result = bb->pending >> (bb->pending_bit_count-1);
    bb->pending = bb->pending & ~(1u << (bb->pending_bit_count-1));
    bb->pending_bit_count --;
    return result;
}

void bit_buffer_put_bit(bit_buffer_t *bb, uint8_t bit)
{
    assert(bit == 0 || bit == 1);
    bb->pending = (bb->pending<<1) | bit;
    bb->pending_bit_count += 1;
    if (bb->pending_bit_count == BITS_PER_BYTE) {
        buffer_put_u8(bb->buffer, bb->pending);
        bb->pending = 0;
        bb->pending_bit_count = 0;
    }
}

uint32_t bit_buffer_get_several(bit_buffer_t *bb, size_t bit_count)
{
    assert(bit_count < sizeof(uint32_t));
    uint32_t result = 0;
    for (size_t i = 0; i < bit_count; i++) {
        result = (result<<1) | bit_buffer_get_bit(bb);
    }
    return result;
}

void bit_buffer_put_several(bit_buffer_t *bb,
                              uint32_t bit_block, size_t bit_count)
{
    assert(bit_count < sizeof(uint32_t));
    for (uint32_t i = 0; i < bit_count; i++) {
        uint8_t current = (bit_block >> (bit_count-1 -i)) & 0x1u;
        bit_buffer_put_bit(bb, current);
    }
}
   
/*---------------------------------------------------------------------------*/
