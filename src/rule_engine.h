/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

/*---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

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
  CDA_NOT_SENT = 0,
  CDA_VALUE_SENT,
  CDA_MAPPING_SENT,
  CDA_LSB,
  CDA_COMPUTE_LENGTH,
  CDA_COMPUTE_CHECKSUM,
  CDA_DEVIID,
  CDA_APPIID
} comp_decomp_action_t;


/*--------------------------------------------------*/
/*
  Rule bytecode:
  <rule ID (4)>
  <fid namespace (1)>
  <fid (1)>
  <fid position (1)>
  <direction (1)>
  <matching operator (1)>
  <matching operator data position (2)>
 */
/*--------------------------------------------------*/
    
#ifdef __cplusplus
}
#endif
    
/*---------------------------------------------------------------------------*/

#endif  /* RULE_ENGINE_H */

