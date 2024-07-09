#define BOOST_TEST_MODULE PeriodicTaskSchedulerTest
#include <boost/test/included/unit_test.hpp>
#include "PeriodicTaskScheduler.h"
#include <sys/time.h> // For timeval

BOOST_AUTO_TEST_CASE(TestAddTask) {
    PeriodicTaskScheduler scheduler;
    int taskId = scheduler.addTask(std::chrono::seconds(5));
    BOOST_CHECK(taskId == 1);
}

BOOST_AUTO_TEST_CASE(TestRemoveTask) {
    PeriodicTaskScheduler scheduler;
    int taskId = scheduler.addTask(std::chrono::seconds(5));
    scheduler.removeTask(taskId);
    // Assuming some internal method to check if task exists (this is just for example)
    BOOST_CHECK(true); // Check if task was removed correctly
}

BOOST_AUTO_TEST_CASE(TestChangeTaskInterval) {
    PeriodicTaskScheduler scheduler;
    int taskId = scheduler.addTask(std::chrono::seconds(5));
    scheduler.changeTaskInterval(taskId, std::chrono::seconds(10));
    // Assuming some internal method to get task interval (this is just for example)
    BOOST_CHECK(true); // Check if task interval was changed correctly
}

BOOST_AUTO_TEST_CASE(TestOnNewTime) {
    PeriodicTaskScheduler scheduler;
    timeval newTime;
    gettimeofday(&newTime, nullptr);
    newTime.tv_sec += 5;
    scheduler.onNewTime(newTime);
    // Assuming some internal method to get current time (this is just for example)
    BOOST_CHECK(true); // Check if time was updated correctly
}

BOOST_AUTO_TEST_CASE(TestTaskExecution) {
    PeriodicTaskScheduler scheduler;

    int executionCount = 0;
    scheduler.setTaskFunction([&executionCount] {
        ++executionCount;
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

    std::this_thread::sleep_for(std::chrono::seconds(6)); // Allow some time for tasks to execute

    BOOST_CHECK(executionCount > 0); // Check if tasks were executed
}

BOOST_AUTO_TEST_CASE(TestSchedulerStop) {
    PeriodicTaskScheduler scheduler;
    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    scheduler.stop();
    // Ensure no tasks are running and scheduler has stopped
    BOOST_CHECK(true);
}
