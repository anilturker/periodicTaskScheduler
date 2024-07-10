#include "PeriodicTaskScheduler.h"
#include <algorithm>
#include <sys/time.h>
#include <iostream> // For logging

// Constructor initializes the scheduler with a specified number of threads
PeriodicTaskScheduler::PeriodicTaskScheduler(int numThreads) : nextTaskId_(1), running_(false) {
    // Initialize current time
    gettimeofday(&currentTime_, nullptr);

    // Create threads for executing tasks
    threads_ = std::vector<std::thread>(numThreads);
    for (auto &t : threads_) {
        t = std::thread([this] { this->threadRun(); });
    }
}

// Destructor stops the scheduler and cleans up resources
PeriodicTaskScheduler::~PeriodicTaskScheduler() {
    stop();
}

// Set the function that will be periodically executed
void PeriodicTaskScheduler::setTaskFunction(Task taskFunction) {
    std::lock_guard<std::mutex> lock(mutex_);
    taskFunction_ = taskFunction;
}

// Add a task to the scheduler with a specified interval
int PeriodicTaskScheduler::addTask(std::chrono::seconds interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    int taskId = nextTaskId_++;
    timeval nextExecutionTime = currentTime_;
    nextExecutionTime.tv_sec += interval.count();
    TaskInfo taskInfo = {interval, nextExecutionTime, taskId};
    tasks_[taskId] = taskInfo;
    taskQueue_.push(taskInfo);
    task_updated_.notify_all();
    std::cout << "Task " << taskId << " added with interval " << interval.count() << " seconds." << std::endl;
    return taskId;
}

// Remove a task from the scheduler using its task ID
void PeriodicTaskScheduler::removeTask(int taskId) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.erase(taskId);
    task_updated_.notify_all();
    std::cout << "Task " << taskId << " removed." << std::endl;
}

// Change the interval of an existing task
void PeriodicTaskScheduler::changeTaskInterval(int taskId, std::chrono::seconds newInterval) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        it->second.interval = newInterval;
        updateNextExecutionTime(it->second, currentTime_);
        taskQueue_.push(it->second);
        task_updated_.notify_all();
        std::cout << "Task " << taskId << " interval changed to " << newInterval.count() << " seconds." << std::endl;
    }
}

// Update the scheduler with an externally provided time
void PeriodicTaskScheduler::onNewTime(const timeval &externalTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentTime_ = externalTime;
    task_updated_.notify_all();
    std::cout << "Scheduler time updated to " << currentTime_.tv_sec << " seconds." << std::endl;
}

// Start the scheduler loop
void PeriodicTaskScheduler::start() {
    // Ensure the scheduler loop starts running
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // Start the scheduler loop in a separate thread
        schedulerThread_ = std::thread([this] { this->schedulerLoop(); });
        running_ = true;
    }
    std::cout << "Scheduler started." << std::endl;
}

// Stop the scheduler and join all threads
void PeriodicTaskScheduler::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
    }
    task_updated_.notify_all();
    for (auto &t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    if (schedulerThread_.joinable()) {
        schedulerThread_.join();
    }
    std::cout << "Scheduler stopped." << std::endl;
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
        // Wait until the next task is ready to be executed or the scheduler is stopping
        task_updated_.wait(lock, [this] {
            return !taskQueue_.empty() || !running_;
        });

        if (!running_) break;

        // Process tasks that are ready for execution
        while (!taskQueue_.empty() && timeElapsed(currentTime_, taskQueue_.top().nextExecutionTime)) {
            TaskInfo task = taskQueue_.top();
            taskQueue_.pop();

            // Check if the task is still valid (not removed)
            if (tasks_.find(task.taskId) != tasks_.end()) {
                readyTasks_.push_back(task);
                std::cout << "Task " << task.taskId << " is ready to execute." << std::endl;
            }
        }

        // Notify worker threads that tasks are available for execution
        task_updated_.notify_all();
    }

    // Notify any waiting threads to exit
    task_updated_.notify_all();
}

// Function that runs in each thread to execute tasks
void PeriodicTaskScheduler::threadRun() {
    while (true) {
        TaskInfo task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // Wait until there is a task available to execute or the scheduler is stopping
            task_updated_.wait(lock, [this] { return !readyTasks_.empty() || !running_; });
            if (!running_ && readyTasks_.empty()) return;
            if (!readyTasks_.empty()) {
                task = readyTasks_.front();
                readyTasks_.pop_front();
            }
        }
        // Execute the task function if set
        if (taskFunction_) {
            std::cout << "Executing task " << task.taskId << std::endl;
            taskFunction_();
        }

        // After execution, update the next execution time and requeue the task
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (tasks_.find(task.taskId) != tasks_.end()) {
                updateNextExecutionTime(task, currentTime_);
                taskQueue_.push(task);
                task_updated_.notify_all();
            }
        }
    }
}

// Check if a task is present in the scheduler
bool PeriodicTaskScheduler::isTaskPresent(int taskId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.find(taskId) != tasks_.end();
}

// Get the interval of a task
std::chrono::seconds PeriodicTaskScheduler::getTaskInterval(int taskId){
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        return it->second.interval;
    }
    return std::chrono::seconds(0);
}

// Check if the scheduler is running
bool PeriodicTaskScheduler::isRunning(){
    return running_;
}
