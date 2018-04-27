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

static int y = 0;
static int z = 0;

void test_sequence_cb_y()
{
    y++;
    cout << "y: " << y << " " << z << endl;

    assert(y == z + 1);
}

void test_sequence_cb_z()
{
    z++;
    cout << "z: " << y << " " << z << endl;
    assert(y == z);
}

void test_sequence(TimerManager & tmanager)
{
    Timer timer;
    timer.callback = std::bind(test_sequence_cb_y);
    timer.interval = 100;
    timer.loop_times = 100;

    cout << tmanager.add_timer(timer) << endl;

    usleep(50);

    timer.callback = std::bind(test_sequence_cb_z);
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

    usleep(1000000);
    sleep(2);
    cout << x << endl;
    assert(x == 100);

    test_sequence(tmanager);
    sleep(2);
    
    test_remove_timer(tmanager);

    tmanager.fina();

    return 0;
}

