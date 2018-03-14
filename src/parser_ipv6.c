/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

/* example of IPv6 parsing:
   https://github.com/the-tcpdump-group/tcpdump/blob/master/print-ip6.c */

#include "buffer.h"
#include "lethryk.h"

#define CHECK_BOUND_OR_RETURN(buffer, result_index, result_max_size) \
    BEGIN_MACRO                                                      \
    if ((buffer)->has_bound_error) {                                 \
        return -1;                                                   \
    }                                                                \
    if ((result_index) >= (result_max_size)) {                       \
        return -1;                                                   \
    }                                                                \
    END_MACRO

#define IP_VERSION_IPV6 6
#define IPV6_ADDRESS_SIZE 16

static inline void
set_token(rule_token_t *token, unsigned int fid,
          uint32_t value, uint8_t *value_ptr, size_t length)
{
    token->field_id_namespace = FIELD_ID_IPV6_SPACE;
    token->field_id = fid;
    token->field_position = 1;

    token->target.value = value;
    token->target.value_ptr = value_ptr;
    token->target.length = length;
}

int lethryk_parse_ipv6(buffer_t* data,
                       rule_token_t* result, size_t result_max_size)
{
    size_t result_index = 0;
    size_t current = 0;
    CHECK_BOUND_OR_RETURN(data, result_index, result_max_size);    

    /*&
      299    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      300    |Version| Traffic Class |           Flow Label                  |
      301    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    current = result_index;
    uint8_t  data1 = buffer_get_u8(data);
    uint8_t  data2 = buffer_get_u8(data);
    uint16_t data3 = buffer_get_u16(data);
    uint8_t  ip_version =  (data1 & 0xf0) >> 4;
    uint8_t  traffic_class = ((data1 & 0x0f) << 4) | ((data2 & 0xf0) >> 4);
    uint32_t flow_label = (((uint32_t) (data2 & 0x0f)) << 16)
        | ((uint32_t) data3); // XXX: check parsing is correct !!!
    result_index += 3;
    CHECK_BOUND_OR_RETURN(data, result_index-1, result_max_size);
    
    if (ip_version != IP_VERSION_IPV6) {
        DEBUG("not an IPv6 packet");
        return -1;
    }
    set_token(&result[current], FID_IPv6_version,
              ip_version, NULL, BUFFER_SIZE_U8);
    set_token(&result[current+1], FID_IPv6_Traffic_class,
              traffic_class, NULL, BUFFER_SIZE_U8);
    set_token(&result[current+2], FID_IPv6_Flow_label,
              flow_label, NULL, BUFFER_SIZE_U32);

    /*&
      301    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      302    |         Payload Length        |  Next Header  |   Hop Limit   |
      303    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    current = result_index;
    uint16_t payload_length = buffer_get_u16(data);
    uint8_t next_header = buffer_get_u8(data);
    uint8_t hop_limit = buffer_get_u8(data);
    result_index += 3;
    CHECK_BOUND_OR_RETURN(data, result_index-1, result_max_size);

    
    /*&
      303    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      307    +                         Source Address                        +
      308    |                                                               |
      311    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      315    +                      Destination Address                      +
      316    |                                                               |
      319    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    current = result_index;
    uint8_t* source_address = buffer_peek_data(data, IPV6_ADDRESS_SIZE);
    uint8_t* destination_address = buffer_peek_data(data, IPV6_ADDRESS_SIZE);
    result_index += 2;
    CHECK_BOUND_OR_RETURN(data, result_index-1, result_max_size);
    
    
    CHECK_BOUND_OR_RETURN(data, result_index, result_max_size);
    return result_index;
}

/*---------------------------------------------------------------------------*/
