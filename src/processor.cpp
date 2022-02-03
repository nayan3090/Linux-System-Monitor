#include "processor.h"
#include "linux_parser.h"
#include <time.h>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{
    float totald, idled, percent;
    int start_clock = (int)clock();
    int current_clock = (int)clock();

    while (current_clock - start_clock < 500)
    {
        total.push_back((float)LinuxParser::Jiffies());
        idle.push_back((float)LinuxParser::IdleJiffies());
        current_clock = clock();
    }

    totald = total.back() - total.front();
    idled = idle.back() - idle.front();

    percent = (totald - idled) / totald;
    return percent;
}