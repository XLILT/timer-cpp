/*************************************************************************
    > File Name: Timer.h
    > Author: mxl
    > Mail: xiaolongicx@gmail.com
    > Created Time: Sun 01 Apr 2018 05:35:44 AM UTC
 ************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include <inttypes.h>
#include <functional>
#include <thread>
#include <unistd.h>
#include <sys/time.h>
#include <mutex>
#include <unordered_map>
#include <atomic>

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <memory.h>

struct Timer
{
    std::function<void()> callback;         // function object. return value of std::bind
    uint32_t interval;     // call interval(millisecond)
    bool immediately;      // call immediatelly
    int32_t loop_times;      // loop times, -1 means forever

    static const int32_t LOOP_FOREVER = -1;

    Timer(): interval(0), immediately(false), loop_times(LOOP_FOREVER) {}
};

class TimerManager
{
protected:
    struct Entry
    {
        uint64_t time;
        Timer timer;
        int32_t id;
        int32_t fd;
    };

public:
    TimerManager():
        _stop_running(false),
        _run_thread(),
        _timer_id(0),
        _epoll_fd(0),
        _entry_map(),
        _entry_map_mutex()
    {}
        
    virtual ~TimerManager() { fina(); }

    int init()
    {
        int ret = 0;

        try
        {
            _epoll_fd = epoll_create1(EPOLL_CLOEXEC);

            _run_thread = std::thread(&TimerManager::run, this);
        }
        catch(const std::exception & e)
        {
            ret = -1;
        }
        catch(...)
        {
            ret = -2;
        }

        return ret;
    }

    int fina()
    {
        int ret = 0;

        _stop_running = true;

        try
        {
            if(_run_thread.joinable())
            {
                _run_thread.join();
            }
        }
        catch(const std::exception & e)
        {
            ret = -1;
        }
        catch(...)
        {
            ret = -2;
        }

        close(_epoll_fd);


        return ret;
    }

    int32_t add_timer(const Timer & timer)
    {
        if(timer.interval == 0
            || (timer.loop_times <= 0 && timer.loop_times != Timer::LOOP_FOREVER))
        {
            return 0;
        }

        uint64_t expires_time = 0;
        if(!timer.immediately)
        {
            expires_time = get_current_millisecs() + timer.interval;
        }
       
        int32_t tid = 0;

        do
        {
            tid = ++_timer_id;
        }while(_entry_map.count(tid) > 0);

        int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
        if (-1 == tfd)
        {
            return 0;
        }

        struct itimerspec span;

        memset(&span, 0, sizeof(struct itimerspec));

        span.it_value.tv_sec = timer.interval / 1000;
        span.it_value.tv_nsec = (timer.interval % 1000) * 1000000; // 1ms = 1000000ns
        
        span.it_interval.tv_sec  = timer.interval / 1000;
        span.it_interval.tv_nsec = (timer.interval % 1000) * 1000000;
        

        if (0 != timerfd_settime(tfd, 0, &span, NULL))
        {
            return 0;
        }

        {
            std::unique_lock<std::recursive_mutex> guard(_entry_map_mutex);
            Entry entry = {expires_time, timer, tid, tfd};
            
            _entry_map[tid] = entry;

            struct epoll_event ev;

            memset(&ev, 0, sizeof(struct epoll_event));

            ev.events = EPOLLIN | EPOLLET;
            ev.data.u64 = (uint64_t)tid;

            if (0 != epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, tfd, &ev))
            {
                remove_timer(tid);

                return 0;
            }

            if(timer.immediately)
            {
                exec_entry(_entry_map[tid]);
            }
        }

        return tid;
    }

    void remove_timer(int32_t id)
    {    
        std::unique_lock<std::recursive_mutex> guard(_entry_map_mutex);

        if(0 == _entry_map.count(id))
        {
            return;
        }

        auto & entry = _entry_map[id];

        close(entry.fd);

        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, entry.fd, NULL);

        _entry_map.erase(id);        
    }

protected:
    void run()
    {
        while(!_stop_running)
        {
            struct epoll_event evs[1024];
            memset(evs, 0, 1024 * sizeof(struct epoll_event));

            int esize = epoll_wait(_epoll_fd, evs, 1024, 100);

            for(int i = 0; i < esize; ++i)
            {
                if(EPOLLIN | evs[i].events)
                {
                    std::unique_lock<std::recursive_mutex> guard(_entry_map_mutex);

                    int32_t tid = (int32_t)evs[i].data.u64;
                    if(0 == _entry_map.count(tid))
                    {
                        continue;
                    }

                    Entry & entry = _entry_map[tid];
                    
                    char trigger_buffer[64];
                    read(entry.fd, trigger_buffer, 64);

                    exec_entry(entry);
                }            
            }
        }
    }

    void exec_entry(Entry & entry)
    {
        if(entry.timer.loop_times == 0)
        {
            remove_timer(entry.id);

            return;
        }

        if(entry.timer.callback)
        {
            entry.timer.callback();
        }

        if(entry.timer.loop_times != Timer::LOOP_FOREVER)
        {
            --entry.timer.loop_times;
        }

        if(entry.timer.loop_times == 0)
        {
            remove_timer(entry.id);
        }
        else
        {
            entry.time = get_current_millisecs() + entry.timer.interval;
        }
    }

    static uint64_t get_current_millisecs()
    {
        timeval tv;
        ::gettimeofday(&tv, 0);
        uint64_t ret = tv.tv_sec;

        return ret * 1000 + tv.tv_usec / 1000;    
    }

protected:

    volatile bool _stop_running;
    std::thread _run_thread;

    std::atomic<int32_t> _timer_id;

    int32_t _epoll_fd;
    std::unordered_map<int32_t, Entry> _entry_map;
    std::recursive_mutex _entry_map_mutex;
};

#endif  //__SL_TIMER_H__

