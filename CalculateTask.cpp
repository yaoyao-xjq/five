#include "StdAfx.h"
#include "CalculateTask.h"
#include "Engine.h"

CalculateTask::CalculateTask(Engine* engine)
: m_engine(engine)
{
}

CalculateTask::~CalculateTask(void)
{
}

void CalculateTask::Run()
{
    if (NULL != m_engine)
    {
        m_engine->Thinking();
    }
}