
#include "Logger.h"
#include "Timer.h"
#include "EpollLoop.h"

namespace agile
{

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
		
		if(!m_epolls.empty())
		{
			for(auto& it : m_epolls)
			{
				it->Loop();
			}
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
	m_exit = true;
	m_running = false;
}

NetEpoll* EpollLoop::ShareEpoll()
{
	if(!m_sharedEpoll)
	{
		m_sharedEpoll = new NetEpoll(3, 100000);
	}
	return m_sharedEpoll;
}

void EpollLoop::AddEpoll(NetEpoll* epollObj)
{
	if(!epollObj)
	{ 
		return;
	}
	
	for(auto& it : m_epolls)
	{
		if(it->GetEpollId() == epollObj->GetEpollId() )
		{
			LOG_ERROR_S << "equal epollId:" << epollObj->GetEpollId();
			return;
		}
	}
	m_epolls.push_back(epollObj);
}

void EpollLoop::DeleteEpoll(NetEpoll* epollObj)
{
	if(!epollObj)
	{ 
		return;
	}
	
	auto it = m_epolls.begin();
	while( it != m_epolls.end() )
	{
		if((*it)->GetEpollId() == epollObj->GetEpollId() )
		{
			it = m_epolls.erase(it);
			delete epollObj;
			epollObj = nullptr;
			break;
		}
		else
		{
			it++;
		}
	}	
}

}
