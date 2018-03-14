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

static const int FIELD_ID_IPV6_SPACE = 0;

typedef enum {
    /*& 2082 9.1.  IPv6 version field */
    FID_IPv6_version,

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
    FID_IPv6_Source_Address,
    FID_IPv6_Destination_Address,

    FID_IPv6_Unused // XXX
} field_id_ipv6_t;




#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif /* PARSER_IPV6_H */

