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
#include <vector>
#include <thread>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>
#include <mutex>
#include <iostream>

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
    struct HeapEntry
    {
        uint64_t time;
        Timer timer;
        int32_t id;

        bool operator<(const HeapEntry & entry) const
        {
            return time < entry.time;
        }

        bool operator>(const HeapEntry & entry) const
        {
            return time > entry.time;
        }
    };


public:
    TimerManager():
        _timer_heap(),
        _stop_running(false),
        _run_thread(),
        _timer_id(0),
        _heap_mutex()
    {}
        
    virtual ~TimerManager() { fina(); }

    void init()
    {
        try
        {
            _run_thread = std::thread(&TimerManager::run, this);
        }
        catch(const std::exception & e)
        {
            std::cout << "init exception " << e.what() << std::endl;
        }
        catch(...)
        {
            std::cout << "init unknow exception" << std::endl;
        }
    }

    void fina()
    {
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
            std::cout << "fina exception " << e.what() << std::endl;
        }
        catch(...)
        {
            std::cout << "fina unknow exception" << std::endl;
        }
    }

    int32_t add_timer(const Timer & timer)
    {
        if(timer.interval == 0)
        {
            return 0;
        }

        uint64_t expires_time = 0;
        if(!timer.immediately)
        {
            expires_time = get_current_subtlesecs() + timer.interval * 1000;
        }

        _timer_id++;
        if(_timer_id == 0)
        {
            _timer_id++;
        }

        HeapEntry entry = {expires_time, timer, _timer_id};

        if(timer.immediately)
        {
            exec_entry(entry);
        }

        {
            std::lock_guard<std::recursive_mutex> guard(_heap_mutex);

            push_heap(entry);
        }
    
        return _timer_id;
    }

    void remove_timer(int32_t id)
    {    
        {
            std::lock_guard<std::recursive_mutex> guard(_heap_mutex);
            auto it = _timer_heap.begin();
            while(it != _timer_heap.end())
            {
                if(it->id == id)
                {
                    _timer_heap.erase(it);

                    make_heap();

                    break;
                }
                
                ++it;
            }
        }
    }

protected:
    void run()
    {
        int idle_cnt = 0;

        while(!_stop_running)
        {
            uint64_t now_time = get_current_subtlesecs();

            do
            {
                HeapEntry top_entry;

                {
                    std::lock_guard<std::recursive_mutex> guard(_heap_mutex);
                    
                    if(_timer_heap.empty())
                    {
                        cout << "empty" << endl;

                        ++idle_cnt;
                        break;
                    }

                    top_entry = pop_heap();
                }

                cout << top_entry.id << " " << top_entry.time << " " << top_entry.timer.loop_times << endl;

                if(top_entry.time <= now_time)
                {
                    if(top_entry.timer.loop_times != 0)
                    {
                        exec_entry(top_entry);
                    }

                    if(top_entry.timer.loop_times != 0)
                    {
                        std::lock_guard<std::recursive_mutex> guard(_heap_mutex);

                        push_heap(top_entry);
                    }

                    break;
                }
                else
                {
                    push_heap(top_entry);

                    ++idle_cnt;
                    break;
                }
            }while(0);

            if(idle_cnt >= 100)
            {
                usleep(1);
                idle_cnt = 0;
            }
        }
    }

    void exec_entry(HeapEntry & entry)
    {
        if(entry.timer.callback)
        {
            entry.timer.callback();
        }

        if(entry.timer.loop_times != Timer::LOOP_FOREVER)
        {
            entry.timer.loop_times--;
        }

        entry.time = get_current_subtlesecs() + entry.timer.interval * 1000;
    }

    static uint64_t get_current_millisecs()
    {
        timeval tv;
        ::gettimeofday(&tv, 0);
        uint64_t ret = tv.tv_sec;

        return ret * 1000 + tv.tv_usec / 1000;    
    }
    
    static uint64_t get_current_subtlesecs()
    {
        timeval tv;
        ::gettimeofday(&tv, 0);
        uint64_t ret = tv.tv_sec;

        return ret * 1000 * 1000 + tv.tv_usec;
    }

protected:
    inline void make_heap()
    {
        std::make_heap(_timer_heap.begin(), _timer_heap.end(), std::greater<HeapEntry>());
    }

    inline HeapEntry pop_heap()
    {
        std::pop_heap(_timer_heap.begin(), _timer_heap.end(), std::greater<HeapEntry>());

        HeapEntry top_entry = _timer_heap.back();
        _timer_heap.pop_back();

        return top_entry;
    }

    inline void push_heap(const HeapEntry & entry)
    {
        _timer_heap.push_back(entry);

        std::push_heap(_timer_heap.begin(), _timer_heap.end(), std::greater<HeapEntry>());
    }

protected:

    std::vector<HeapEntry> _timer_heap;
    bool _stop_running;
    std::thread _run_thread;
    int32_t _timer_id;
    std::recursive_mutex _heap_mutex;
};

#endif  //__SL_TIMER_H__

