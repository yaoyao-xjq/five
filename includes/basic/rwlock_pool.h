// RWLockPool.h: interface for the RWLockPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFX_POST_MUTEXMAP_H__CB2E7499_2818_426B_967C_1C5479CB3E00__INCLUDED_)
#define AFX_BFX_POST_MUTEXMAP_H__CB2E7499_2818_426B_967C_1C5479CB3E00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwlock.h"
#include "object.h"

HT_NAMESPACE_BEGIN

class RWLockPool  
{
public:
	RWLockPool(){};
	~RWLockPool(){};

	/**
	*	申请共享锁
	*/
	void AcquireReaderLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->AcquireReaderLock();
	};
	/**
	*	释放共享锁
	*/
	void ReleaseReaderLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->ReleaseReaderLock();
	};
	/**
	*	申请排它锁
	*/
	void AcquireWriterLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->AcquireWriterLock();
	};
	/**
	*	释放排它锁
	*/
	void ReleaseWriterLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->ReleaseWriterLock();
	};

	/**
	 *	删除指定ID的锁对象
	 */
	void DeleteLock(int32 iId)
	{
		m_MapMutex.AcquireWriterLock();

		StdPtrMap::iterator it = m_MutexMap.find(iId);
		
		if(it != m_MutexMap.end())
		{
			RWLock* OldLock = (RWLock*)(*it).second;
			m_MutexMap.erase(it);

			delete OldLock;
			OldLock = NULL;
		}

		m_MapMutex.ReleaseWriterLock();
	}

	/**
	 *	查找指定ID的锁对象并返回
	 */
	RWLock* GetLock(int32 iId)
	{
		m_MapMutex.AcquireReaderLock();
		
		StdPtrMap::iterator it = m_MutexMap.find(iId);

		if(it == m_MutexMap.end())
		{
			m_MapMutex.ReleaseReaderLock();

			return CreateLock(iId);
		}
		else
		{
			m_MapMutex.ReleaseReaderLock();

			return (RWLock*)(*it).second;
		}
	};

private:

	/**
	 *	新建指定ID的锁对象并返回
	 */
	RWLock* CreateLock(int32 iId)
	{
		RWLock* NewLock = new RWLock();

		m_MapMutex.AcquireWriterLock();

		m_MutexMap[iId] = NewLock;

		m_MapMutex.ReleaseWriterLock();

		return NewLock;
	}

	RWLock		m_MapMutex;	//保护锁表的锁
	StdPtrMap	m_MutexMap;	//post数据锁表
};

//全局锁表
static RWLockPool g_MutexMap;

/**
 *	自动共享互斥对象.
 *	当此对象构造时加上共享锁，析构时释放共享锁，使得编写代码简单而且不容易遗漏
 */
class RW_Map_ShareMutex
{
public:
	RW_Map_ShareMutex(int32 iId)
	{
		m_pMutex = g_MutexMap.GetLock(iId);
		m_pMutex->AcquireReaderLock();
	}

	~RW_Map_ShareMutex()
	{
		m_pMutex->ReleaseReaderLock();
	}
private:
	RWLock *m_pMutex;
};

/**
 *	自动排它互斥对象.
 *	当此对象构造时加上排它锁，析构时释放排它锁，使得编写代码简单而且不容易遗漏
 */
class RW_Map_ExclusiveMutex
{
public:
	RW_Map_ExclusiveMutex(int32 iId)
	{
		m_pMutex = g_MutexMap.GetLock(iId);
		m_pMutex->AcquireWriterLock();
	}
	
	~RW_Map_ExclusiveMutex()
	{
		m_pMutex->ReleaseWriterLock();
	}
private:
	RWLock *m_pMutex;
};

HT_NAMESPACE_END

// 自动互斥对象定义宏，此宏定义时相当于定义了一个BFX_POST_ShareMutex类型的变量，以代码
#define MAP_SHARE_LOCK(iid) HT_NAMESPACE::RW_Map_ShareMutex __bfx_post_map_sharemutex(iid)
// 自动互斥对象定义宏，此宏定义时相当于定义了一个BFX_POST_ExclusiveMutex类型的变量，以代码
#define MAP_EXCLUSIVE_LOCK(iid) HT_NAMESPACE::RW_Map_ExclusiveMutex __bfx_post_map_exclusivemutex(iid)
// 删除锁对象
#define MAP_DELETE_LOCK(iid) g_MutexMap.DeleteLock(iid)
// 进入共享锁
#define MAP_SHARE_LOCK_ENTER(iid) g_MutexMap.AcquireReaderLock(iid)
// 离开共享锁
#define MAP_SHARE_LOCK_LEAVE(iid) g_MutexMap.ReleaseReaderLock(iid)
// 进入共享锁
#define MAP_EXCLUSIVE_LOCK_ENTER(iid) g_MutexMap.AcquireWriterLock(iid)
// 离开共享锁
#define MAP_EXCLUSIVE_LOCK_LEAVE(iid) g_MutexMap.ReleaseWriterLock(iid)

#endif // !defined(AFX_BFX_POST_MUTEXMAP_H__CB2E7499_2818_426B_967C_1C5479CB3E00__INCLUDED_)
