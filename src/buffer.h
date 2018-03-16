/*---------------------------------------------------------------------------
 * Buffer related functions
 * Copied from contiki-senslab-unified/hipercom/node_ui/buffer.h 
 * by C.A., 16 June 2013
 * March 2018: copied from ~/HgRep/NC-iotlab/ns-3.16-hana/nctools/src was:
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih
 * Copyright 2013-2014 Inria
 * All rights reserved. Distributed only with permission.
 *---------------------------------------------------------------------------*/

/** \addtogroup core 
    @{ */

/**
 * \defgroup buffer 
 * @{
 *
 * \file 
 *   Buffer related functions:
 *     A buffer is a sequence of bytes, in which one can either push or pop: 
 *     block of bytes, unsigned 8-bit integers, unsigned 16-bit
 *     integers or unsigned 32-bit integers, in network byte order.
 *
 *     These are used for packet generation and parsing.
 *     
 *     Operations on unstructured data are the macros BLOCK_...
 *     Operations on 'struct' buffer are the macros BUFFER_...
 *     and equivalent inline functions are available as buffer_...
 *
 * \author Cedric Adjih <cedric.adjih@inria.fr>
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

/*---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
  
#include "lethryk_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------*/

#define BITS_PER_BYTE (8)

#define BUFFER_SIZE_U8  (1)
#define BUFFER_SIZE_U16 (2)
#define BUFFER_SIZE_U32 (4)        

/*---------------------------------------------------------------------------*/

/* internal macros */

/** \brief Internal macros */
#define BLOCK_ERROR(err) BEGIN_MACRO err; END_MACRO
#define BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, added_data_size, err) \
  BEGIN_MACRO  \
  if ( (pos)+(added_data_size) > (max_data_size)) { BLOCK_ERROR(err); } \
  else

/*---------------------------------------------------------------------------*/

/** \brief Put an unsigned 16 bit integer at pointer \a data */
#define DIRECT_PUT_U16(data, value)					\
  BEGIN_MACRO {								\
    (data)[0] = (((uint16_t)(value))>>8) & 0xffu;			\
    (data)[1] = ((uint16_t)(value)) & 0xffu;				\
  } END_MACRO

/** \brief Get an unsigned 16 bit integer at pointer \a data */
#define DIRECT_GET_U16(data)				    \
  ((((uint16_t)((data)[0])) << 8)			    \
   |((uint16_t)((data)[1])))

/** \brief Put an unsigned 32 bit integer at pointer \a data */
#define DIRECT_PUT_U32(data, value)			    \
  BEGIN_MACRO {						    \
    (data)[0] = (((uint32_t)(value))>>(3*8)) & 0xffu;	    \
    (data)[1] = (((uint32_t)(value))>>(2*8)) & 0xffu;	    \
    (data)[2] = (((uint32_t)(value))>>(1*8)) & 0xffu;	    \
    (data)[3] = ((uint32_t)(value)) & 0xffu;		    \
  } END_MACRO

/** \brief Get an unsigned 32 bit integer at pointer \a data */
#define DIRECT_GET_U32(data)				    \
  ( (((uint32_t)((data)[0])) << (3*8))			    \
    | (((uint32_t)((data)[1])) << (2*8))		    \
    | (((uint32_t)((data)[2])) << (1*8))		    \
    | ((uint32_t)((data)[3])))

/*---------------------------------------------------------------------------*/

/** \brief Append some data to a block */
#define BLOCK_PUT_DATA(data, max_data_size, pos,			\
		       added_data, added_data_size, err)		\
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, added_data_size, err) { 	\
    memcpy((data)+(pos), (added_data), (added_data_size));		\
    pos += (added_data_size);						\
  } END_MACRO

/** \brief Append one unsigned byte to a block */
#define BLOCK_PUT_U8(data, max_data_size, pos, value, err)  \
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, 1, err) {	    \
    (data)[(pos)] = (uint8_t)(value);			    \
    pos ++;						    \
  } END_MACRO;

/** \brief Append one unsigned short to a block */
#define BLOCK_PUT_U16(data, max_data_size, pos, value, err)	    \
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, 2, err) {		    \
    DIRECT_PUT_U16( ((data)+(pos)),  value);			    \
    pos +=2 ;							    \
  } END_MACRO;

/** \brief Append one unsigned 32 bit integer to a block */
#define BLOCK_PUT_U32(data, max_data_size, pos, value, err)	    \
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, 4, err) {		    \
    DIRECT_PUT_U32( ((data)+(pos)),  value);			    \
    pos +=4 ;							    \
  } END_MACRO;

/*--------------------------------------------------*/

/** \brief Get some data from a block */
#define BLOCK_GET_DATA(popped_data, popped_data_size,\
		       data, max_data_size, pos,   \
		       err)						\
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, popped_data_size, err) { 	\
    memcpy((popped_data), (data)+(pos), (popped_data_size));		\
    pos += (popped_data_size);						\
  } END_MACRO

