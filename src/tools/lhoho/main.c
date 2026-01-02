#include <stdio.h>
#include <version.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "tree.h"
#include "countdown.h"

void printHelp(char* name)
{
    fprintf(stdout, "Usage: %s <command> (<args>)\n", name);

    fputc('\n', stdout);
    fputs("Commands:\n", stdout);
    fputs("> tree (<height>)       Print a tree\n", stdout);
    fputs("> countdown             Prints the time until Christmas\n", stdout);
}

int main(int argc, const char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printVersion();
            return 0;
        }
    }

    if (argc < 2) {
        printHelp(argv[0]);
        return 1;
    }

    int color = 1;
    int dense = 1;
    int ornaments = 1;

    srand(time(NULL));

    const char* command = argv[1];

    if (strcmp(command, "tree") == 0) {
        int height = 10;
        if (argc > 2) {
            const char* height_str = argv[2];
            height = atoi(height_str);

            if (height <= 0) {
                fprintf(stderr, "Invalid Height: %i\n", height);
                return 1;
            }
        }
        int trunkHeight = (height + 2) / 3;
        int trunkWidth = ((height + 3) / 10) * 2 + 1;

        if (printTree(height, trunkHeight, trunkWidth, color, dense, ornaments) != 0) {
            fputs("Error: Couldn't print a tree :(\n", stdout);
            return 1;
        }
    } else if (strcmp(command, "countdown") == 0) {
        time_t now = time(NULL);
        struct tm* current_tm = localtime(&now);

        struct tm christmas_eve_tm = {0};
        christmas_eve_tm.tm_year = current_tm->tm_year;
        christmas_eve_tm.tm_mon = 11;   // December
        christmas_eve_tm.tm_mday = 24;  // 24th
        christmas_eve_tm.tm_hour = 18;  // 6:00 PM / 18:00
        christmas_eve_tm.tm_min = 0;
        christmas_eve_tm.tm_sec = 0;
        christmas_eve_tm.tm_isdst = -1;
        time_t christmas_eve = mktime(&christmas_eve_tm);

        struct tm christmas_tm = {0};
        christmas_tm.tm_year = current_tm->tm_year;
        christmas_tm.tm_mon = 11;   // December
        christmas_tm.tm_mday = 25;  // 25th
        christmas_tm.tm_hour = 0;   // 00:00
        christmas_tm.tm_min = 0;
        christmas_tm.tm_sec = 0;
        christmas_tm.tm_isdst = -1;
        time_t christmas = mktime(&christmas_tm);

        time_t seconds_to_christmas_eve = christmas_eve - now;
        printCountdown("Christmas Eve (Dec 24, 18:00)", seconds_to_christmas_eve);

        time_t seconds_to_christmas = christmas - now;
        printCountdown("Christmas (Dec 25, 00:00)", seconds_to_christmas);
    }

    else {
        fprintf(stderr, "Unknown command '%s'\n", command);
        return 1;
    }

    return 0;
}
