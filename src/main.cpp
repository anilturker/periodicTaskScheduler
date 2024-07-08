#include <iostream>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include "PeriodicTaskScheduler.h"

void task1() {
    std::cout << "Task 1 executed" << std::endl;
}

void task2() {
    std::cout << "Task 2 executed" << std::endl;
}

int main() {
    PeriodicTaskScheduler scheduler;

    timeval interval1 = {5, 0}; // 5 seconds interval
    timeval interval2 = {10, 0}; // 10 seconds interval

    auto taskId1 = scheduler.addTask(task1, interval1);
    auto taskId2 = scheduler.addTask(task2, interval2);

    scheduler.start();

    std::this_thread::sleep_for(std::chrono::seconds(30));  // Run for 30 seconds

    timeval newInterval = {3, 0}; // 3 seconds interval
    scheduler.removeTask(taskId1);
    scheduler.changeTaskInterval(taskId2, newInterval);

    std::this_thread::sleep_for(std::chrono::seconds(30));  // Run for 30 more seconds

    scheduler.stop();

    return 0;
}
