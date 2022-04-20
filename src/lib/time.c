#include <lib/time.h>
#include <kernel/core/kernel.h>

char* timestr(time_t time, bool military, bool seconds)
{
    char* str = tmalloc(4096, MEMTYPE_STRING);

    // temp vars
    char temp[64];
    memset(temp, 0, 64);

    // 24-hour
    if (military)
    {
        if (time.hour < 10) { strcat(str, "0"); } 
        strcat(str, ltoa(time.hour, temp, 10));
    }
    // 12-hour
    else
    {
        int hour = time.hour;
        if (hour > 12) { hour -= 12; }
        if (time.hour < 10) { strcat(str, "0"); }
        strcat(str, ltoa(hour, temp, 10));
    }
    
    // clear temp and add colon
    memset(temp, 0, 64);
    strcat(str, ":");

    // minute
    if (time.minute < 10) { strcat(str, "0"); }
    strcat(str, ltoa(time.minute, temp, 10));

    // clear temp
    memset(temp, 0, 64);

    // seconds
    if (seconds)
    {
        strcat(str, ":");
        if (time.second < 10) { strcat(str, "0"); }
        strcat(str, ltoa(time.second, temp, 10));
    }

    // am/pm
    if (!military)
    { if (time.hour >= 12) { strcat(str, " PM"); } else { strcat(str, " AM"); } }

    // return output string
    return str;
}

time_t timenow() { return rtc_get_time(); }