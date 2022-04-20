#pragma once

#define PACKED __attribute__((packed))
#define ALIGNED(x) __attribute__((aligned(x)))
#define OPTIMIZE(x)  __attribute__((optimize(x)))

typedef unsigned char bool;
#define false 0
#define true  1

typedef signed char   int8_t;
typedef short int     int16_t;
typedef int           int32_t;
typedef long long int int64_t;

typedef unsigned char          uint8_t;
typedef unsigned short int     uint16_t;
typedef unsigned int           uint32_t;
typedef unsigned long long int uint64_t;

typedef signed char   int_least8_t;
typedef short int     int_least16_t;
typedef int           int_least32_t;
typedef long long int int_least64_t;

typedef unsigned char          uint_least8_t;
typedef unsigned short int     uint_least16_t;
typedef unsigned int           uint_least32_t;
typedef unsigned long long int uint_least64_t;

typedef signed char                int_fast8_t;
typedef int                        int_fast16_t;
typedef int                        int_fast32_t;
typedef long long int              int_fast64_t;

typedef unsigned char              uint_fast8_t;
typedef unsigned int               uint_fast16_t;
typedef unsigned int               uint_fast32_t;
typedef unsigned long long int     uint_fast64_t;

typedef int                         intptr_t;
typedef unsigned int                uintptr_t;

typedef long long int                intmax_t;
typedef unsigned long long int       uintmax_t;

#  define __INT64_C(c)        c ## LL
#  define __UINT64_C(c)        c ## ULL

# define INT8_MIN                (-128)
# define INT16_MIN                (-32767-1)
# define INT32_MIN                (-2147483647-1)
# define INT64_MIN                (-__INT64_C(9223372036854775807)-1)

# define INT8_MAX                (127)
# define INT16_MAX                (32767)
# define INT32_MAX                (2147483647)
# define INT64_MAX                (__INT64_C(9223372036854775807))

# define UINT8_MAX                (255)
# define UINT16_MAX                (65535)
# define UINT32_MAX                (4294967295U)
# define UINT64_MAX                (__UINT64_C(18446744073709551615))

# define INT_LEAST8_MIN                (-128)
# define INT_LEAST16_MIN        (-32767-1)
# define INT_LEAST32_MIN        (-2147483647-1)
# define INT_LEAST64_MIN        (-__INT64_C(9223372036854775807)-1)

# define INT_LEAST8_MAX                (127)
# define INT_LEAST16_MAX        (32767)
# define INT_LEAST32_MAX        (2147483647)
# define INT_LEAST64_MAX        (__INT64_C(9223372036854775807))

# define UINT_LEAST8_MAX        (255)
# define UINT_LEAST16_MAX        (65535)
# define UINT_LEAST32_MAX        (4294967295U)
# define UINT_LEAST64_MAX        (__UINT64_C(18446744073709551615))

# define INT_FAST8_MIN                (-128)
# define INT_FAST16_MIN        (-2147483647-1)
# define INT_FAST32_MIN        (-2147483647-1)
# define INT_FAST64_MIN                (-__INT64_C(9223372036854775807)-1)

# define INT_FAST8_MAX                (127)
# define INT_FAST16_MAX        (2147483647)
# define INT_FAST32_MAX        (2147483647)
# define INT_FAST64_MAX                (__INT64_C(9223372036854775807))

# define UINT_FAST8_MAX                (255)
# define UINT_FAST16_MAX        (4294967295U)
# define UINT_FAST32_MAX        (4294967295U)
# define UINT_FAST64_MAX        (__UINT64_C(18446744073709551615))

# define INTMAX_MIN                (-__INT64_C(9223372036854775807)-1)
# define INTMAX_MAX                (__INT64_C(9223372036854775807))
# define UINTMAX_MAX                (__UINT64_C(18446744073709551615))

# define SIG_ATOMIC_MIN                (-2147483647-1)
# define SIG_ATOMIC_MAX                (2147483647)
#  define SIZE_MAX                (4294967295U)

# define WINT_MIN                (0u)
# define WINT_MAX                (4294967295u)

# define INT8_C(c)        c
# define INT16_C(c)        c
# define INT32_C(c)        c
#  define INT64_C(c)        c ## LL

# define UINT8_C(c)        c
# define UINT16_C(c)        c
# define UINT32_C(c)        c ## U
#  define UINT64_C(c)        c ## ULL

#  define INTMAX_C(c)        c ## LL
#  define UINTMAX_C(c)        c ## ULL

#define LOW_16(x) ((uint16_t)(x))
#define HIGH_16(x) ((uint16_t)((x) >> 16))

static inline bool bit_addr_from_byte(uint8_t to_convert, int to_return)
{
    int mask = 1 << (to_return - 1);
    return ((to_convert & mask) != 0);
}