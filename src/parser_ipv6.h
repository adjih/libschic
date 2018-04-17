/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef PARSER_IPV6_H
#define PARSER_IPV6_H

/*---------------------------------------------------------------------------*/

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

static const int FID_NAMESPACE_IPV6 = 1; // SCHIC_FID_SPACE_IVP6;

typedef enum {
    /*& 2082 9.1.  IPv6 version field */
    FID_IPv6_version = 0,

    /*& 2087 9.2.  IPv6 Traffic class field */
    FID_IPv6_Traffic_class,

    /*& 2105 9.3.  Flow label field */
    FID_IPv6_Flow_label,

    /*& 2133 9.4.  Payload Length field */
    FID_IPv6_Payload_Length,

    /*& 2145 9.5.  Next Header field */
    FID_IPv6_Next_Header,

    /*& 2156 9.6.  Hop Limit field */
    FID_IPv6_Hop_Limit,

    /*& 2172 9.7.  IPv6 addresses fields */
    FID_IPv6_Source_Address_Prefix,
    FID_IPv6_Source_Address_IID,
    FID_IPv6_Destination_Address_Prefix,
    FID_IPv6_Destination_Address_IID,

    /*& 2251 9.9.  UDP source and destination port */
    FID_IPv6_UDP_Source_Port,
    FID_IPv6_UDP_Destination_Port,

    /*& 2275 9.10.  UDP length field */
    FID_IPv6_UDP_Length,

    /*& 2287 9.11.  UDP Checksum field */
    FID_IPv6_UDP_Checksum,

    FID_IPv6_COUNT
} field_id_ipv6_t;


typedef struct {
    uint32_t value;
    uint8_t *value_ptr;
    size_t   length; /* in bits */
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
    schic_field_namespace_t field_id_namespace; /**< FID */
    unsigned int      field_id;           /**< FID */
    unsigned int      field_position;     /**< FP */
    target_value_t    target;             /**< TV */
} rule_token_t;

int schic_parse_ipv6_udp(buffer_t* data,
                         rule_token_t* result, size_t result_max_size);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif /* PARSER_IPV6_H */
