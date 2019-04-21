#ifndef __CTYPE_H
#define __CTYPE_H

//****************************************************************************
//**
//**    ctype.h
//**    - character macros
//**
//****************************************************************************
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

/* Constants */

#define CT_UP 0x01  /* upper case */
#define CT_LOW 0x02 /* lower case */
#define CT_DIG 0x04 /* digit */
#define CT_CTL 0x08 /* control */
#define CT_PUN 0x10 /* punctuation */
#define CT_WHT 0x20 /* white space (space/cr/lf/tab) */
#define CT_HEX 0x40 /* hex digit */
#define CT_SP 0x80  /* hard space (0x20) */

/* Basic macros */

#define isspace(c) ((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define isupper(c) ((c) >= 'A' && (c) <= 'Z')
#define islower(c) ((c) >= 'a' && (c) <= 'z')
#define isalpha(c) (isupper(c) || islower(c))
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isxdigit(c) (isdigit(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
#define isprint(c) ((c) >= ' ' && (c) <= '~')
#define toupper(c) ((c)-0x20 * (((c) >= 'a') && ((c) <= 'z')))
#define tolower(c) ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
#define isascii(c) ((unsigned)(c) <= 0x7F)
#define toascii(c) ((unsigned)(c)&0x7F)

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================
//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END ctype.h
//**
//****************************************************************************

#endif