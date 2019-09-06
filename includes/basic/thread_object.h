// ThreadObject.h: interface for the ThreadObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_THREAD_OBJECT_H_5555)
#define _THREAD_OBJECT_H_5555	1

#include "threadbase.h"

HT_NAMESPACE_BEGIN

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif	// INFINITE

class HT_CLASS_EXPORT ThreadObject  
{
public:
	ThreadObject();
	virtual ~ThreadObject();
	bool Wait (unsigned long Milli = INFINITE);
	bool IsRuning();
	bool IsBreak ();
	int StartThread(bool bAutoDelete);
	bool Cancel(unsigned long nWaitMilli = INFINITE);
	void KillThread ();
	int GetThreadId () { return (unsigned long)m_iThreadId; }

protected:
	virtual void OnExit();
	virtual void Run();

#ifdef _WIN32
	unsigned long	m_iThreadId;
	HANDLE	m_hThread;
	static int ThreadProc(void* arg);
#else // _WIN32
	pthread_t m_iThreadId;
	static void* ThreadProc(void* arg);
#endif // _WIN32
	
	bool m_bUserBreak;

private:
	bool m_bAutoDelete;
	Mutex m_Mutex;
};

HT_NAMESPACE_END

#endif // !defined(_THREAD_OBJECT_H_5555)