/** \brief Pop one unsigned byte from a block */
#define BLOCK_GET_U8(value, data, max_data_size, pos, err)	\
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, 1, err) {		\
    value = (data)[(pos)];					\
    pos ++;							\
  } END_MACRO;

/** \brief Pop one unsigned 16 bit integer from a block */
#define BLOCK_GET_U16(value, data, max_data_size, pos, err)	    \
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, 2, err) {		    \
    value = DIRECT_GET_U16( (data)+(pos) );			    \
    pos +=2 ;							    \
  } END_MACRO;

/** \brief Pop one unsigned 32 bit integer from a block */
#define BLOCK_GET_U32(value, data, max_data_size, pos, err)	    \
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, 4, err) {		    \
    value = DIRECT_GET_U32( (data)+(pos));			    \
    pos +=4 ;							    \
  } END_MACRO;


/** \brief Get some data from a block */
#define BLOCK_PEEK_DATA(popped_data_size,\
			data, max_data_size, pos,			\
			err)						\
  BEGIN_MACRO_CHECK_SIZE(max_data_size, pos, popped_data_size, err) { 	\
    pos += (popped_data_size);						\
  } END_MACRO


/*---------------------------------------------------------------------------*/

/**
 * \struct buffer_t
 * \brief A buffer: it is a sequence of bytes starting from pointer `data'
 *   with (maximum) size `capacity' and where `position' acts as a cursor.
 *
 *   It must be initialized by `BUFFER_INIT' or `buffer_init'
 *   (then `position' equals 0 and `has_bound_error' is reset to false)
 *
 *   - One can push and pop data, with BUFFER_PUT_... or BUFFER_GET_... macros;
 *   with these functions, the current `position' is properly incremented,
 *   and tracks the last unused byte.
 *   - One can alternatively push and pop data, with buffer_put_... or 
 *   buffer_get_... functions (instead of macros) ; and in this case, 
 *   bounds errors are handled (moreover they are checked with unit tests).
 */

typedef struct {
  uint8_t* data;            /**< pointer to underlying block of memory */
  unsigned int capacity;    /**< maximum amount of bytes associated to `data' */
  unsigned int position;    /**< current position (for next put/get data) */ 
  bool   has_bound_error; /**< true iff an overflow was detected */
} buffer_t;

/*--------------------------------------------------*/

/** \brief Initialize the content of one buffer */
#define BUFFER_INIT(buffer, data, capacity)	    \
  BEGIN_MACRO { 				    \
    (buffer).data = (data);			    \
    (buffer).capacity = (capacity);		    \
    (buffer).position = 0;			    \
  } END_MACRO

/*--------------------------------------------------*/

/**
 * \name BUFFER_PUT_... macros
 * @{
 */

/** \brief Append a sequence of bytes a buffer */
#define BUFFER_PUT_DATA(buffer, added_data, added_data_size, err) \
  BLOCK_PUT_DATA((buffer).data, (buffer).capacity, (buffer).position, \
		 (added_data), (added_data_size), err)

/** \brief Append one byte a buffer */
#define BUFFER_PUT_U8(buffer, value, err) \
  BLOCK_PUT_U8((buffer).data, (buffer).capacity, (buffer).position,	\
	       (value), err)

/** \brief Append a 16-bit unsigned integer to a buffer */
#define BUFFER_PUT_U16(buffer, value, err) \
  BLOCK_PUT_U16((buffer).data, (buffer).capacity, (buffer).position,	\
	       (value), err)

/** \brief Append a 32-bit unsigned integer to a buffer */
#define BUFFER_PUT_U32(buffer, value, err) \
  BLOCK_PUT_U32((buffer).data, (buffer).capacity, (buffer).position,	\
	       (value), err)

/** @} */

/*--------------------------------------------------*/

#define BUFFER_GET_DATA(popped_data, popped_data_size, buffer, err)    \
  BLOCK_GET_DATA(popped_data, popped_data_size, (buffer).data, \
		 (buffer).capacity, (buffer).position, err)

#define BUFFER_GET_U8(value, buffer, err)				\
  BLOCK_GET_U8(value, (buffer).data, (buffer).capacity, (buffer).position, \
	       err)

#define BUFFER_GET_U16(value, buffer, err)				\
  BLOCK_GET_U16(value, (buffer).data, (buffer).capacity, \
		(buffer).position, err)

#define BUFFER_GET_U32(value, buffer, err)				\
  BLOCK_GET_U32(value, (buffer).data, (buffer).capacity,		\
		(buffer).position, err)

/*---------------------------------------------------------------------------*/

/**
 * \name buffer__... inline functions
 * @{
 */

static inline void buffer_init(buffer_t* buffer, uint8_t* data, 
			       unsigned int capacity)
{ 
  BUFFER_INIT( (*buffer), data, capacity); 
  buffer->has_bound_error = false;
}

