#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <functional>
#include <future>
#include <string>


// This class is a pure virtual class that can be implemented as a
// SerialTaskQueue or a ConcurrentTaskQueue.

class TaskQueue  {
public:
    using Task = std::function<void()>;
    virtual void runTaskInQueue(const Task &task) = 0;
    virtual void runTaskInQueue(Task &&task) = 0;
    virtual std::string getName() const {
        return "";
    };

    // Run a task in the queue sychronously. This means that the task is
    // executed before the method returns.
    void syncTaskInQueue(const Task &task) {
        std::promise<int> prom;
        std::future<int> fut = prom.get_future();
        runTaskInQueue([&]() {
            task();
            prom.set_value(1);
        });
        fut.get();
    };
    virtual ~TaskQueue() {}
};
#endif // TASKQUEUE_H