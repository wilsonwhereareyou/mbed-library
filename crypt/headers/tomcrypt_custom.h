#ifndef TOMCRYPT_CUSTOM_H_
#define TOMCRYPT_CUSTOM_H_

/* macros for various libc functions you can change for embedded targets */
#ifndef XMALLOC
   #ifdef malloc
   #define LTC_NO_PROTOTYPES
   #endif
#define XMALLOC  malloc
#endif
#ifndef XREALLOC
   #ifdef realloc
   #define LTC_NO_PROTOTYPES
   #endif
#define XREALLOC realloc
#endif
#ifndef XCALLOC
   #ifdef calloc
   #define LTC_NO_PROTOTYPES
   #endif
#define XCALLOC  calloc
#endif
#ifndef XFREE
   #ifdef free
   #define LTC_NO_PROTOTYPES
   #endif
#define XFREE    free
#endif

#ifndef XMEMSET
   #ifdef memset
   #define LTC_NO_PROTOTYPES
   #endif
#define XMEMSET  memset
#endif
#ifndef XMEMCPY
   #ifdef memcpy
   #define LTC_NO_PROTOTYPES
   #endif
#define XMEMCPY  memcpy
#endif
#ifndef XMEMCMP
   #ifdef memcmp
   #define LTC_NO_PROTOTYPES
   #endif
#define XMEMCMP  memcmp
#endif
#ifndef XSTRCMP
   #ifdef strcmp
   #define LTC_NO_PROTOTYPES
   #endif
#define XSTRCMP strcmp
#endif

#ifndef XCLOCK
#define XCLOCK   clock
#endif
#ifndef XCLOCKS_PER_SEC
#define XCLOCKS_PER_SEC CLOCKS_PER_SEC
#endif

#ifndef XQSORT
   #ifdef qsort
   #define LTC_NO_PROTOTYPES
   #endif
#define XQSORT qsort
#endif

/* CUSTOM DEFINES *************************************************************/
#define LTC NO FILE
#define TFM_DESC

/* Easy button? */
#ifdef LTC_EASY
   #define LTC_NO_CIPHERS
   #define LTC_RIJNDAEL
   #define LTC_BLOWFISH
   #define LTC_DES
   #define LTC_CAST5

   #define LTC_NO_MODES
   #define LTC_ECB_MODE
   #define LTC_CBC_MODE
   #define LTC_CTR_MODE

   #define LTC_NO_HASHES
   #define LTC_SHA1
   #define LTC_SHA512
   #define LTC_SHA384
   #define LTC_SHA256
   #define LTC_SHA224

   #define LTC_NO_MACS
   #define LTC_HMAC
   #define LTC_OMAC
   #define LTC_CCM_MODE

   #define LTC_NO_PRNGS
   #define LTC_SPRNG
   #define LTC_YARROW
   #define LTC_DEVRANDOM
   #define TRY_URANDOM_FIRST

   #define LTC_NO_PK
   #define LTC_MRSA
   #define LTC_MECC
#endif

/* Use small code where possible */
/* #define LTC_SMALL_CODE */

/* Enable self-test test vector checking */
#ifndef LTC_NO_TEST
   #define LTC_TEST
#endif

/* clean the stack of functions which put private information on stack */
/* #define LTC_CLEAN_STACK */

/* disable all file related functions */
/* #define LTC_NO_FILE */

/* disable all forms of ASM */
 #define LTC_NO_ASM

/* disable FAST mode */
/* #define LTC_NO_FAST */

/* disable BSWAP on x86 */
/* #define LTC_NO_BSWAP */

/* ---> Symmetric Block Ciphers <--- */
#define LTC_RIJNDAEL

#define LTC_NO_CIPHERS
#ifndef LTC_NO_CIPHERS

#define LTC_BLOWFISH
#define LTC_RC2
#define LTC_RC5
#define LTC_RC6
#define LTC_SAFERP
#define LTC_RIJNDAEL
#define LTC_XTEA
/* _TABLES tells it to use tables during setup, _SMALL means to use the smaller scheduled key format
 * (saves 4KB of ram), _ALL_TABLES enables all tables during setup */
#define LTC_TWOFISH
#ifndef LTC_NO_TABLES
   #define LTC_TWOFISH_TABLES
   /* #define LTC_TWOFISH_ALL_TABLES */
