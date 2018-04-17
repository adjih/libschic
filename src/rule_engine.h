/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

typedef struct {
    uint32_t context_id;
    uint8_t  rule_id_bitsize;
    uint32_t default_rule_id;
    schic_mic_type_t mic_type;
} rule_context_t;

typedef struct {
    uint8_t *bytecode;
    size_t   bytecode_size;

    unsigned int rule_count;
    size_t rule_start_position;

    rule_context_t rule_context;
} rule_engine_t;

void rule_engine_init(rule_engine_t *engine);

int rule_engine_load_rule_bytecode(rule_engine_t *engine,
                                   uint8_t *data, size_t data_size);

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* RULE_ENGINE_H */
