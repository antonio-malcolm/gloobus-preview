#include "task-manager.h"

static void work( gpointer data, gpointer) {
    Task* task = static_cast<Task*>(data);
    task->run();
    delete task;
}

TaskManager * TaskManager::instance_ = NULL;

TaskManager::TaskManager() {
    pool_ = g_thread_pool_new(work, this, MAX_THREADS, false, NULL);
}

TaskManager::~TaskManager() {
    g_thread_pool_free(pool_, false, false);
}

void TaskManager::add(Task* task) {
    if (instance_ == NULL) {
        instance_ = new TaskManager;
    }
    g_thread_pool_push(instance_->pool_, task, NULL);
}

