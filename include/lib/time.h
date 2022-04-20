#pragma once
#include <lib/stdint.h>

typedef struct
{
    uint16_t millisecond;       // max = 1000
    uint8_t  second;            // max = 60
    uint8_t  minute;            // max = 60
    uint8_t  hour;              // max = 24
    uint8_t  day;               // max = 31
    uint8_t  week_day;          // max = 6
    uint8_t  month;             // max = 12
    uint16_t year;              // max = 65536
} PACKED time_t;

/// @brief Get time string @param time Time value @param military Use 24-hour format @param seconds Show seconds @return Allocated string of time
char* timestr(time_t time, bool military, bool seconds);

time_t timenow();