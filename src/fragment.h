/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef FRAGMENT_H
#define FRAGMENT_H

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"
#include "schic.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

#define MIC_SIZE (4)
#define MIC_BITSIZE (BITS_PER_BYTE * MIC_SIZE)
void compute_mic(uint8_t* data, size_t data_size,
                 uint8_t* result, size_t result_max_size);


typedef struct {
    bool is_sender; /**< fragmenting if true, otherwise defragmenting */
    schic_ack_mode_t ack_mode;

    buffer_t data;
    bit_buffer_t bit_data;
    uint8_t mic[MIC_SIZE];

    uint32_t     rule_id;
    unsigned int rule_id_bitsize;

    uint32_t     dtag;

    unsigned int R;
    unsigned int T; /**< bitsize of dtag */
    unsigned int N;
    unsigned int W;
    unsigned int M; /**< bitsize of MIC */


    unsigned int frag_index; /* sender */
    unsigned int frag_count;
    size_t frag_size; /* sender */

} frag_engine_t;

/*--------------------------------------------------*/


void frag_engine_init(frag_engine_t *engine,
                      uint8_t *data, size_t max_data_size,
                      size_t rule_id_bitsize, size_t dtag_bitsize);

void frag_sender_prepare_noack(frag_engine_t *engine,
                               uint32_t rule_id, uint32_t dtag,
                               size_t max_frag_size);

void frag_receiver_prepare_noack(frag_engine_t *engine);

int frag_engine_generate(frag_engine_t *engine,
                         uint8_t *res_data, size_t res_data_max_size);

int frag_engine_process(frag_engine_t *engine,
                        uint8_t *res_data, size_t res_data_max_size);

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* FRAGMENT_H */
