#pragma once

#include <chrono>
#include "IOCPOpType.h"

typedef struct Event
{
	int provider;
	IOCPOpType event_type;
}Event;

typedef struct TimeEvent
{
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> time;
	Event event;
}TimeEvent;
