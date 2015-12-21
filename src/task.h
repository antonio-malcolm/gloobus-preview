#ifndef TASK_H
#define TASK_H

#include <boost/function.hpp>

class Task {
    public:
        typedef boost::function<void (void)> Function;

        Task(Function function):function_(function) {}
        void run() { function_(); }

    private:
        Function function_;
};

#endif // TASK_H