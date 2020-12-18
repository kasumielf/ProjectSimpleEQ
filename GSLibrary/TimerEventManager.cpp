#include "TimerEventManager.h"

TimerEventManager::TimerEventManager()
{
}

TimerEventManager::~TimerEventManager()
{
}

void TimerEventManager::Push(const double duration, const Event event)
{
    TimeEvent ev;
    auto event_time = std::chrono::high_resolution_clock::now() + std::chrono::duration<double, std::milli>(duration);

    ev.time = event_time;
    ev.event = event;
    timer_lock.lock();
    m_timerEventQueue.push(ev);
    timer_lock.unlock();
}

const TimeEvent& TimerEventManager::Top()
{
    return m_timerEventQueue.top();
}

void TimerEventManager::Pop()
{
    timer_lock.lock();
    m_timerEventQueue.pop();
    timer_lock.unlock();
}