#else
   #define LTC_TWOFISH_SMALL
#endif
/* #define LTC_TWOFISH_SMALL */
/* LTC_DES includes EDE triple-LTC_DES */
#define LTC_DES
#define LTC_CAST5
#define LTC_NOEKEON
#define LTC_SKIPJACK
#define LTC_SAFER
#define LTC_KHAZAD
#define LTC_ANUBIS
#define LTC_ANUBIS_TWEAK
#define LTC_KSEED
#define LTC_KASUMI

#endif /* LTC_NO_CIPHERS */


/* ---> Block Cipher Modes of Operation <--- */
// Enable CBC mode
#define LTC_CBC_MODE

#define LTC_NO_MODES
#ifndef LTC_NO_MODES

#define LTC_CFB_MODE
#define LTC_OFB_MODE
#define LTC_ECB_MODE
#define LTC_CBC_MODE
#define LTC_CTR_MODE

/* F8 chaining mode */
#define LTC_F8_MODE

/* LRW mode */
#define LTC_LRW_MODE
#ifndef LTC_NO_TABLES
   /* like GCM mode this will enable 16 8x128 tables [64KB] that make
    * seeking very fast.
    */
   #define LRW_TABLES
#endif

/* XTS mode */
#define LTC_XTS_MODE

#endif /* LTC_NO_MODES */

/* ---> One-Way Hash Functions <--- */
#ifndef LTC_NO_HASHES

#undef LTC_CHC_HASH
#undef LTC_WHIRLPOOL
#undef LTC_SHA512
#undef LTC_SHA384
#undef LTC_SHA256
#undef LTC_SHA224
#undef LTC_TIGER
#define LTC_SHA1
#undef LTC_MD5
#undef LTC_MD4
#undef LTC_MD2
#undef LTC_RIPEMD128
#undef LTC_RIPEMD160
#undef LTC_RIPEMD256
#undef LTC_RIPEMD320

#endif /* LTC_NO_HASHES */

/* ---> MAC functions <--- */
#ifndef LTC_NO_MACS

#define LTC_HMAC
#undef LTC_OMAC
#undef LTC_PMAC
#undef LTC_XCBC
#undef LTC_F9_MODE
#undef LTC_PELICAN

#if defined(LTC_PELICAN) && !defined(LTC_RIJNDAEL)
   #error Pelican-MAC requires LTC_RIJNDAEL
#endif

/* ---> Encrypt + Authenticate Modes <--- */

#undef LTC_EAX_MODE
#if defined(LTC_EAX_MODE) && !(defined(LTC_CTR_MODE) && defined(LTC_OMAC))
   #error LTC_EAX_MODE requires CTR and LTC_OMAC mode
#endif

#undef LTC_OCB_MODE
#undef LTC_CCM_MODE
#undef LTC_GCM_MODE

/* Use 64KiB tables */
#define LTC_NO_TABLES
#ifndef LTC_NO_TABLES
   #define LTC_GCM_TABLES
#endif

/* USE SSE2? requires GCC works on x86_32 and x86_64*/
#ifdef LTC_GCM_TABLES
/* #define LTC_GCM_TABLES_SSE2 */
#endif

#endif /* LTC_NO_MACS */

/* Various tidbits of modern neatoness */
#define LTC_BASE64

/* --> Pseudo Random Number Generators <--- */
#ifndef LTC_NO_PRNGS

/* Yarrow */
#undef LTC_YARROW
/* which descriptor of AES to use?  */
/* 0 = rijndael_enc 1 = aes_enc, 2 = rijndael [full], 3 = aes [full] */
#define LTC_YARROW_AES 0

#if defined(LTC_YARROW) && !defined(LTC_CTR_MODE)
   #error LTC_YARROW requires LTC_CTR_MODE chaining mode to be defined!
#endif

/* a PRNG that simply reads from an available system source */
#undef LTC_SPRNG

/* The LTC_RC4 stream cipher */
#undef LTC_RC4

/* Fortuna PRNG */
#undef LTC_FORTUNA
/* reseed every N calls to the read function */
#define LTC_FORTUNA_WD    10
/* number of pools (4..32) can save a bit of ram by lowering the count */
#define LTC_FORTUNA_POOLS 32

