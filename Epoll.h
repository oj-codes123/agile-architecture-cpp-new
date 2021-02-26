
#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <sys/epoll.h>

namespace agile
{
const static int GEpollNoneEvent  = 0;
const static int GEpollReadEvent  = EPOLLIN | EPOLLPRI;
const static int GEpollWriteEvent = EPOLLOUT;
const static int GEpollModule     = EPOLLET;

class NetEpoll
{
public:
    NetEpoll(int timeout, int eventNum);
    ~NetEpoll(){ }

    void Loop();
    int GetId() const { return m_id; }
    int GetEpollId() const { return m_epollId; }

private:
    int m_id = 0;
    int m_timeout = 0;
    int m_epollId = 0;
    int m_eventNum = 0;
    std::vector<epoll_event> m_events;
};

}
