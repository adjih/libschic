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

static const int FID_NAMESPACE_IPV6 = 0;

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
    
    FID_IPv6_COUNT // XXX
} field_id_ipv6_t;

int schic_parse_ipv6(buffer_t* data,
                     rule_token_t* result, size_t result_max_size);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif /* PARSER_IPV6_H */