/* Greg's LTC_SOBER128 PRNG ;-0 */
#undef LTC_SOBER128

/* the *nix style /dev/random device */
#undef LTC_DEVRANDOM
/* try /dev/urandom before trying /dev/random */
#undef TRY_URANDOM_FIRST

#endif /* LTC_NO_PRNGS */

/* ---> math provider? <--- */
#ifndef LTC_NO_MATH

/* LibTomMath */
/* #define LTM_LTC_DESC */

/* TomsFastMath */
/* #define TFM_LTC_DESC */

#endif /* LTC_NO_MATH */

/* ---> Public Key Crypto <--- */
#define LTC_NO_PK
#ifndef LTC_NO_PK

/* Include RSA support */
#define LTC_MRSA

/* Include Katja (a Rabin variant like RSA) */
/* #define MKAT */

/* Digital Signature Algorithm */
#define LTC_MDSA

/* ECC */
#define LTC_MECC

/* use Shamir's trick for point mul (speeds up signature verification) */
#define LTC_ECC_SHAMIR

#if defined(TFM_LTC_DESC) && defined(LTC_MECC)
   #define LTC_MECC_ACCEL
#endif

/* do we want fixed point ECC */
/* #define LTC_MECC_FP */

/* Timing Resistant? */
/* #define LTC_ECC_TIMING_RESISTANT */

#endif /* LTC_NO_PK */

/* LTC_PKCS #1 (RSA) and #5 (Password Handling) stuff */
#define LTC_NO_PKCS
#ifndef LTC_NO_PKCS

#define LTC_PKCS_1
#define LTC_PKCS_5

/* Include ASN.1 DER (required by DSA/RSA) */
#undef LTC_DER

#endif /* LTC_NO_PKCS */

/* cleanup */

#ifdef LTC_MECC
/* Supported ECC Key Sizes */
#ifndef LTC_NO_CURVES
   #define ECC112
   #define ECC128
   #define ECC160
   #define ECC192
   #define ECC224
   #define ECC256
   #define ECC384
   #define ECC521
#endif
#endif

#if defined(LTC_MECC) || defined(LTC_MRSA) || defined(LTC_MDSA) || defined(MKATJA)
   /* Include the MPI functionality?  (required by the PK algorithms) */
   #define MPI
#endif

#ifdef LTC_MRSA
   #define LTC_PKCS_1
#endif

#if defined(LTC_DER) && !defined(MPI)
   #error ASN.1 DER requires MPI functionality
#endif

#if (defined(LTC_MDSA) || defined(LTC_MRSA) || defined(LTC_MECC) || defined(MKATJA)) && !defined(LTC_DER)
   #error PK requires ASN.1 DER functionality, make sure LTC_DER is enabled
#endif

/* THREAD management */
#ifdef LTC_PTHREAD

#include <pthread.h>

#define LTC_MUTEX_GLOBAL(x)   pthread_mutex_t x = PTHREAD_MUTEX_INITIALIZER;
#define LTC_MUTEX_PROTO(x)    extern pthread_mutex_t x;
#define LTC_MUTEX_TYPE(x)     pthread_mutex_t x;
#define LTC_MUTEX_INIT(x)     pthread_mutex_init(x, NULL);
#define LTC_MUTEX_LOCK(x)     pthread_mutex_lock(x);
#define LTC_MUTEX_UNLOCK(x)   pthread_mutex_unlock(x);

#else

/* default no functions */
#define LTC_MUTEX_GLOBAL(x)
#define LTC_MUTEX_PROTO(x)
#define LTC_MUTEX_TYPE(x)
#define LTC_MUTEX_INIT(x)
#define LTC_MUTEX_LOCK(x)
#define LTC_MUTEX_UNLOCK(x)

#endif

/* Debuggers */

/* define this if you use Valgrind, note: it CHANGES the way SOBER-128 and LTC_RC4 work (see the code) */
/* #define LTC_VALGRIND */

#endif



/* $Source: /cvs/libtom/libtomcrypt/src/headers/tomcrypt_custom.h,v $ */
/* $Revision: 47287 $ */
/* $Date: 2007/05/12 14:37:41 $ */
