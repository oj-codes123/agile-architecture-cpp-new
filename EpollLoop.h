
#pragma once

#include <list>
#include "Epoll.h"

namespace agile
{
class EpollLoop
{
public:
    static EpollLoop& Instance(){ static EpollLoop obj; return obj; }
	~EpollLoop();
	
	void Run();
	
	NetEpoll* ShareEpoll();
	
	NetEpoll* CreateEpoll();

	void DeleteEpoll(int id);

	void StopAndExit();

private:
	EpollLoop();
	
private:
	bool m_exit = false;
	bool m_running = false;
	
	uint32_t m_epoll_timeout = 3;
	uint32_t m_epoll_event_num = 100000;

    NetEpoll* m_sharedEpoll = nullptr;
    std::vector<NetEpoll*> m_epolls;
};

}
