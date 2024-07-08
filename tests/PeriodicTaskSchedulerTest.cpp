#define BOOST_TEST_MODULE PeriodicTaskSchedulerTest
#include <boost/test/included/unit_test.hpp>
#include "../src/PeriodicTaskScheduler.h"

BOOST_AUTO_TEST_CASE(add_task_test) {
    PeriodicTaskScheduler scheduler;
    timeval interval = {2, 0}; // 2 seconds interval
    bool taskExecuted = false;

    auto task = [&taskExecuted]() { taskExecuted = true; };

    scheduler.addTask(task, interval);

    timeval newTime = {3, 0}; // Advance time by 3 seconds
    scheduler.onNewTime(newTime);

    BOOST_CHECK(taskExecuted);
}

BOOST_AUTO_TEST_CASE(remove_task_test) {
    PeriodicTaskScheduler scheduler;
    timeval interval = {2, 0}; // 2 seconds interval
    bool taskExecuted = false;

    auto task = [&taskExecuted]() { taskExecuted = true; };

    int taskId = scheduler.addTask(task, interval);
    scheduler.removeTask(taskId);

    timeval newTime = {3, 0}; // Advance time by 3 seconds
    scheduler.onNewTime(newTime);

    BOOST_CHECK(!taskExecuted);
}

BOOST_AUTO_TEST_CASE(change_interval_test) {
    PeriodicTaskScheduler scheduler;
    timeval interval = {2, 0}; // 2 seconds interval
    timeval newInterval = {4, 0}; // 4 seconds interval
    bool taskExecuted = false;

    auto task = [&taskExecuted]() { taskExecuted = true; };

    int taskId = scheduler.addTask(task, interval);
    scheduler.changeTaskInterval(taskId, newInterval);

    timeval newTime = {3, 0}; // Advance time by 3 seconds (task should not execute)
    scheduler.onNewTime(newTime);
    BOOST_CHECK(!taskExecuted);

    newTime = {5, 0}; // Advance time by 5 seconds (task should execute)
    scheduler.onNewTime(newTime);
    BOOST_CHECK(taskExecuted);
}

BOOST_AUTO_TEST_CASE(thread_safety_test) {
    PeriodicTaskScheduler scheduler;
    timeval interval = {1, 0}; // 1 second interval
    std::atomic<int> counter(0);

    auto task = [&counter]() { ++counter; };

    int taskId1 = scheduler.addTask(task, interval);
    int taskId2 = scheduler.addTask(task, interval);

    scheduler.start();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    scheduler.stop();

    BOOST_CHECK(counter >= 8); // Since there are two tasks, they should execute around 10 times in total (5 seconds)
}
