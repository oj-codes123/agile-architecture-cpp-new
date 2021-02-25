
#pragma once

#include <functional>
#include <unordered_map>
#include <list>

namespace agile
{

typedef std::function<void(uint64_t)> TimerCallback;

class Timer
{
public:
	static Timer& Instance() { static Timer obj; return obj; }
	~Timer();

	inline uint64_t GetCurSec();
	inline uint64_t GetCurMSec();
	 
	uint64_t GetCacheCurMSec() const { return m_curMSec; }
	void SetCacheSize(uint32_t val) { m_cacheSize = val; }

	void AddSecTimer(uint32_t secVal, TimerCallback callbackFunc);
	
	void AddMSecTimer(uint32_t msecVal, TimerCallback callbackFunc);
	
	void Loop();

private:
	Timer();

	struct TimerObj
	{
		uint64_t val = 0;
		TimerCallback func = nullptr;
	};
	
	void AddCurMSecTimer(uint64_t curMSecVal, TimerCallback callbackFunc);
	TimerObj* GetTimerObj(uint64_t msecVal, TimerCallback callbackFunc);


private:
	uint32_t m_cacheSize = 1024;
	uint32_t m_timerVal  = 10;
	uint64_t m_traceTime = 0;
	uint64_t m_curMSec   = 0;
	std::list<TimerObj*> m_timersCache;
	std::unordered_map<uint64_t, std::list<TimerObj*> > m_timers;
};

}
