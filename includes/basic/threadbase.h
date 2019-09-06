/**
 *	@file threadbase.h
 *	6`O_3LV'3V@`
 */

#ifndef THREAD_BASE_H
#define THREAD_BASE_H 1

#include "config.h"

HT_NAMESPACE_BEGIN

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif	// INFINITE

extern void HT_FUNC_EXPORT HTSleep (int ms);
extern uint64 HT_FUNC_EXPORT HTGetTickCount ();

#ifndef NOT_USE_THREAD_BASE

// ------------------------- classes for multi thread ------------------------

#if defined(_WIN32)
#include <Windows.h>
/************************************************************************/
// Windows
/************************************************************************/


/** @class Mutex
*/
class HT_CLASS_EXPORT Mutex
{
public:
	Mutex()		{ InitializeCriticalSection(&cs); }
	~Mutex()		{ DeleteCriticalSection(&cs); }	
	void enter() const	{ EnterCriticalSection((LPCRITICAL_SECTION)&cs); }
	void leave() const	{ LeaveCriticalSection((LPCRITICAL_SECTION)&cs); }
private:
	CRITICAL_SECTION cs;
};

/** @class Event
*/
class HT_CLASS_EXPORT Event
{
public: 
	/// 构造函�?
	/// @param signaled 指定初始化时是否为有信号状态，true为由信号，false为无信号
	Event(bool signaled = false, bool manual_reset = true)	{ h = CreateEvent(NULL, manual_reset, signaled, NULL); }

	~Event()	{ CloseHandle(h); }
	/// 获取windows事件句柄
	HANDLE get_handle()	{ return h; }
	/// 等待信号
	void wait()	 const { WaitForSingleObject(h, INFINITE); }
	/// 超时等待信号
	/// @param mill 毫秒单位的超时事�?
	/// @return 返回true表示已接收到信号，否则表示超时返�?
	bool wait_with_timeout(time_t mill)  const { return WaitForSingleObject(h, mill) == WAIT_OBJECT_0;	}
	/// 产生信号
	void signal()  const { SetEvent(h); }
	/// 重置为无信号状�?
	void reset()  const { ResetEvent(h); }

private:
	HANDLE h;
}; 

#else	// _WIN32

#include <unistd.h> 

/************************************************************************/
/* Linux																									*/
/************************************************************************/
#define SYSCHECK(op) {int rc=(op); if (rc < 0)	printf("!!! error = %d", rc); }

class Mutex {
	mutable pthread_mutex_t cs;
	mutable int       count;
	mutable pthread_t owner;
	
public:

	void enter() const	{ 
 		pthread_t self = pthread_self();
		if (owner != self) { 
			pthread_mutex_lock(&cs); 
			owner = self;
		}
		count += 1;
    }
    
	void leave() const
    {
		assert(pthread_self() == owner);
		if (--count == 0) {
			owner = 0;
			pthread_mutex_unlock(&cs);
        } 
    }
    
	Mutex() 
	{ 
		count = 0;
		owner = 0;
		SYSCHECK(pthread_mutex_init(&cs, NULL));
	}
    
	~Mutex() { pthread_mutex_destroy(&cs);	}
};

//	implement by Sleep and Mutex
class Event { 
public: 
	Event(bool signaled = false, bool manual_reset = true) : manual(manual_reset)
	{
		this->signaled = signaled;
		this->wait_num = 0;
	}

	~Event()
	{
	}

	void wait()
	{
		cs.enter();
		long before_n_signals = n_signals;
		wait_num ++;
		while (1) { 
			long before_wait_num = wait_num;
			cs.leave();
			HTSleep(2);
			cs.enter();
			if (manual)
			{
				if (signaled || (n_signals != before_n_signals))
					break;
			}
			else
			{
				if (signaled && before_wait_num == wait_num)
				{
					signaled = manual;
					break;
				}
			}
		}

		cs.leave();
	}

	bool wait_with_timeout(uint32 mill)
	{
		if (mill == INFINITE)
		{
			wait();
			return true;
		}

		cs.enter();
		long before_n_signals = n_signals;
		uint64 begin_tick = HTGetTickCount();
		uint64 end_tick = begin_tick + mill;
		while (1) { 
			long before_wait_num = wait_num;
			cs.leave();
			HTSleep(2);
			cs.enter();

			if (manual)
			{
				if (signaled || (n_signals != before_n_signals))
					break;
			}
			else
			{
				if (signaled && before_wait_num == wait_num)
				{
					signaled = manual;
					break;
				}
			}

			uint64 tick = HTGetTickCount();
			if ( (end_tick > begin_tick))
			{
				if (tick > end_tick)
				{
					cs.leave();
					return false;
				}
			}
			else
			{
				if ((tick <= begin_tick) && (tick > end_tick))
				{
					cs.leave();
					return false;
				}
			}

		}
		cs.leave();
		return true;
	}

