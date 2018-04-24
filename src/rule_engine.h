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
    uint8_t *raw_rule;
    size_t   raw_rule_size;

    size_t rule_frag_count;
    size_t rule_frag_start;
    size_t rule_compress_count;
    size_t rule_compress_start;

    size_t bytecode_start;
    size_t bytecode_size;

    rule_context_t rule_context;
} rule_engine_t;

void rule_engine_init(rule_engine_t *engine);

int rule_engine_load_rule_bytecode(rule_engine_t *engine,
                                   uint8_t *data, size_t data_size);

/*--------------------------------------------------*/

typedef struct {
    uint32_t         rule_id;
    schic_ack_mode_t ack_mode;
    uint8_t          dtag_bitsize;
    uint8_t          fcn_bitsize;
    uint32_t         default_dtag;
} parsed_frag_rule_t;

typedef struct {
    uint32_t rule_id;
    uint8_t  nb_action;
    size_t   bytecode_position;
    size_t   bytecode_size;
} parsed_compress_rule_t;

/*--------------------------------------------------*/

void rule_engine_retrieve_frag_rule
(rule_engine_t* engine, size_t frag_rule_idx, parsed_frag_rule_t* result);

void rule_engine_retrieve_compress_rule
(rule_engine_t* engine, size_t compress_rule_idx,
 parsed_compress_rule_t* result);

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* RULE_ENGINE_H */
