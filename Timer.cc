
#include "Timer.h"
#include "Logger.h"
#include "AgileGlobal.h"
#include <sys/time.h>

namespace agile
{

Timer::Timer()
{
	m_timerVal = AgileGlobal::Instance().timer_loop_val;
	if(m_timerVal != 1 && m_timerVal != 10 && m_timerVal != 100 && m_timerVal%1000 != 0)
	{
		LOG_WARN_S << "timer_loop_val error val is " << m_timerVal << " default value is 10";
		m_timerVal = 10;
	}
}

Timer::~Timer()
{
	for( auto& it : m_timersCache )
	{
		delete it;
	}
	m_timersCache.clear();
	
	for( auto& it : m_timers )
	{
		for(auto& it2 : it.second)
		{
			delete it2;
		}
	}
	m_timers.clear();
}

uint64_t Timer::GetCurMSec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) ( tv.tv_sec * 1000 + tv.tv_usec / 1000 ); 
}

uint64_t Timer::GetCurSec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) tv.tv_sec;
}

Timer::TimerObj* Timer::GetTimerObj(uint64_t msecVal, TimerCallback callbackFunc)
{
	if(m_timersCache.empty())
	{
		Timer::TimerObj* obj = new Timer::TimerObj();
		obj->val = msecVal;
		obj->func = callbackFunc;
		return obj;
	}
	else
	{
		Timer::TimerObj* obj = m_timersCache.front();
		obj->val = msecVal;
		obj->func = callbackFunc;
		m_timersCache.pop_front();
		return obj;
	}
}

void Timer::AddSecTimer(uint32_t secVal, TimerCallback callbackFunc)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64_t triggertMsecVal = 0;
	switch(m_timerVal)
	{
		case 1:
		    triggertMsecVal = (uint64_t) ( tv.tv_sec * 1000 + tv.tv_usec / 1000 + secVal * 1000 );
			break;
		case 10:
		    triggertMsecVal = (uint64_t) ( tv.tv_sec * 100 + tv.tv_usec / 10000 + secVal * 100 );
			break;
		case 100:
		    triggertMsecVal = (uint64_t) ( tv.tv_sec * 10 + tv.tv_usec / 100000 + secVal * 10 );
			break;
		default:
		 	triggertMsecVal = (uint64_t) ( tv.tv_sec + secVal);
			break;
	}
	AddCurMSecTimer(triggertMsecVal, callbackFunc);
}

void Timer::AddMSecTimer(uint32_t msecVal, TimerCallback callbackFunc)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64_t triggertMsecVal = 0;
	switch(m_timerVal)
	{
		case 1:
			triggertMsecVal = (uint64_t) ( tv.tv_sec * 1000 + tv.tv_usec / 1000 + msecVal);
			break;
		case 10:
		    triggertMsecVal = (uint64_t) ( tv.tv_sec * 100 + tv.tv_usec / 10000 + msecVal / 10 );
			break;
		case 100:
		    triggertMsecVal = (uint64_t) ( tv.tv_sec * 10 + tv.tv_usec / 100000 + msecVal / 100 );
			break;
		default:
		 	triggertMsecVal = (uint64_t) ( tv.tv_sec + msecVal / 1000);
			break;
	}
	AddCurMSecTimer(triggertMsecVal, callbackFunc);
}

void Timer::AddCurMSecTimer(uint64_t curMSecVal, TimerCallback callbackFunc)
{
	auto it = m_timers.find(curMSecVal);
	if(it != m_timers.end())
	{
		Timer::TimerObj* obj = GetTimerObj(curMSecVal, callbackFunc);
		it->second.push_back(obj);
	}
	else
	{
		std::list<Timer::TimerObj*>& newVal = m_timers[curMSecVal];
		Timer::TimerObj* obj = GetTimerObj(curMSecVal, callbackFunc);
		newVal.push_back(obj);
	}

	if(m_traceTime == 0)
	{
		m_traceTime = curMSecVal;
	}
	else
	{
		if(m_traceTime > curMSecVal)
		{
			m_traceTime = curMSecVal;
		}
	}

}

void Timer::Loop()
{
	size_t timerNum = m_timers.size();
	if(timerNum == 0){
		return;
	}

	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	uint64_t curMsec = 0;
	switch(m_timerVal)
	{
		case 1:
			{
		    	curMsec = (uint64_t) ( tv.tv_sec * 1000 + tv.tv_usec / 1000 );
				m_curMSec = curMsec;
			}
			break;
		case 10:
			{
		    	curMsec = (uint64_t) ( tv.tv_sec * 100 + tv.tv_usec / 10000 );
				m_curMSec = curMsec * 10;
			}
			break;
		case 100:
			{
		    	curMsec = (uint64_t) ( tv.tv_sec * 10 + tv.tv_usec / 100000 );
				m_curMSec = curMsec * 100;
			}
			break;
		default:
			{
		    	curMsec = (uint64_t) ( tv.tv_sec );
				m_curMSec = curMsec * 1000;
			}
			break;
	}

	uint64_t i = m_traceTime;
	while(i <= curMsec && timerNum > 0)
	{
		auto it = m_timers.find(i);
		if(it != m_timers.end())
		{
			--timerNum;
			for(auto& it2 : it->second)
			{
				it2->func(curMsec);

				if(m_cacheSize > m_timersCache.size())
				{
					m_timersCache.push_back(it2);
				}
				else
				{
					delete it2;
				}
			}
			it->second.clear();
			m_timers.erase(it);
		}
		++i;
	}
	m_traceTime = curMsec;
}

}
