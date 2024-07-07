#define BOOST_TEST_MODULE PeriodicTaskSchedulerTest
#include <boost/test/included/unit_test.hpp>
#include "PeriodicTaskScheduler.h"

BOOST_AUTO_TEST_CASE(AddTaskTest) {
    PeriodicTaskScheduler scheduler;
    auto taskId = scheduler.addTask([](){}, std::chrono::seconds(1));
    BOOST_CHECK(taskId == 0);
}

BOOST_AUTO_TEST_CASE(RemoveTaskTest) {
    PeriodicTaskScheduler scheduler;
    auto taskId = scheduler.addTask([](){}, std::chrono::seconds(1));
    scheduler.removeTask(taskId);
    scheduler.onNewTime(std::chrono::seconds(1)); // Should not crash
}

BOOST_AUTO_TEST_CASE(ChangeIntervalTest) {
    PeriodicTaskScheduler scheduler;
    bool executed = false;
    auto taskId = scheduler.addTask([&executed](){ executed = true; }, std::chrono::seconds(5));
    scheduler.changeTaskInterval(taskId, std::chrono::seconds(1));
    scheduler.onNewTime(std::chrono::seconds(1));
    BOOST_CHECK(executed);
}

BOOST_AUTO_TEST_CASE(ThreadSafetyTest) {
    PeriodicTaskScheduler scheduler;
    std::atomic<bool> executed(false);
    auto taskId = scheduler.addTask([&executed](){ executed = true; }, std::chrono::seconds(1));
    std::thread t1([&scheduler, taskId](){
        for (int i = 0; i < 10; ++i) {
            scheduler.changeTaskInterval(taskId, std::chrono::seconds(2));
        }
    });
    std::thread t2([&scheduler](){
        for (int i = 0; i < 10; i++) {
            scheduler.onNewTime(std::chrono::seconds(i));
        }
    });
    t1.join();
    t2.join();
    BOOST_CHECK(executed);
}
