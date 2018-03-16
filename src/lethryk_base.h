/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#ifndef LETHRYK_BASE_H
#define LETHRYK_BASE_H

/*---------------------------------------------------------------------------*/

#include <stdio.h>

#define BEGIN_MACRO do {
#define END_MACRO } while(0)

#ifndef DEBUG
#define DEBUG(...) BEGIN_MACRO printf(__VA_ARGS__); END_MACRO
#endif

/*---------------------------------------------------------------------------*/

#endif  /* LETHRYK_BASE_H */
