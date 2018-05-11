/*************************************************************************
    > File Name: test/main.cpp
    > Author: mxl
    > Mail: xiaolongicx@gmail.com
    > Created Time: 2018-04-27 11:06
 ************************************************************************/

#include <iostream>
#include <assert.h>

#include "../Timer.h"

using namespace std;

static int x = 0;

void test_run_times_cb()
{
    x++;
    cout << x << endl;
}

void test_run_times(TimerManager & tmanager)
{
    
    Timer timer;
    timer.callback = std::bind(test_run_times_cb);
    timer.immediately = true;
    timer.interval = 10;
    timer.loop_times = 100;

    tmanager.add_timer(timer);
}

static int s1 = 0;
static int s2 = 0;
static int s3 = 0;
static int s4 = 0;
static int s5 = 0;
static int s6 = 0;
static int s7 = 0;
static int s8 = 0;
static int s9 = 0;
static int s10 = 0;

void test_sequence_cb_1()
{
    s1++;
    cout << "1: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;

    assert(s1 == s2 + 1);
    assert(s10 == s2);
    assert(s10 == s3);
    assert(s10 == s4);
    assert(s10 == s5);
    assert(s10 == s6);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_2()
{
    s2++;
    cout << "2: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;

    assert(s2 == s3 + 1);
    assert(s10 == s3);
    assert(s10 == s4);
    assert(s10 == s5);
    assert(s10 == s6);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_3()
{
    s3++;
    cout << "3: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;

    assert(s3 == s4 + 1);
    assert(s10 == s4);
    assert(s10 == s5);
    assert(s10 == s6);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_4()
{
    s4++;
    cout << "4: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    
    assert(s4 == s5 + 1);
    assert(s10 == s5);
    assert(s10 == s6);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_5()
{
    s5++;
    cout << "5: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    
    assert(s5 == s6 + 1);
    assert(s10 == s6);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_6()
{
    s6++;
    cout << "6: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    
    assert(s6 == s7 + 1);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_7()
{
    s7++;
    cout << "7: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    
    assert(s7 == s8 + 1);
    assert(s10 == s8);
    assert(s10 == s9);
}

void test_sequence_cb_8()
{
    s8++;
    cout << "8: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    
    assert(s8 == s9 + 1);
    assert(s10 == s9);
}

void test_sequence_cb_9()
{
    s9++;
    cout << "9: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    
    assert(s9 == s10 + 1);
}

void test_sequence_cb_10()
{
    s10++;
    cout << "10: " << s1 << " " << s2 << " " << s3 << " " << s4 << " " << s5 << " " << s6 << " " << s7 << " " << s8 << " " << s9 << " " << s10  << endl;
    assert(s10 == s1);
    assert(s10 == s2);
    assert(s10 == s3);
    assert(s10 == s4);
    assert(s10 == s5);
    assert(s10 == s6);
    assert(s10 == s7);
    assert(s10 == s8);
    assert(s10 == s9);
   
}

void test_sequence(TimerManager & tmanager)
{
    Timer timer;
    timer.callback = std::bind(test_sequence_cb_1);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_2);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;
    
    usleep(10);

    timer.callback = std::bind(test_sequence_cb_3);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_4);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_5);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_6);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_7);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_8);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_9);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(10);

    timer.callback = std::bind(test_sequence_cb_10);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;
}

static int s_a = 0;
void test_remove_timer_cb()
{
    s_a++;
    cout << "s_a " << s_a << endl;
}

void test_remove_timer(TimerManager & tmanager)
{
    Timer timer;
    timer.callback = std::bind(test_remove_timer_cb);
    timer.interval = 100;
    timer.loop_times = 100;

    int timer_id = tmanager.add_timer(timer);

    usleep(550000);

    tmanager.remove_timer(timer_id);
    assert(s_a == 5);
    
    usleep(550000);
    assert(s_a == 5);
}

int main()
{
    TimerManager tmanager;
    tmanager.init();
    
    test_run_times(tmanager);

    sleep(2);
    cout << x << endl;
    assert(x == 100);

    test_sequence(tmanager);
    sleep(2);
    
    test_remove_timer(tmanager);

    tmanager.fina();

    return 0;
}

