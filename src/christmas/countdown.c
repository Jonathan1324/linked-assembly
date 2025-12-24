#include "countdown.h"
#include <stdio.h>

void printCountdown(const char* label, time_t seconds)
{
    if (seconds < 0) {
        printf("%s has already passed!\n", label);
        return;
    }
    int days    = seconds / (24*3600);
    int hours   = (seconds % (24*3600)) / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs    = seconds % 60;
    
    if (days > 0) printf("%d %s, ", days, (days == 1) ? "day" : "days");
    if (hours > 0 || days > 0) printf("%d %s, ", hours, (hours == 1) ? "hour" : "hours");
    printf("%d %s, %d %s remaining until %s!\n",
           minutes, (minutes == 1) ? "minute" : "minutes",
           secs, (secs == 1 ? "second" : "seconds"),
           label);
}
