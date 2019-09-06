#ifndef FIVE_THREAD_AI_H
#define FIVE_THREAD_AI_H

#include "basic/thread_object.h"
#include "Task.h"

using namespace HT;

class ThreadAI :
    public ThreadObject
{
public:
    ThreadAI(void);
    ~ThreadAI(void);

     void Run();

     void PushTask(Task* taskPtr);

     Task* PopTask();

private:
    std::vector<Task*> m_taskList;
    Mutex m_taskMutex;
};

#endif