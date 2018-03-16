/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

/*
  Comments:
  - DiffServ vs Traffic Class (RFC 8200) - and "9.2 IPv6 Traffic class"
  - Naming of the fields should be defined rigourously
    DEVprefix/DEViid/APPprefix/APPiid is LoRaWAN specific ?
  - Inconsistency in capitalizing "9.3. Flow label", "9.5 Next Header"
  - Why prefix length 64 necessarily, not link-layer specific ?
 */

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

#define IP_VERSION_IPV6 (6)
#define IPV6_ADDRESS_SIZE (16)
#define IPV6_SCHC_PREFIX_SIZE_BYTES (8)
#define IPV6_SCHC_IID_SIZE_BYTES (8)
#define IPV6_FIXED_HEADER_SIZE (4+4+IPV6_ADDRESS_SIZE+IPV6_ADDRESS_SIZE)
#define IP_PROTOCOL_NUMBER_UDP (17)

static inline void
set_token(rule_token_t *token, const unsigned int fid,
          const uint32_t value, uint8_t *const value_ptr, const size_t length)
{
    token->field_id_namespace = FID_NAMESPACE_IPV6;
    token->field_id = fid;
    token->field_position = 1;

    token->target.value = value;
    token->target.value_ptr = value_ptr;
    token->target.length = length;
    //DEBUG("set_token %u\n", fid);
}

/*
 *
 *
 */
int lethryk_parse_ipv6(buffer_t* data,
                       rule_token_t* result, size_t result_max_size)
{
    size_t packet_size = buffer_get_available(data);
    //DEBUG("packet total size=%u\n", packet_total - IPV6_FIXED_HEADER_SIZE);
    
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
        | ((uint32_t) data3);
    result_index += 3;
    CHECK_BOUND_OR_RETURN(data, result_index-1, result_max_size);
    
    if (ip_version != IP_VERSION_IPV6) {
        DEBUG("not an IPv6 packet");
        return -1;
    }
    set_token(&result[current], FID_IPv6_version, ip_version, NULL, 4);
    set_token(&result[current+1], FID_IPv6_Traffic_class,
	      traffic_class, NULL, 8);
    set_token(&result[current+2], FID_IPv6_Flow_label, flow_label, NULL, 20);

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

    if ( (packet_size < IPV6_FIXED_HEADER_SIZE)
	 || (packet_size - IPV6_FIXED_HEADER_SIZE != payload_length)) {
      // XXX: is it grounds for rejection ?
      DEBUG("incorrect IPv6 packet size, %zu %u\n",
            packet_size, payload_length);
      return -1;
    }
    set_token(&result[current], FID_IPv6_Payload_Length,
              payload_length, NULL, BUFFER_SIZE_U16*BITS_PER_BYTE);
    set_token(&result[current+1], FID_IPv6_Next_Header,
	      next_header, NULL, BUFFER_SIZE_U8*BITS_PER_BYTE);
    set_token(&result[current+2], FID_IPv6_Hop_Limit,
	      hop_limit, NULL, BUFFER_SIZE_U8*BITS_PER_BYTE);
    
    /*&
      303    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      307    +                         Source Address                        +
      308    |                                                               |
      311    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      315    +                      Destination Address                      +
      316    |                                                               |
      319    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */

    /*& 2174 As in 6LoWPAN [RFC4944], IPv6 addresses are split into two 64-bit*/
    /*& 2191 Both ends MUST be synchronized with the appropriate prefixes. */

    current = result_index;
    uint8_t* src_prefix = buffer_peek_data(data, IPV6_SCHC_PREFIX_SIZE_BYTES);
    uint8_t* src_iid    = buffer_peek_data(data, IPV6_SCHC_IID_SIZE_BYTES);    
    uint8_t* dst_prefix = buffer_peek_data(data, IPV6_SCHC_PREFIX_SIZE_BYTES);
    uint8_t* dst_iid    = buffer_peek_data(data, IPV6_SCHC_IID_SIZE_BYTES);
    result_index += 4;
    CHECK_BOUND_OR_RETURN(data, result_index-1, result_max_size);

    set_token(&result[current], FID_IPv6_Source_Address_Prefix,
              0, src_prefix, IPV6_SCHC_PREFIX_SIZE_BYTES * BITS_PER_BYTE);
    set_token(&result[current+1], FID_IPv6_Source_Address_IID,
              0, src_iid, IPV6_SCHC_IID_SIZE_BYTES * BITS_PER_BYTE);
    set_token(&result[current+2], FID_IPv6_Destination_Address_Prefix,
              0, dst_prefix, IPV6_SCHC_PREFIX_SIZE_BYTES * BITS_PER_BYTE);
    set_token(&result[current+3], FID_IPv6_Destination_Address_IID,
              0, dst_iid, IPV6_SCHC_IID_SIZE_BYTES * BITS_PER_BYTE);	      

    /*& 
      343  Next Header         8-bit selector.  Identifies the type of header
      344                      immediately following the IPv6 header.  Uses
      345                      the same values as the IPv4 Protocol field
      346                      [IANA-PN].
      
      1821    [IANA-PN]  IANA, "Protocol Numbers",
      1822               <https://www.iana.org/assignments/protocol-numbers>.
    */
    if (next_header != IP_PROTOCOL_NUMBER_UDP) {
      return result_index;
    }

    // XXX: RFC 768
    /*& 2251 9.9.  UDP source and destination port */
    /*& 2275 9.10.  UDP length field */
    /*& 2287 9.11.  UDP Checksum field */    
    current = result_index;
    uint16_t src_port     = buffer_get_u16(data);
    uint16_t dst_port     = buffer_get_u16(data);
    uint16_t udp_length   = buffer_get_u16(data);
    uint16_t udp_checksum = buffer_get_u16(data);
    result_index += 4;
    CHECK_BOUND_OR_RETURN(data, result_index-1, result_max_size);
    
    set_token(&result[current], FID_IPv6_UDP_Source_Port,
              src_port, NULL, BUFFER_SIZE_U16 * BITS_PER_BYTE);
    set_token(&result[current+1], FID_IPv6_UDP_Destination_Port,
              dst_port, NULL, BUFFER_SIZE_U16 * BITS_PER_BYTE);
    set_token(&result[current+2], FID_IPv6_UDP_Length,
              udp_length, NULL, BUFFER_SIZE_U16 * BITS_PER_BYTE);
    set_token(&result[current+3], FID_IPv6_UDP_Checksum,
              udp_checksum, NULL, BUFFER_SIZE_U16 * BITS_PER_BYTE);
    // XXX: check udp size
    
    return result_index;
}

/*---------------------------------------------------------------------------*/