	void signal()
	{
		cs.enter();
		signaled = true;
		n_signals ++;
		cs.leave();
	}

	void reset() { signaled = false; }

	//HANDLE get_handle() { return NULL; }

protected:
	Mutex cs;
	int				wait_num;
	int             signaled; 
	long            n_signals;
	bool			manual;
}; 

// class Event { 
// public: 
// 	Event(bool signaled)
// 	{
// 		SYSCHECK(pthread_mutex_init(&cs, NULL));
// 		SYSCHECK(pthread_cond_init(&cond, NULL));
// 		this->signaled = signaled;
// 	}
// 
// 	~Event()
// 	{
// 		pthread_mutex_destroy(&cs);
// 		pthread_cond_destroy(&cond);
// 	}
// 
// 	void wait()
// 	{
// 		SYSCHECK(pthread_mutex_lock(&cs));
// 		long before_n_signals = n_signals;
// 		while (!signaled && n_signals == before_n_signals) { 
// 			SYSCHECK(pthread_cond_wait(&cond, &cs));
// 		}
// 		SYSCHECK(pthread_mutex_unlock(&cs));
// 	}
// 
// 	bool wait_with_timeout(time_t sec)
// 	{
// 		SYSCHECK(pthread_mutex_lock(&cs));
// 		long before_n_signals = n_signals;
// 		while (!signaled && n_signals == before_n_signals) { 
// #ifdef PTHREAD_GET_EXPIRATION_NP
// 			struct timespec rel_ts, abs_ts; 
// 			rel_ts.tv_sec = sec; 
// 			rel_ts.tv_nsec = 0;
// 			pthread_get_expiration_np(&rel_ts, &abs_ts);
// #else
// 			struct timespec abs_ts; 
// 			struct timeval cur_tv;
// 			gettimeofday(&cur_tv, NULL);
// 			abs_ts.tv_sec = cur_tv.tv_sec + sec; 
// 			abs_ts.tv_nsec = cur_tv.tv_usec*1000;
// #endif
// 			int rc = pthread_cond_timedwait(&cond, &cs, &abs_ts);
// 			if (rc == ETIMEDOUT) { 
// 				pthread_mutex_unlock(&cs);
// 				return false;
// 			}
// 			SYSCHECK(rc);
// 		} 
// 		SYSCHECK(pthread_mutex_unlock(&cs));
// 		return true;
// 	}
// 
// 	void signal()
// 	{
// 		SYSCHECK(pthread_mutex_lock(&cs));
// 		signaled = true;
// 		n_signals += 1;
// 		SYSCHECK(pthread_cond_broadcast(&cond));
// 		SYSCHECK(pthread_mutex_unlock(&cs));
// 	}
// 
// 	void reset() { signaled = false; }
// 
// 	HANDLE get_handle() { return NULL; }
// 
// protected:
// 	pthread_mutex_t cs;
// 	pthread_cond_t  cond; 
// 	int             signaled; 
// 	long            n_signals;
// }; 

inline int32 GetCurrentThreadId ()
{
	return pthread_self();
}

inline int32 GetCurrentProcessId ()
{
	return getpid();
}

#endif // PTHREADS

/**
 *	自动互斥对象.
 *	当此对象构造时进入指定的Mutex，析构时离开，使得编写代码简单而且不容易遗�?
 */
class HT_CLASS_EXPORT Critical
{
public:
	Critical(const Mutex& cs) : mutex(cs)
	{ cs.enter(); }
	~Critical()	{ mutex.leave(); }
private:
	const Mutex& mutex;
};

#endif // NOT_USE_THREAD_BASE

HT_NAMESPACE_END

/// 自动互斥对象定义宏，此宏定义时相当于定义了一个simple_critical类型的变量，以代�?
#define HT_CS(mutex) HT_NAMESPACE::Critical __critical_section(mutex)
/// �?CS相同，仅变量名不同，使得可以同时对多个互斥对象进行操�?
#define HT_CS1(mutex) HT_NAMESPACE::Critical __critical_section1(mutex)
/// �?CS相同，仅变量名不同，使得可以同时对多个互斥对象进行操�?
#define HT_CS2(mutex) HT_NAMESPACE::Critical __critical_section2(mutex)


#endif	// THREAD_BASE_H
