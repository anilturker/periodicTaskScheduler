#include "PeriodicTaskScheduler.h"
#include <iostream>

PeriodicTaskScheduler::PeriodicTaskScheduler() {}

PeriodicTaskScheduler::~PeriodicTaskScheduler() {
    stop();
}

int PeriodicTaskScheduler::addTask(Task task, std::chrono::seconds interval) {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    int taskId = nextTaskId++;
    auto now = std::chrono::system_clock::now();
    tasks[taskId] = {task, interval, now + interval};
    std::cout << "Task " << taskId << " added with interval " << interval.count() << " seconds." << std::endl; // Debugging statement
    return taskId;
}

void PeriodicTaskScheduler::removeTask(int taskId) {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.erase(taskId);
    std::cout << "Task " << taskId << " removed." << std::endl; // Debugging statement
}

void PeriodicTaskScheduler::changeTaskInterval(int taskId, std::chrono::seconds newInterval) {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    if (tasks.find(taskId) != tasks.end()) {
        tasks[taskId].interval = newInterval;
        tasks[taskId].nextExecutionTime = std::chrono::system_clock::now() + newInterval;
        std::cout << "Task " << taskId << " interval changed to " << newInterval.count() << " seconds." << std::endl; // Debugging statement
    }
}

void PeriodicTaskScheduler::onNewTime(int newTime) {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    std::cout << "onNewTime called with newTime: " << newTime << std::endl; // Debugging statement
    for (auto& [taskId, task] : tasks) {
        auto currentTime = std::chrono::system_clock::now();
        int currentSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count();
        std::cout << "Checking task " << taskId << " with next execution time: " << std::chrono::duration_cast<std::chrono::seconds>(task.nextExecutionTime.time_since_epoch()).count() << std::endl; // Debugging statement
        if (currentSeconds >= std::chrono::duration_cast<std::chrono::seconds>(task.nextExecutionTime.time_since_epoch()).count()) {
            std::cout << "Executing task " << taskId << std::endl; // Debugging statement
            task.task();
            task.nextExecutionTime = currentTime + task.interval;
        }
    }
}

void PeriodicTaskScheduler::processPackets() {
    int previousTime = -1;
    while (running) {
        auto now = std::chrono::system_clock::now();
        int currentTime = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        if (currentTime != previousTime) {
            onNewTime(currentTime);
            previousTime = currentTime;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate packet processing delay
    }
}

void PeriodicTaskScheduler::start() {
    running = true;
    processing_thread = std::thread(&PeriodicTaskScheduler::processPackets, this);
}

void PeriodicTaskScheduler::stop() {
    running = false;
    if (processing_thread.joinable()) {
        processing_thread.join();
    }
}
