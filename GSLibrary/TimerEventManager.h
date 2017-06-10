#pragma once
#include <deque>
#include <chrono>

#include "Event.h"

class TimerEventManager
{
private:
	std::deque<TimeEvent> m_timerEventQueue;

public:
	TimerEventManager();
	~TimerEventManager();

	auto GetQueueBegin() { return m_timerEventQueue.begin(); }
	auto GetQueueEnd() { return m_timerEventQueue.end(); }

	void Enqueue(const double duration, const Event event);
	const TimeEvent* Dequeue();
	bool isEmpty() { return m_timerEventQueue.size() <= 0 ? true : false; }
};
