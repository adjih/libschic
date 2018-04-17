/*---------------------------------------------------------------------------
 * This file contains all the high level definitions of schic that are
 * necessary for interoperability (exposed in the bytecode format).
 * See DESIGN.txt
 *
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef SCHIC_H
#define SCHIC_H

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

#define SCHIC_BYTECODE_MAGIC         0x43484943ul
#define SCHIC_BYTECODE_VERSION_BASIC 0x0

#define SCHIC_CONTEXT_ID_BASIC   0x1

typedef enum {
    SCHIC_MIC_TYPE_CRC32 = 0,
    SCHIC_MIC_TYPE_COUNT
} schic_mic_type_t;

static inline bool schic_mic_type_is_valid(schic_mic_type_t value)
{ return 0 <= value && value < SCHIC_MIC_TYPE_COUNT; }


typedef enum {
    SCHIC_RULE_COMPRESS = 0,
    SCHIC_RULE_FRAGMENT,
    SCHIC_RULE_COUNT
} schic_rule_type_t;

static inline bool schic_rule_type_is_valid(schic_rule_type_t value)
{ return 0 <= value && value < SCHIC_RULE_COUNT; }

/*---------------------------------------------------------------------------*/
/* Fragmentation rules */
/*&
  1213    This specification defines three reliability modes: No-ACK, ACK-
  1214    Always and ACK-on-Error.  ACK-Always and ACK-on-Error operate on
*/
typedef enum {
    SCHIC_ACK_NO_ACK = 0,
    SCHIC_ACK_ALWAYS,
    SCHIC_ACK_ON_ERROR,
    SCHIC_ACK_COUNT
} schic_ack_mode_t;

static inline bool schic_ack_mode_is_valid(schic_ack_mode_t value)
{ return 0 <= value && value < SCHIC_ACK_COUNT; }

/*---------------------------------------------------------------------------*/
/* Compression/decompression rules */

/* the idea is that there is one namespace per different IETF draft/RFC */
typedef enum {
    /* 0 is reserved (could be flat namespace) */
    SCHIC_FID_NAMESPACE_IVP6_UDP = 1, /* lpwan-ipv6 */
    SCHIC_FID_NAMESPACE_COAP,
    SCHIC_FID_NAMESPACE_ICMPV6,
    SCHIC_FID_NAMESPACE_COUNT
} schic_field_namespace_t;

/*&
  666    o  A direction indicator (DI) indicating the packet direction(s) this
  667       Field Description applies to.  Three values are possible:
  668
  677       *  UPLINK (Up): this Field Description is only applicable to
  678          packets sent by the Dev to the App,
  679
  680       *  DOWNLINK (Dw): this Field Description is only applicable to
  681          packets sent from the App to the Dev,
  682
  683       *  BIDIRECTIONAL (Bi): this Field Description is applicable to
  684          packets travelling both Up and Dw.
*/
typedef enum {
    SCHIC_DI_UP = 0,
    SCHIC_DI_DW,
    SCHIC_DI_BI,
    SCHIC_DI_COUNT
} schic_direction_indicator_t;

typedef schic_direction_indicator_t schic_di_t;

static inline bool schic_direction_indicator_is_valid(schic_di_t value)
{ return 0 <= value && value < SCHIC_DI_COUNT; }


/*&
 814 6.4.  Matching operators
 815
 816    Matching Operators (MOs) are functions used by both SCHC C/D
 817    endpoints involved in the header compression/decompression.  They are
 818    not typed and can be indifferently applied to integer, string or any
 819    other data type.  The result of the operation can either be True or
 820    False.  MOs are defined as follows:
 821
 822    o  equal: The match result is True if a field value in a packet and
 823       the value in the TV are equal.
 824
 825    o  ignore: No check is done between a field value in a packet and a
 826       TV in the Rule.  The result of the matching is always true.
 827
 828    o  MSB(x): A match is obtained if the most significant x bits of the
 829       field value in the header packet are equal to the TV in the Rule.
 830       The x parameter of the MSB Matching Operator indicates how many
 831       bits are involved in the comparison.
 832
 833    o  match-mapping: With match-mapping, the Target Value is a list of
 834       values.  Each value of the list is identified by a short ID (or
 835       index).  Compression is achieved by sending the index instead of
 836       the original header field value.  This operator matches if the
 845       header field value is equal to one of the values in the target
 846       list.
*/

typedef enum {
    SCHIC_MO_EQUAL = 0,
    SCHIC_MO_IGNORE,
    SCHIC_MO_MSG,
    SCHIC_MO_MATCH_MAPPING,
    SCHIC_MO_COUNT,
} schic_matching_operator_t;

typedef schic_matching_operator_t schic_mo_t;

static inline bool schic_matching_operator_is_valid(schic_mo_t value)
{ return 0 <= value && value < SCHIC_MO_COUNT; }


/*&
 848 6.5.  Compression Decompression Actions (CDA)
 849
 850    The Compression Decompression Action (CDA) describes the actions
 851    taken during the compression of headers fields, and inversely, the
 852    action taken by the decompressor to restore the original value.
 853
 854    /--------------------+-------------+---------------------------- \
 855    |  Action            | Compression | Decompression              |
 856    |                    |             |                            |
 857    +--------------------+-------------+----------------------------+
 858    |not-sent            |elided       |use value stored in ctxt    |
 859    |value-sent          |send         |build from received value   |
 860    |mapping-sent        |send index   |value from index on a table |
 861    |LSB(y)              |send LSB     |TV, received value          |
 862    |compute-length      |elided       |compute length              |
 863    |compute-checksum    |elided       |compute UDP checksum        |
 864    |Deviid              |elided       |build IID from L2 Dev addr  |
 865    |Appiid              |elided       |build IID from L2 App addr  |
 866    \--------------------+-------------+----------------------------/
 867    y=size of the transmitted bits
 868
*/

typedef enum {
    SCHIC_CDA_NOT_SENT = 0,
    SCHIC_CDA_VALUE_SENT,
    SCHIC_CDA_MAPPING_SENT,
    SCHIC_CDA_LSB,
    SCHIC_CDA_COMPUTE_LENGTH,
    SCHIC_CDA_COMPUTE_CHECKSUM,
    SCHIC_CDA_DEVIID,
    SCHIC_CDA_APPIID,
    SCHIC_CDA_COUNT
} schic_compression_decompression_action_t;

typedef schic_compression_decompression_action_t schic_cda_t;

static inline bool schic_compression_decompression_action_is_valid
(schic_cda_t value)
{ return 0 <= value && value < SCHIC_CDA_COUNT; }

/*---------------------------------------------------------------------------*/

#include "parser_ipv6.h"

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* SCHIC_H */
