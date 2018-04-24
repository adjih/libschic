/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "schic.h"
#include "rule_engine.h"

void rule_engine_frag_rule_repr(rule_engine_t *engine,
                                size_t rule_idx, FILE *out)
{
    assert(rule_idx < engine->rule_frag_count);
    parsed_frag_rule_t rule;
    rule_engine_retrieve_frag_rule(engine, rule_idx, &rule);

    fprintf(out, "{'type':'frag_rule'");
    fprintf(out, ", 'rule_id':%"PRIu32, rule.rule_id);
    fprintf(out, ", 'ack_mode':%u", rule.ack_mode); // XXX: not an in
    fprintf(out, ", 'dtag_bitsize':%u", rule.dtag_bitsize);
    fprintf(out, ", 'fcn_bitsize':%u", rule.fcn_bitsize);
    fprintf(out, ", 'default_dtag':%u", rule.fcn_bitsize);
    fprintf(out, "}");
}

#define RULE_CDA_SIZE (1+1+1+1+1+1 +4 +4)

void rule_engine_compress_rule_repr(rule_engine_t *engine,
                                    size_t rule_idx, FILE *out)
{
    assert(rule_idx < engine->rule_compress_count);
    parsed_compress_rule_t rule;
    rule_engine_retrieve_compress_rule(engine, rule_idx, &rule);

    fprintf(out, "{'type':'compress_rule'");
    fprintf(out, ", 'rule_id':%"PRIu32, rule.rule_id);
    fprintf(out, ", 'nb_action':%u", rule.nb_action); // XXX: not an in
    fprintf(out, ", 'bytecode_position':%zu", rule.bytecode_position);
    fprintf(out, ", 'bytecode_size':%zu", rule.bytecode_size);

    fprintf(out, ", 'bytecode':[");
    for (unsigned int i=0; i<rule.bytecode_size; i += RULE_CDA_SIZE) {
        if (i > 0) {
            fprintf(out,",");
        }
        if (i+RULE_CDA_SIZE > rule.bytecode_size) {
            fprintf(out,"'overflow'");
        }
    }
    
    fprintf(out, "]");
    fprintf(out, "}");
}

void rule_engine_repr(rule_engine_t *engine, FILE *out)
{
    fprintf(out, "{'type':'rule_engine'");
    fprintf(out, ", 'rule_frag_count':%zu", engine->rule_frag_count);
    fprintf(out, ", 'rule_frag_start':%zu", engine->rule_frag_start);
    fprintf(out, ", 'rule_compress_count':%zu", engine->rule_compress_count);
    fprintf(out, ", 'rule_compress_start':%zu", engine->rule_compress_start);
    fprintf(out, ", 'bytecode_start':%zu", engine->bytecode_start);
    fprintf(out, ", 'bytecode_size':%zu", engine->bytecode_size);

    fprintf(out, ", 'frag_rule_list':[");
    for (size_t i = 0; i < engine->rule_frag_count; i++) {
        if (i > 0) {
            fprintf(out, ", ");
        }
        rule_engine_frag_rule_repr(engine, i, out);
    }
    fprintf(out, "]");

    fprintf(out, ", 'compress_rule_list':[");
    for (size_t i = 0; i < engine->rule_compress_count; i++) {
        if (i > 0) {
            fprintf(out, ", ");
        }
        rule_engine_compress_rule_repr(engine, i, out);
    }
    fprintf(out, "]");

    fprintf(out, "}");
}

/*---------------------------------------------------------------------------*/
