/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include "schic.h"
#include "rule_engine.h"

/*---------------------------------------------------------------------------*/

/*--------------------------------------------------*/

#define RET_IF(condition,...) BEGIN_MACRO       \
    if (condition) {                            \
        DEBUG(__VA_ARGS__);                     \
        return -1;                              \
    }                                           \
    END_MACRO

/*---------------------------------------------------------------------------*/

void rule_engine_init(rule_engine_t *engine)
{
    memset(&engine, 0, sizeof(engine));
    //memset(&engine->rule_context, 0, sizeof(engine->rule_context));
    //engine->bytecode = NULL;
    //engine->bytecode_size = 0;
    //engine->rule_count = 0;
}

/*---------------------------------------------------------------------------*/

int rule_engine_load_rule_bytecode(rule_engine_t *engine,
                                   uint8_t *data, size_t data_size)
{
    rule_engine_init(engine);

    buffer_t buffer;
    buffer_init(&buffer, data, data_size);

    /* SCHIC header */
    uint32_t magic = buffer_get_u32(&buffer);
    if (magic != SCHIC_BYTECODE_MAGIC) {
        DEBUG("bad SCHIC bytecode magic: %u\n", magic);
        return -1;
    }
    uint32_t version = buffer_get_u32(&buffer);
    RET_IF((version != SCHIC_BYTECODE_VERSION_BASIC),
           "unexpected SCHIC bytecode version: %u\n", version);

    /* Context */
    uint8_t context_id = buffer_get_u8(&buffer);
    RET_IF( (context_id != SCHIC_CONTEXT_ID_BASIC),
            "unexpected SCHIC context ID: %u\n", context_id);
    engine->rule_context.context_id = context_id;

    uint8_t rule_id_bitsize = buffer_get_u8(&buffer);
    RET_IF( (rule_id_bitsize > sizeof(uint32_t)*BITS_PER_BYTE),
            "Bad rule ID bitsize: %u\n", rule_id_bitsize);
    engine->rule_context.rule_id_bitsize = rule_id_bitsize;

    uint32_t default_rule_id = buffer_get_u8(&buffer);
    RET_IF(!u32_bitsize_lower_than(default_rule_id, rule_id_bitsize),
           "Default rule ID too large: %u (>%u bits)\n",
           default_rule_id, rule_id_bitsize);
    engine->rule_context.default_rule_id = default_rule_id;

    uint8_t mic_type = buffer_get_u8(&buffer);
    RET_IF(!schic_mic_type_is_valid(mic_type),
           "Invalid MIC type %u\n", mic_type);
    engine->rule_context.mic_type = mic_type;

    size_t rule_count = buffer_get_u32(&buffer);
    RET_IF(buffer.has_bound_error, "bytecode too small\n");
    engine->bytecode = data;
    engine->bytecode_size = data_size;
    engine->rule_start_position = buffer.position;
    engine->rule_count = rule_count;

    return 0;
}

/*---------------------------------------------------------------------------*/

#define RULE_FRAGMENT_SIZE (1+4+ 1+1+1+4)
#define RULE_COMPRESS_SIZE (1+4+ 1+4+4)

void rule_engine_init_buffer_to_rule_bytecode
(rule_engine_t *engine, buffer_t *buffer)
{
    assert(engine->rule_start_position <= engine->bytecode_size);
    buffer_init(buffer, engine->bytecode + engine->rule_start_position,
                engine->bytecode_size - engine->rule_start_position);
}

int fragment_check_rule_match
(rule_engine_t *engine, uint8_t *data, size_t data_size, buffer_t* buffer)
{
    /* buffer is expected to be just past rule type */
    uint8_t fragment_mode = buffer_get_u8(buffer);
    uint8_t dtag_bitsize = buffer_get_u8(buffer);
    uint8_t fcn_bitsize = buffer_get_u8(buffer);

    return -1; // XXX: not implemented yet
}

int rule_engine_find_frag_rule
(rule_engine_t *engine, uint8_t *data, size_t data_size)
{
    if (engine->rule_count == 0) {
        return -1;
    }

    buffer_t buffer;
    rule_engine_init_buffer_to_rule_bytecode(engine, &buffer);

    for (unsigned int i = 0; i < engine->rule_count; i++) {
        schic_rule_type_t rule_type = buffer_get_u8(&buffer);
        RET_IF(!schic_rule_type_is_valid(rule_type), "invalid rule type");

        if (rule_type == SCHIC_RULE_FRAGMENT) {

            int rule_match =
                fragment_check_rule_match(engine, data, data_size, &buffer);
            if (rule_match)
                return i;
        } else {
            /* not a compression rule, skip it */
            (void)buffer_pop_data(&buffer, RULE_COMPRESS_SIZE - 1/*type*/);
        }

    }
    return -1;
}


int compress_check_rule_match
(rule_engine_t *engine, uint8_t *data, size_t data_size,
 rule_token_t* token_array, size_t nb_token,
 buffer_t* buffer)
{
    /* buffer is expected to be just past rule type */
    unsigned int nb_action = buffer_get_u8(buffer);
    uint32_t bytecode_position = buffer_get_u32(buffer);
    uint32_t bytecode_size = buffer_get_u32(buffer);

    RET_IF( (bytecode_position + bytecode_size > engine->bytecode_size),
            "rule bytecode out of range");
    return -1; // XXX: not implemented
}


//XXX: not thread safe
#define MAX_RULE_TOKEN 20
rule_token_t _rule_token_array[MAX_RULE_TOKEN];


int rule_engine_find_compress_rule
(rule_engine_t *engine, uint8_t *data, size_t data_size)
{
    if (engine->rule_count == 0) {
        return -1; // XXX: just not found
    }

    buffer_t data_buffer;
    buffer_init(&data_buffer, data, data_size);
    // maybe there are no compression rules, then no need to parse?
    int nb_token = schic_parse_ipv6_udp(&data_buffer, _rule_token_array,
                                        MAX_RULE_TOKEN);
    if (nb_token < 0) {
        /* not an IPv6 packet, (or some error) */
        return -1;
    }

    buffer_t buffer;
    rule_engine_init_buffer_to_rule_bytecode(engine, &buffer);
    for (unsigned int i = 0; i < engine->rule_count; i++) {
        schic_rule_type_t rule_type = buffer_get_u8(&buffer);
        RET_IF(!schic_rule_type_is_valid(rule_type), "invalid rule type");

        if (rule_type == SCHIC_RULE_COMPRESS) {

            int rule_match =
                compress_check_rule_match(engine, data, data_size,
                                          _rule_token_array, nb_token, &buffer);
            if (rule_match)
                return i;
        } else {
            /* not a compression rule, skip it */
            (void)buffer_pop_data(&buffer, RULE_FRAGMENT_SIZE - 1/*type*/);
        }

    }
    return -1;
}

/*---------------------------------------------------------------------------*/
