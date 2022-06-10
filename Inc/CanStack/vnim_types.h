#if !defined (VNIM_TYPES_H)
#define VNIM_TYPES_H

/* =============================================================================
//
//  Name:           vnim_types.h
//
//  Description:    This file defines common type definitions, defined
//                  constants and function mimicking macros.
//
// ========================================================================== */

/* =============================================================================
//
//  Include Section
//
// ========================================================================== */

/* =============================================================================
//
//  Public Defined Constants Section
//
// ========================================================================== */

#if !defined (VNIM_NULL)

#define VNIM_NULL    ((void *)(0))

#endif /* NULL */


#define VNIM_YES       (1)
#define VNIM_NO        (2)

#define VNIM_ENABLED   (1)
#define VNIM_DISABLED  (2)

/* =============================================================================
//
//  Public Type Definitions Section
//
// ========================================================================== */

typedef unsigned char   vnim_bit_type;
typedef unsigned char   vnim_unsigned8;
typedef signed char     vnim_signed8;

typedef unsigned int  vnim_unsigned16;
typedef signed int    vnim_signed16;

typedef unsigned long    vnim_unsigned32;
typedef signed long      vnim_signed32;

typedef unsigned char     vnim_message_id_type;
typedef unsigned int    signal_id_type;

#if defined (__cplusplus)

#define VNIM_FALSE   false
#define VNIM_TRUE    true

typedef bool VNIM_BOOL;

#else

#define VNIM_FALSE   (0)
#define VNIM_TRUE    (1)

typedef vnim_unsigned8 VNIM_BOOL;

#endif /* __cplusplus */


typedef void (*vnim_function_ptr_type)(void);

#endif /* VNIM_TYPES_H =========================================================== */
