#include <iostream>
#include <thread>
#include <chrono>
#include "PeriodicTaskScheduler.h"

void task1() {
    std::cout << "Task 1 executed" << std::endl;
}

void task2() {
    std::cout << "Task 2 executed" << std::endl;
}

int main() {
    PeriodicTaskScheduler scheduler;

    std::cout << "Hi there" << std::endl;

    auto taskId1 = scheduler.addTask(task1, std::chrono::seconds(5));
    auto taskId2 = scheduler.addTask(task2, std::chrono::seconds(10));

    scheduler.start();

    std::this_thread::sleep_for(std::chrono::seconds(30));  // Run for 30 seconds

    scheduler.removeTask(taskId1);
    scheduler.changeTaskInterval(taskId2, std::chrono::seconds(3));

    std::this_thread::sleep_for(std::chrono::seconds(30));  // Run for 30 more seconds

    scheduler.stop();

    return 0;
}
