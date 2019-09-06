/*********************************************************************
RWLock: A simple and fast reader-writer lock class in C++
                   has characters of .NET ReaderWriterLock class
Copyright (C) 2006 Quynh Nguyen Huu

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

Email questions, comments or suggestions to quynhnguyenhuu@gmail.com
*********************************************************************/

/*********************************************************************
Introduction:
This implementation is inspired by System.Threading.ReaderWriterLock in
.NET framework. Following are some important statements I excerpted
(with some words modified) from .NET document.

http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemThreadingReaderWriterLockClassTopic.asp

"ReaderWriterLock is used to synchronize access to a resource.
At any given time, it allows either concurrent read access for
multiple threads, or write access for a single thread.
In a situation where a resource is changed infrequently, a
ReaderWriterLock provides better throughput than a simple
one-at-a-time lock, such as CriticalSection or Mutex.

This library works best where most accesses are reads, while
writes are infrequent and of short duration.

While a writer is waiting for active reader locks to be released,
threads requesting new reader locks will have to wait in the reader
queue. Their requests are not granted, even though they could share
concurrent access with existing reader-lock holders; this helps
protect writers against indefinite blockage by readers..."
*********************************************************************/

#pragma once

#include <map>
#include "threadbase.h"

HT_NAMESPACE_BEGIN

// Forward reference
class RWLock;
//////////////////////////////////////////////////////////////////
// RWLockNoReentrance class
// NOTE: This class doesn't support reentrance & lock escalation.
// May be deadlock in one of following situations: 
//  1) Call AcquireReaderLock twice (reentrance)
//     --> Revise execution flow.
//  2) Call AcquireWriterLock twice (reentrance)
//     --> Revise execution flow.
//  3) Call AcquireReaderLock then AcquireWriterLock (lock escalation)
//     --> Use ReleaseReaderAndAcquireWriterLock method
//  4) Call AcquireWriterLock then AcquireReaderLock (lock deescalation)
//     --> Use DowngradeFromWriterLock method
class HT_CLASS_EXPORT RWLockNoReentrance
{
public:
	RWLockNoReentrance();
	virtual ~RWLockNoReentrance();
	virtual bool AcquireReaderLock(uint32 dwTimeout = INFINITE);
	virtual void ReleaseReaderLock();
	virtual bool AcquireWriterLock(uint32 dwTimeout = INFINITE);
	virtual void ReleaseWriterLock();
	void DowngradeFromWriterLock();

	// When a thread calls UpgradeToWriterLock, the reader lock is released,
	// and the thread goes to the end of the writer queue. Thus, other threads
	// might write to resources before this method returns
	bool UpgradeToWriterLock(uint32 dwTimeout = INFINITE);
protected:
	// A critical section to guard all the other members
	mutable Mutex m_cs;
	// Auto-reset event, will be dynamically created/destroyed on demand
	mutable Event *m_hSafeToWriteEvent;
	// Manual-reset event, will be dynamically created/destroyed on demand
	mutable Event *m_hSafeToReadEvent;
	// Total number of writers on this object
	volatile int32 m_iNumOfWriter;
	// Total number of readers have already owned this object
	volatile int32 m_iNumOfReaderEntered;
	// Total number of readers are waiting to be owners of this object
	volatile int32 m_iNumOfReaderWaiting;
	// Internal/Real implementation
	void EnterCS() const;
	void LeaveCS() const;
	bool _ReaderWait(uint32 dwTimeout);
	bool _WriterWaitAndLeaveCSIfSuccess(uint32 dwTimeout);
	bool _UpgradeToWriterLockAndLeaveCS(uint32 dwTimeout);
	void _ReaderRelease();
	void _WriterRelease(bool blDowngrade);
};

//////////////////////////////////////////////////////////////////
// Inline methods
inline void RWLockNoReentrance::EnterCS() const { m_cs.enter(); }
inline void RWLockNoReentrance::LeaveCS() const { m_cs.leave(); }

//////////////////////////////////////////////////////////////////
// RWLockReentrance class
// This class supports reentrance & lock escalation
class HT_CLASS_EXPORT RWLock : public RWLockNoReentrance
{
public:
	RWLock();
	virtual ~RWLock();

	virtual bool AcquireReaderLock(uint32 dwTimeout = INFINITE);
	virtual void ReleaseReaderLock();

	// If current thread was already a reader
	// it will be upgraded to be writer automatically.
	// BE CAREFUL! Other threads might write to the resource
	// before current thread is successfully upgraded.
	virtual bool AcquireWriterLock(uint32 dwTimeout = INFINITE);
	virtual void ReleaseWriterLock();

	// Regardless of how many times current thread acquired reader
	// or writer locks, a call to this method will release all locks.
	// After that, any call to ReleaseWriterLock or ReleaseReaderLock
	// will raise exception in DEBUG mode.
	void ReleaseAllLocks();

	// Query thread's status
	uint32 GetCurrentThreadStatus() const;
	void GetCurrentThreadStatus(uint32* lpdwReaderLockCounter,
		uint32* lpdwWriterLockCounter) const;
protected:
	typedef std::map<uint32,uint32> CMapThreadToState;
	CMapThreadToState m_map;
};

//////////////////////////////////////////////////////////////////
// CAutoReadLockT & CAutoWriteLockT classes
// Couple of template helper classes which would let one acquire a lock
// in a body of code and not have to worry about explicitly releasing
// that lock if an exception is encountered in that piece of code or
// if there are multiple return points out of that piece.
template<class T>
class CAutoReadLock
{
public:
	CAutoReadLock(T& objLock) throw() : m_lock(objLock)
	{
		m_lock.AcquireReaderLock();
	}
	~CAutoReadLock() throw()
	{
		m_lock.ReleaseReaderLock();
	}
protected:
	T& m_lock;
};

template<class T>
class CAutoWriteLock
{
public :
	CAutoWriteLock(T& objLock) throw() : m_lock(objLock)
	{
		m_lock.AcquireWriterLock();
	}
	~CAutoWriteLock() throw()
	{
		m_lock.ReleaseWriterLock();
	}
protected:
	T& m_lock;
};

//////////////////////////////////////////////////////////////////
// Instances of above template helper classes
#define HT_LOCK_R(x) HT::CAutoReadLock<HT::RWLockNoReentrance>	___r_lock__(x);
#define HT_LOCK_W(x) HT::CAutoWriteLock<HT::RWLockNoReentrance> ___w_lock__(x);

HT_NAMESPACE_END


