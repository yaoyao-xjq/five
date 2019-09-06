#pragma once

class Task
{
public:
    Task(void);
    virtual ~Task(void);

    virtual void Run() = 0;
};
