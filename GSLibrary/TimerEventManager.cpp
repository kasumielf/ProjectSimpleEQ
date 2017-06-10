#include "TimerEventManager.h"

TimerEventManager::TimerEventManager()
{
}

TimerEventManager::~TimerEventManager()
{
}

void TimerEventManager::Enqueue(const double duration, const Event event)
{
	TimeEvent ev;
	auto event_time = std::chrono::high_resolution_clock::now() + std::chrono::duration<double, std::milli>(duration);

	ev.time = event_time;
	ev.event = event;

	if (isEmpty())
	{
		m_timerEventQueue.push_front(ev);
	}
	else
	{
		auto iter_b = m_timerEventQueue.begin();
		auto iter_e = m_timerEventQueue.end();

		for (; iter_b != iter_e; ++iter_b)
		{
			if ((*iter_b).time >= event_time)
			{
				m_timerEventQueue.insert(iter_b, ev);
				break;
			}
			else
			{
				m_timerEventQueue.push_back(ev);
				break;
			}
		}
	}
}

const TimeEvent* TimerEventManager::Dequeue()
{
	TimeEvent *rtn;

	if (isEmpty() == false)
	{
		rtn = &m_timerEventQueue.front();
		m_timerEventQueue.pop_front();
	}

	return rtn;
}
