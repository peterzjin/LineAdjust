/**************************************************************************************************  
 * @file hal_types.h
 * @brief .
 **************************************************************************************************/

#ifndef HAL_TYPES_H
#define HAL_TYPES_H


#ifdef __cplusplus
extern "C"
{
#endif
/*
** Includes
*/

/*
** Macro Definitions
*/
#define _ix86_  1
/*
** Define compiler specific macros
** The __extension__ compiler pragma is required
** for the uint64 type using GCC with the ANSI C90 standard.
** Other macros can go in here as needed, for example alignment 
** pragmas.
*/
#if defined (__GNUC__)
   #define _EXTENSION_    __extension__
   #define OS_PACK        __attribute__ ((packed))
   #define OS_ALIGN(n)  __attribute__((aligned(n)))
#else
   #define _EXTENSION_ 
   #define OS_PACK
   #define OS_ALIGN(n) 
#endif

#if defined(_ix86_)
/* ----------------------- Intel x86 processor family -------------------------*/
  /* Little endian */
  #undef   _STRUCT_HIGH_BIT_FIRST_
  #define  _STRUCT_LOW_BIT_FIRST_


  typedef unsigned char                         boolean;
  typedef signed char                           int8;
  typedef short int                             int16;
  typedef long int                              int32;
  typedef unsigned char                         uint8;
  typedef unsigned short int                    uint16;
  typedef unsigned long int                     uint32;

  _EXTENSION_ typedef unsigned long long int    uint64;


//  typedef unsigned char                         bool;


#elif defined(__PPC__)
   /* ----------------------- Motorola Power PC family ---------------------------*/
   /* The PPC can be programmed to be big or little endian, we assume native */
   /* Big endian */
   #define _STRUCT_HIGH_BIT_FIRST_
   #undef  _STRUCT_LOW_BIT_FIRST_

   typedef unsigned char                        boolean;
   typedef signed char                          int8;
   typedef short int                            int16;
   typedef long int                             int32;
   typedef unsigned char                        uint8;
   typedef unsigned short int                   uint16;
   typedef unsigned long int                    uint32;
   _EXTENSION_ typedef unsigned long long int   uint64;

#elif defined(_m68k_)
   /* ----------------------- Motorola m68k/Coldfire family ---------------------------*/
   /* Big endian */
   #define _STRUCT_HIGH_BIT_FIRST_
   #undef  _STRUCT_LOW_BIT_FIRST_

   typedef unsigned char                        boolean;
   typedef signed char                          int8;
   typedef short int                            int16;
   typedef long int                             int32;
   typedef unsigned char                        uint8;
   typedef unsigned short int                   uint16;
   typedef unsigned long int                    uint32;
   _EXTENSION_ typedef unsigned long long int   uint64;

#elif defined(__SPARC__)
   /* ----------------------- SPARC/LEON family ---------------------------*/
   /* SPARC Big endian */
   #define _STRUCT_HIGH_BIT_FIRST_
   #undef  _STRUCT_LOW_BIT_FIRST_

   typedef unsigned char                        boolean;
   typedef signed char                          int8;
   typedef short int                            int16;
   typedef long int                             int32;
   typedef unsigned char                        uint8;
   typedef unsigned short int                   uint16;
   typedef unsigned long int                    uint32;
   _EXTENSION_ typedef unsigned long long int   uint64;

#else  /* not any of the above */
   #error undefined processor
#endif  /* processor types */

#ifndef NULL              /* pointer to nothing */
   #define NULL ((void *) 0)
#endif

#ifndef TRUE              /* Boolean true */
   #define TRUE (1)
#endif

#ifndef FALSE              /* Boolean false */
   #define FALSE (0)
#endif
#ifdef __cplusplus
}
#endif
#endif  /* HAL_TYPES_H */
