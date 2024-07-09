#include "PeriodicTaskScheduler.h"
#include <algorithm>
#include <sys/time.h>

// Constructor to initialize the scheduler with a specified number of threads
PeriodicTaskScheduler::PeriodicTaskScheduler(int numThreads) : nextTaskId_(1), running_(true) {
    // Initialize current time
    gettimeofday(&currentTime_, nullptr);

    // Create threads for executing tasks
    threads_ = std::vector<std::thread>(numThreads);
    for (auto &t : threads_) {
        t = std::thread([this] { this->threadRun(); });
    }
}

// Destructor to stop the scheduler and clean up resources
PeriodicTaskScheduler::~PeriodicTaskScheduler() {
    stop();
}

// Set the function that will be periodically executed
void PeriodicTaskScheduler::setTaskFunction(Task taskFunction) {
    taskFunction_ = taskFunction;
}

// Add a task to the scheduler with a specified interval
int PeriodicTaskScheduler::addTask(std::chrono::seconds interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    int taskId = nextTaskId_++;
    timeval nextExecutionTime = currentTime_;
    nextExecutionTime.tv_sec += interval.count();
    tasks_[taskId] = {interval, nextExecutionTime, taskId};
    task_updated_.notify_all();
    return taskId;
}

// Remove a task from the scheduler using its task ID
void PeriodicTaskScheduler::removeTask(int taskId) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.erase(taskId);
    task_updated_.notify_all();
}

// Change the interval of an existing task
void PeriodicTaskScheduler::changeTaskInterval(int taskId, std::chrono::seconds newInterval) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        it->second.interval = newInterval;
        updateNextExecutionTime(it->second, currentTime_);
        task_updated_.notify_all();
    }
}

// Update the scheduler with an externally provided time
void PeriodicTaskScheduler::onNewTime(const timeval &externalTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentTime_ = externalTime;
    task_updated_.notify_all();
}

// Start the scheduler in online mode
void PeriodicTaskScheduler::start() {
    std::thread([this] { this->schedulerLoop(); }).detach();
}

// Stop the scheduler
void PeriodicTaskScheduler::stop() {
    running_ = false;
    task_updated_.notify_all();
    task_available_to_execute_.notify_all();
    for (auto &t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

// Update the next execution time for a task
void PeriodicTaskScheduler::updateNextExecutionTime(TaskInfo &taskInfo, const timeval &currentTime) {
    taskInfo.nextExecutionTime = currentTime;
    taskInfo.nextExecutionTime.tv_sec += taskInfo.interval.count();
}

// Check if the current time has elapsed beyond the next execution time of a task
bool PeriodicTaskScheduler::timeElapsed(const timeval &currentTime, const timeval &nextExecutionTime) {
    if (currentTime.tv_sec > nextExecutionTime.tv_sec) {
        return true;
    } else if (currentTime.tv_sec == nextExecutionTime.tv_sec) {
        return currentTime.tv_usec >= nextExecutionTime.tv_usec;
    }
    return false;
}

// Main loop for the scheduler to update tasks and manage execution
void PeriodicTaskScheduler::schedulerLoop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        task_updated_.wait(lock, [this] {
            bool tasksReady = false;
            for (auto &task : tasks_) {
                if (timeElapsed(currentTime_, task.second.nextExecutionTime)) {
                    tasksExecQueue_.push_back(task.second);
                    updateNextExecutionTime(task.second, currentTime_);
                    tasksReady = true;
                }
            }
            return tasksReady || !running_;
        });

        while (!tasksExecQueue_.empty()) {
            auto task = tasksExecQueue_.front();
            tasksExecQueue_.pop_front();
            lock.unlock();
            if (taskFunction_) {
                taskFunction_();
            }
            lock.lock();
        }
    }
}

// Function that runs in each thread to execute tasks
void PeriodicTaskScheduler::threadRun() {
    while (running_) {
        TaskInfo task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            task_available_to_execute_.wait(lock, [this] { return !tasksExecQueue_.empty() || !running_; });
            if (!running_) return;
            task = tasksExecQueue_.front();
            tasksExecQueue_.pop_front();
        }
        if (taskFunction_) {
            taskFunction_();
        }
    }
}
