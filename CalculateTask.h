#ifndef CALCULATE_TASK_H
#define CALCULATE_TASK_H
#include "task.h"

class Engine;

class CalculateTask :
    public Task
{
public:
    CalculateTask(Engine* engine);
    ~CalculateTask(void);

    virtual void Run();
private:
    Engine* m_engine;
};
#endif