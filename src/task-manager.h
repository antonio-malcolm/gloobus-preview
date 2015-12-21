#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <boost/function.hpp>
#include <glib.h>

#include "task.h"

class TaskManager {
    public:
        static void add(Task *task);

    private:
        static const int MAX_THREADS = 5;
        static TaskManager* instance_;
        GThreadPool* pool_;

        TaskManager();
        ~TaskManager();
};

#endif /* TASK_MANAGER_H */