#define BOOST_TEST_MODULE PeriodicTaskSchedulerTest
#include <boost/test/included/unit_test.hpp>
#include "PeriodicTaskScheduler.h"
#include <sys/time.h> // For timeval
#include <atomic>
#include <iostream>

BOOST_AUTO_TEST_CASE(TestAddTask) {
    PeriodicTaskScheduler scheduler;
    int taskId = scheduler.addTask(std::chrono::seconds(5));
    BOOST_CHECK(taskId == 1);
}

BOOST_AUTO_TEST_CASE(TestRemoveTask) {
    PeriodicTaskScheduler scheduler;
    int taskId = scheduler.addTask(std::chrono::seconds(5));
    scheduler.removeTask(taskId);
    BOOST_CHECK(!scheduler.isTaskPresent(taskId));
}

BOOST_AUTO_TEST_CASE(TestChangeTaskInterval) {
    PeriodicTaskScheduler scheduler;
    int taskId = scheduler.addTask(std::chrono::seconds(5));
    scheduler.changeTaskInterval(taskId, std::chrono::seconds(10));
    BOOST_CHECK(scheduler.getTaskInterval(taskId) == std::chrono::seconds(10));
}

BOOST_AUTO_TEST_CASE(TestOnNewTime) {
    PeriodicTaskScheduler scheduler;

    std::atomic<int> executionCount(0);
    scheduler.setTaskFunction([&executionCount] {
        ++executionCount;
        std::cout << "Task executed. Count: " << executionCount << std::endl;
    });

    scheduler.start(); // Ensure the scheduler is started before adding tasks

    int taskId1 = scheduler.addTask(std::chrono::seconds(5));
    int taskId2 = scheduler.addTask(std::chrono::seconds(10));

    timeval startTime;
    gettimeofday(&startTime, nullptr);
    timeval newTime = startTime;
    newTime.tv_sec += 5;
    scheduler.onNewTime(newTime);

    std::this_thread::sleep_for(std::chrono::seconds(6)); // Allow time for tasks to execute

    BOOST_CHECK(executionCount > 0); // Check if tasks were executed

    scheduler.stop(); // Ensure the scheduler is stopped after the test
}

BOOST_AUTO_TEST_CASE(TestTaskExecution) {
    PeriodicTaskScheduler scheduler;

    // Ensure the scheduler is started before adding tasks
    scheduler.start();

    std::atomic<int> executionCount(0);
    scheduler.setTaskFunction([&executionCount] {
        ++executionCount;
        std::cout << "Task executed. Count: " << executionCount << std::endl;
    });

    int taskId1 = scheduler.addTask(std::chrono::seconds(1));
    int taskId2 = scheduler.addTask(std::chrono::seconds(2));

    timeval startTime;
    gettimeofday(&startTime, nullptr);
    for (int i = 0; i < 5; ++i) {
        timeval packetTime = startTime;
        packetTime.tv_sec += i;
        scheduler.onNewTime(packetTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some delay
    }

    std::this_thread::sleep_for(std::chrono::seconds(10)); // Allow more time for tasks to execute

    BOOST_CHECK(executionCount > 0); // Check if tasks were executed

    scheduler.stop(); // Ensure the scheduler is stopped after the test
}

BOOST_AUTO_TEST_CASE(TestSchedulerStartStop) {
    PeriodicTaskScheduler scheduler;
    BOOST_CHECK(!scheduler.isRunning()); // Scheduler should not be running initially
    scheduler.start();
    BOOST_CHECK(scheduler.isRunning()); // Scheduler should be running after start
    std::this_thread::sleep_for(std::chrono::seconds(1));
    scheduler.stop();
    BOOST_CHECK(!scheduler.isRunning()); // Scheduler should not be running after stop
}
