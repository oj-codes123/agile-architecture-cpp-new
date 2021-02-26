
#include "Logger.h"
#include "Timer.h"
#include "EpollLoop.h"
#include "AgileGlobal.h"

namespace agile
{

EpollLoop::EpollLoop()
{
	m_epoll_timeout = AgileGlobal::Instance().epoll_timeout;
	if(m_epoll_timeout > 1000)
	{
		m_epoll_timeout = 3;
	}

	m_epoll_event_num = AgileGlobal::Instance().epoll_event_num;
	if(m_epoll_event_num < 100 || m_epoll_event_num > 300000)
	{
		m_epoll_event_num = 50000;
	}
}

EpollLoop::~EpollLoop()
{
	if(m_sharedEpoll)
	{
		delete m_sharedEpoll;
		m_sharedEpoll = nullptr;
	}
	
	for(auto it = m_epolls.begin(); it != m_epolls.end(); it++)
	{
		delete *it;
	}
	m_epolls.clear();
}

void EpollLoop::Run()
{
    if(m_running) 
	{ 
		return; 
	}
    m_running = true;

    while(m_running)
    {
		if(m_sharedEpoll)
		{
			m_sharedEpoll->Loop();
		}
		
		for(auto& it : m_epolls)
		{
			it->Loop();
		}
        Timer::Instance().Loop();
    }

    LOG_INFO_S << "finish loop epoll size:" << (uint32_t)m_epolls.size();
	
	if(m_exit)
	{
		exit(1);
	}
}

void EpollLoop::StopAndExit()
{
	m_exit    = true;
	m_running = false;
}

NetEpoll* EpollLoop::ShareEpoll()
{
	if(!m_sharedEpoll)
	{
		m_sharedEpoll = new NetEpoll(m_epoll_timeout, m_epoll_event_num);
	}
	return m_sharedEpoll;
}

NetEpoll* EpollLoop::CreateEpoll()
{
	NetEpoll* epollObj = new NetEpoll(m_epoll_timeout, m_epoll_event_num);
	m_epolls.push_back(epollObj);
	return epollObj;
}

void EpollLoop::DeleteEpoll(int id)
{	
	auto it = m_epolls.begin();
	while( it != m_epolls.end() )
	{
		if((*it)->GetId() == id )
		{
			NetEpoll* temp = *it;
			m_epolls.erase(it);
			delete temp;
			temp = nullptr;
			break;
		}
		else
		{
			it++;
		}
	}
}

}