static inline void buffer_init_from_part(buffer_t* buffer, 
					 buffer_t* super_buffer,
					 unsigned int size)
{ 
  if (super_buffer->capacity - super_buffer->position + 1 < size) {
    super_buffer->position = super_buffer->capacity;
    super_buffer->has_bound_error = true;
    buffer->data = NULL;
    buffer->capacity = 0;
    buffer->position = 0;
    buffer->has_bound_error = true;
    return;
  }

  buffer_init(buffer, super_buffer->data + super_buffer->position, size);
  super_buffer->position += size;
}


static inline void buffer_put_data(buffer_t* buffer, 
				   uint8_t* added_data, 
				   unsigned int added_data_size)
{ BUFFER_PUT_DATA( (*buffer), added_data, added_data_size, 
		  (buffer)->has_bound_error = true); }


#define BUFFER_PEEK_DATA(popped_data_size, buffer, err)		\
  BLOCK_PEEK_DATA(popped_data_size, (buffer).data,		\
		  (buffer).capacity, (buffer).position, err)

static inline uint8_t* buffer_peek_data(buffer_t* buffer, 
					unsigned int data_size)
{ 
  if (buffer->position+data_size <= buffer->capacity) { 
    uint8_t* result = buffer->data + buffer->position;
    buffer->position += data_size;
    return result;
  } else {
    buffer->has_bound_error = true;
    return NULL;
  }
}

static inline void buffer_put_u8(buffer_t* buffer, uint8_t value)
{ BUFFER_PUT_U8( (*buffer), value, (buffer)->has_bound_error = true); }

static inline void buffer_put_u16(buffer_t* buffer, uint16_t value)
{ BUFFER_PUT_U16( (*buffer), value, (buffer)->has_bound_error = true); }

static inline void buffer_put_u32(buffer_t* buffer, uint32_t value)
{ BUFFER_PUT_U32( (*buffer), value, (buffer)->has_bound_error = true); }


static inline void buffer_get_data
(buffer_t* buffer, uint8_t* popped_data, unsigned int popped_data_size)
{ BUFFER_GET_DATA( popped_data, popped_data_size, (*buffer),
		  (buffer)->has_bound_error = true); }

static inline uint8_t buffer_get_u8(buffer_t* buffer)
{ 
  uint8_t result;
  BUFFER_GET_U8(result, (*buffer), 
		(buffer)->has_bound_error = true; result=0); 
  return result;
}

static inline uint16_t buffer_get_u16(buffer_t* buffer)
{ 
  uint16_t result;
  BUFFER_GET_U16(result, (*buffer), 
		 (buffer)->has_bound_error = true; result=0); 
  return result;
}

/** \brief */
static inline uint32_t buffer_get_u32(buffer_t* buffer)
{ 
  uint32_t result;
  BUFFER_GET_U32(result, (*buffer), 
		 (buffer)->has_bound_error = true; result=0); 
  return result;
}

static inline size_t buffer_get_position(buffer_t* buffer)
{ return buffer->position; }

#if 0
// XXX: remove
static inline size_t buffer_get_bit_position(buffer_t* buffer)
{ return buffer->position * BITS_PER_BYTE; }
#endif

static inline size_t buffer_get_available(buffer_t* buffer)
{
  if (!buffer->has_bound_error)
    return buffer->capacity - buffer->position;
  else return 0;
}

/** @} */

/*---------------------------------------------------------------------------*/

typedef struct {
  uint8_t position;
} buffer_mark_t;

static inline buffer_mark_t buffer_put_mark_u8(buffer_t* buffer)
{
  buffer_mark_t result;
  result.position = buffer->position;
  buffer_put_u8(buffer, 0);
  return result;
}

static inline void buffer_put_size_at_mark_u8(buffer_t* buffer, buffer_mark_t mark, 
				int offset)
{
  uint8_t value = buffer->position - mark.position + offset - 1;
  if (mark.position >= buffer->capacity) {
    buffer->has_bound_error = true;
    return;
  }
  buffer->data[mark.position] = value;
}

/*---------------------------------------------------------------------------*/

typedef struct {
    buffer_t *buffer;
    uint32_t  pending;
    size_t    pending_bit_count;
} bit_buffer_t;


void bit_buffer_init(bit_buffer_t *bit_buffer, buffer_t *base_buffer);
uint8_t bit_buffer_get_bit(bit_buffer_t *bit_buffer);
void bit_buffer_put_bit(bit_buffer_t *bit_buffer, uint8_t bit);
uint32_t bit_buffer_get_several(bit_buffer_t *bit_buffer, size_t bit_count);
void bit_buffer_put_several(bit_buffer_t *bit_buffer,
                            uint32_t bit_block, size_t bit_count);
size_t bit_buffer_get_content_bitsize(bit_buffer_t *bit_buffer);
size_t bit_buffer_get_available_bitsize(bit_buffer_t *bit_buffer);
void bit_buffer_copy_several(bit_buffer_t *to_bit_buffer,
                             bit_buffer_t *from_bit_buffer,
                             size_t bit_count);
void bit_buffer_put_data(bit_buffer_t *bit_buffer,
                         uint8_t* data, size_t data_size);

/*---------------------------------------------------------------------------*/

/** @} */ 
/** @} */ 

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_H__ */
