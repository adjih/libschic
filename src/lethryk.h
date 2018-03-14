/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef LETHRYK_H
#define LETHRYK_H

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

typedef enum {
    LETHRYK_FID_SPACE_IVP6,
    LETHRYK_FID_SPACE_UDP,
    LETHRYK_FID_SPACE_COAP,
    LETHRYK_FID_SPACE_ICMPV6,    
} field_namespace_t;

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
    Up,
    Dw,
    Bi
} direction_indicator_t;

typedef struct {
    uint32_t value;
    uint8_t *value_ptr;
    size_t   length; /* in bytes; if 0, value is used otherwise position */
} target_value_t;

/*&
 655    o  Field ID (FID) is a unique value to define the header field.
 656
 657    o  Field Length (FL) represents the length of the field in bits for
 658       fixed values or a type (variable, token length, ...) for Field
 659       Description length unknown at the rule creation.  The length of a
 660       header field is defined in the specific protocol standard.
 661
 662    o  Field Position (FP): indicating if several instances of a field
 663       exist in the headers which one is targeted.  The default position
 664       is 1.
 ......................................................................
 686    o  Target Value (TV) is the value used to make the match with the
 687       packet header field.  The Target Value can be of any type
 688       (integer, strings, etc.).  For instance, it can be a single value
 689       or a more complex structure (array, list, etc.), such as a JSON or
 690       a CBOR structure.
*/

typedef struct {
    field_namespace_t field_id_namespace; /**< FID */
    unsigned int      field_id;           /**< FID */
    unsigned int      field_position;     /**< FP */
    target_value_t    target;             /**< TV */
} rule_token_t;

/*---------------------------------------------------------------------------*/

#include "parser_ipv6.h"

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif
    
/*---------------------------------------------------------------------------*/

#endif  /* LETHRYK_H */
