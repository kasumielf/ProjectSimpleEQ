#pragma once

#include "Event.h"

#include <functional>
#include <vector>
#include <chrono>
#include <queue>
#include <mutex>

class TimerEventManager
{
private:
	//std::deque<TimeEvent> m_timerEventQueue;
	std::priority_queue<TimeEvent, std::vector<TimeEvent>, std::greater<TimeEvent> >m_timerEventQueue;
	std::mutex timer_lock;
public:
	TimerEventManager();
	~TimerEventManager();

	void Push(const double duration, const Event event);
	const TimeEvent& Top();
	void Pop();
	bool isEmpty() { return m_timerEventQueue.size() <= 0 ? true : false; }
};
