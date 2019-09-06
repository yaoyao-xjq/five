#include "StdAfx.h"
#include "ThreadAI.h"

ThreadAI::ThreadAI(void)
{
}

ThreadAI::~ThreadAI(void)
{
}

void ThreadAI::Run()
{
    Task *taskPtr = NULL;
    while (!IsBreak())
    {
        taskPtr = PopTask();
        if (taskPtr != NULL)
        {
            taskPtr->Run();

            delete taskPtr;
            taskPtr = NULL;
        }

        Wait(100);
    }
}

void ThreadAI::PushTask( Task* taskPtr )
{
    HT_CS(m_taskMutex);

    m_taskList.push_back(taskPtr);
}

Task* ThreadAI::PopTask()
{
    HT_CS(m_taskMutex);

    if (m_taskList.size() == 0)
    {
        return NULL;
    }
    else
    {
        Task* taskPtr = m_taskList.back();
        m_taskList.pop_back();

        return taskPtr;
    }
}