/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef FRAGMENT_H
#define FRAGMENT_H

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

/*&
  1213    This specification defines three reliability modes: No-ACK, ACK-
  1214    Always and ACK-on-Error.  ACK-Always and ACK-on-Error operate on
*/
typedef enum {
    SCHC_NO_ACK,
    SCHC_ACK_ALWAYS,
    SCHC_ACK_ON_ERROR
} ack_mode_t; 
    
    
typedef struct {
    //uint8_t* data;
    //size_t   data_size;
    buffer_t data;

    bool is_sender; /**< fragmenting if true, otherwise defragmenting */

    uint32_t rule_id;
    size_t   rule_id_bitsize;

    uint32_t dtag;
    
    unsigned int R;
    unsigned int T;
    unsigned int N;
    unsigned int W;

    ack_mode_t ack_mode;
    size_t max_frag_size;
    
} frag_engine_t;

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif
    
/*---------------------------------------------------------------------------*/

#endif  /* FRAGMENT_H */
    
