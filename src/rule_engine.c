/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include "assert.h"

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
}

/*---------------------------------------------------------------------------*/

#define RULE_FRAG_SIZE (4+ 1+1+1+4)
#define RULE_COMPRESS_SIZE (4+ 1+4+4)

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

    uint32_t default_rule_id = buffer_get_u32(&buffer);
    RET_IF(!u32_bitsize_lower_than(default_rule_id, rule_id_bitsize),
           "Default rule ID too large: %u (>%u bits)\n",
           default_rule_id, rule_id_bitsize);
    engine->rule_context.default_rule_id = default_rule_id;

    uint8_t mic_type = buffer_get_u8(&buffer);
    RET_IF(!schic_mic_type_is_valid(mic_type),
           "Invalid MIC type %u\n", mic_type);
    engine->rule_context.mic_type = mic_type;

    DEBUG("load-rule:\n  version=%u context_id=%lu rule_id_bitsize=%u"
          " default_rule_id=%lu mic_type=%u\n", version,
          context_id, rule_id_bitsize, default_rule_id, mic_type);

    /* updating pointers */
    size_t rule_frag_count = buffer_get_u32(&buffer);
    size_t rule_compress_count = buffer_get_u32(&buffer);
    DEBUG("  nb_rule_frag=%u nb_rule_compress=%u\n",
          rule_frag_count, rule_compress_count);
    RET_IF(buffer.has_bound_error, "rule data too small\n");

    size_t rule_frag_start = buffer.position;
    size_t rule_frag_size = RULE_FRAG_SIZE * rule_frag_count;
    size_t rule_compress_start = rule_frag_start + rule_frag_size;
    size_t rule_compress_size = RULE_COMPRESS_SIZE * rule_compress_count;
    size_t bytecode_start = rule_compress_start + rule_compress_size;
    DEBUG("  frag_start=%u frag_size=%u/%u\n", rule_frag_start, rule_frag_size,
          RULE_FRAG_SIZE);
    DEBUG("  compress_start=%u compress_size=%u/%u\n",
          rule_compress_start, rule_compress_size, RULE_COMPRESS_SIZE);
    DEBUG("  bytecode_start=%u data_size=%u\n", bytecode_start, data_size);
    RET_IF(bytecode_start > data_size, "out of bounds of rule data\n");

    engine->raw_rule = data;
    engine->raw_rule_size = data_size;
    engine->rule_frag_count = rule_frag_count;
    engine->rule_compress_count = rule_compress_count;
    engine->rule_frag_start = rule_frag_start;
    engine->rule_compress_start = rule_compress_start;
    engine->bytecode_start = bytecode_start;
    engine->bytecode_size = data_size - bytecode_start;

    return 0;
}

/*---------------------------------------------------------------------------*/

bool frag_check_rule_match
(rule_engine_t *engine, uint8_t *data, size_t data_size,
 buffer_t* rule_buffer)
{
    /* buffer is expected to be just past rule type */
    uint32_t rule_id = buffer_get_u8(rule_buffer);
    uint8_t fragment_mode = buffer_get_u8(rule_buffer);
    uint8_t dtag_bitsize = buffer_get_u8(rule_buffer);
    uint8_t fcn_bitsize = buffer_get_u8(rule_buffer);

    assert(!rule_buffer->has_bound_error); /* must not happen with any input */

    return false; // XXX: not implemented yet
}

/* Return is frag_rule index (>=0) or -1 if no rule applies */
int rule_engine_find_frag_rule
(rule_engine_t *engine, uint8_t *data, size_t data_size)
{
    if (engine->rule_frag_count == 0) {
        return -1; /* no rule found */
    }

    for (unsigned int i = 0; i < engine->rule_frag_count; i++) {
        buffer_t rule_buffer;
        size_t position = engine->rule_frag_start + i * RULE_FRAG_SIZE;
        buffer_init(&rule_buffer, engine->raw_rule+position, RULE_FRAG_SIZE);
        int rule_match = frag_check_rule_match(engine, data, data_size,
                                               &rule_buffer);
        if (rule_match)
            return i;
    }
    return -1;
}

bool compress_check_rule_match
(rule_engine_t *engine, rule_token_t* token_array, size_t nb_token,
 buffer_t* buffer)
{
    /* buffer is expected to be just past rule type */
    uint32_t rule_id = buffer_get_u8(buffer);
    unsigned int nb_action = buffer_get_u8(buffer);
    uint32_t bytecode_position = buffer_get_u32(buffer);
    uint32_t bytecode_size = buffer_get_u32(buffer);

    RET_IF( (bytecode_position + bytecode_size > engine->bytecode_size),
            "rule bytecode out of range");

    // XXX: we need to check the matching, probably the fid should be
    // sorted in both  token_array and action bytecode

    return false; // XXX: not implemented
}

int rule_engine_find_compress_rule
(rule_engine_t *engine, rule_token_t *token_array, size_t nb_token)
{
    if (engine->rule_compress_count == 0) {
        return -1; /* no rule found */
    }

    for (unsigned int i = 0; i < engine->rule_compress_count; i++) {
        buffer_t rule_buffer;
        size_t position = engine->rule_compress_start + i * RULE_COMPRESS_SIZE;
        buffer_init(&rule_buffer, engine->raw_rule+position,
                    RULE_COMPRESS_SIZE);
        int rule_match =
            compress_check_rule_match(engine, token_array, nb_token,
                                      &rule_buffer);
        if (rule_match)
            return i;
    }
    return -1;
}

/*---------------------------------------------------------------------------*/

// XXX: need to create a top level function calling find_compress/find_frag
//   and then calling some do_compress, do_frag functions.

/*---------------------------------------------------------------------------*/

#if 0
//XXX: not thread safe
#define MAX_RULE_TOKEN 20
rule_token_t _rule_token_array[MAX_RULE_TOKEN];

    buffer_t data_buffer;
    buffer_init(&data_buffer, data, data_size);
    int nb_token = schic_parse_ipv6_udp(&data_buffer, _rule_token_array,
                                        MAX_RULE_TOKEN);
    if (nb_token < 0) {
        /* not an IPv6 packet, (or some error) */
        return -1;
    }
#endif
