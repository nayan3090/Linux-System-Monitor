#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{
    string str_time[3];
    int time[3];
    time[0] = seconds / 3600;
    seconds = seconds % 3600;
    time[1] = seconds / 60;
    time[2] = seconds % 60;

    for (int i = 0; i < 3; i++)
    {
        if (time[i] < 10)
        {
            str_time[i] = "0" + std::to_string(time[i]);
        }
        else
        {
            str_time[i] = std::to_string(time[i]);
        }
    }

    return str_time[0] + ":" + str_time[1] + ":" + str_time[2];
}