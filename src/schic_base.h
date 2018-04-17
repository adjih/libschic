/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef SCHIC_BASE_H
#define SCHIC_BASE_H

/*---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

#define BEGIN_MACRO do {
#define END_MACRO } while(0)

#ifndef DEBUG
#define DEBUG(...) BEGIN_MACRO printf(__VA_ARGS__); END_MACRO
#endif

#define BITS_PER_BYTE (8)

/*---------------------------------------------------------------------------*/

// XXX:move
static inline bool u32_bitsize_lower_than(uint32_t value, unsigned int nb_bit)
{
    if (nb_bit >= sizeof(uint32_t)*BITS_PER_BYTE) {
        return true;
    }
    uint32_t mask = (1 << ((uint32_t)(nb_bit)))-1;
    return (value & mask) == value;
}

/*--------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif  /* SCHIC_BASE_H */
