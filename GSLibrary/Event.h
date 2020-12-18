#pragma once

#include <chrono>
#include "IOCPOpType.h"

typedef struct Event
{
    int provider;
    IOCPOpType event_type;
}Event;

class TimeEvent
{
public:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> time;
    Event event;
    
    friend bool operator<(const TimeEvent& lhs, const TimeEvent& rhs)
    {
        return lhs.time < rhs.time;
    }

    friend bool operator>(const TimeEvent& lhs, const TimeEvent& rhs)
    {
        return lhs.time > rhs.time;
    }

};
