#include "PeriodicTaskScheduler.h"
#include <iostream>
#include <sys/time.h>

PeriodicTaskScheduler::PeriodicTaskScheduler() {
    this->currentTime.tv_sec = 0;
    this->currentTime.tv_usec = 0;
}

PeriodicTaskScheduler::~PeriodicTaskScheduler() {
    this->stop();
}

int PeriodicTaskScheduler::addTask(Task task, const timeval& interval) {
    std::lock_guard<std::mutex> lock(this->tasks_mutex);
    int taskId = this->nextTaskId++;
    timeval nextExecutionTime = addTimevals(this->currentTime, interval);
    this->tasks[taskId] = {task, interval, nextExecutionTime};
    return taskId;
}

void PeriodicTaskScheduler::removeTask(int taskId) {
    std::lock_guard<std::mutex> lock(this->tasks_mutex);
    this->tasks.erase(taskId);
}

void PeriodicTaskScheduler::changeTaskInterval(int taskId, const timeval& newInterval) {
    std::lock_guard<std::mutex> lock(this->tasks_mutex);
    if (this->tasks.find(taskId) != this->tasks.end()) {
        this->tasks[taskId].interval = newInterval;
        this->tasks[taskId].nextExecutionTime = addTimevals(this->currentTime, newInterval);
    }
}

void PeriodicTaskScheduler::onNewTime(const timeval& newTime) {
    std::lock_guard<std::mutex> lock(this->tasks_mutex);
    this->currentTime = newTime;
    for (auto& [taskId, task] : this->tasks) {
        if (timevalGreaterOrEqual(this->currentTime, task.nextExecutionTime)) {
            task.task();
            task.nextExecutionTime = addTimevals(this->currentTime, task.interval);
        }
    }
}

void PeriodicTaskScheduler::processPackets() {
    timeval previousTime = {0, 0};
    timeval pkt = {0, 0}; // Start with an initial packet time
    while (this->running) {
        // Simulate retrieving the latest packet with an external timestamp
        // Increment the packet time by 1 second for demonstration purposes
        pkt.tv_sec += 1;

        if (previousTime.tv_sec != pkt.tv_sec) {
            this->onNewTime(pkt);
            previousTime = pkt;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Simulate real-time packet processing delay
    }
}

void PeriodicTaskScheduler::start() {
    this->running = true;
    this->processing_thread = std::thread(&PeriodicTaskScheduler::processPackets, this);
}

void PeriodicTaskScheduler::stop() {
    this->running = false;
    if (this->processing_thread.joinable()) {
        this->processing_thread.join();
    }
}

timeval PeriodicTaskScheduler::addTimevals(const timeval& t1, const timeval& t2) {
    timeval result;
    result.tv_sec = t1.tv_sec + t2.tv_sec;
    result.tv_usec = t1.tv_usec + t2.tv_usec;
    if (result.tv_usec >= 1000000) {
        result.tv_sec += 1;
        result.tv_usec -= 1000000;
    }
    return result;
}

bool PeriodicTaskScheduler::timevalGreaterOrEqual(const timeval& t1, const timeval& t2) {
    return (t1.tv_sec > t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_usec >= t2.tv_usec);
}
