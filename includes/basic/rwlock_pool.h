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
	*	���빲����
	*/
	void AcquireReaderLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->AcquireReaderLock();
	};
	/**
	*	�ͷŹ�����
	*/
	void ReleaseReaderLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->ReleaseReaderLock();
	};
	/**
	*	����������
	*/
	void AcquireWriterLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->AcquireWriterLock();
	};
	/**
	*	�ͷ�������
	*/
	void ReleaseWriterLock(int32 iId)
	{
		RWLock* ThisLock = GetLock(iId);
		ThisLock->ReleaseWriterLock();
	};

	/**
	 *	ɾ��ָ��ID��������
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
	 *	����ָ��ID�������󲢷���
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
	 *	�½�ָ��ID�������󲢷���
	 */
	RWLock* CreateLock(int32 iId)
	{
		RWLock* NewLock = new RWLock();

		m_MapMutex.AcquireWriterLock();

		m_MutexMap[iId] = NewLock;

		m_MapMutex.ReleaseWriterLock();

		return NewLock;
	}

	RWLock		m_MapMutex;	//�����������
	StdPtrMap	m_MutexMap;	//post��������
};

//ȫ������
static RWLockPool g_MutexMap;

/**
 *	�Զ����������.
 *	���˶�����ʱ���Ϲ�����������ʱ�ͷŹ�������ʹ�ñ�д����򵥶��Ҳ�������©
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
 *	�Զ������������.
 *	���˶�����ʱ����������������ʱ�ͷ���������ʹ�ñ�д����򵥶��Ҳ�������©
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

// �Զ����������꣬�˺궨��ʱ�൱�ڶ�����һ��BFX_POST_ShareMutex���͵ı������Դ���
#define MAP_SHARE_LOCK(iid) HT_NAMESPACE::RW_Map_ShareMutex __bfx_post_map_sharemutex(iid)
// �Զ����������꣬�˺궨��ʱ�൱�ڶ�����һ��BFX_POST_ExclusiveMutex���͵ı������Դ���
#define MAP_EXCLUSIVE_LOCK(iid) HT_NAMESPACE::RW_Map_ExclusiveMutex __bfx_post_map_exclusivemutex(iid)
// ɾ��������
#define MAP_DELETE_LOCK(iid) g_MutexMap.DeleteLock(iid)
// ���빲����
#define MAP_SHARE_LOCK_ENTER(iid) g_MutexMap.AcquireReaderLock(iid)
// �뿪������
#define MAP_SHARE_LOCK_LEAVE(iid) g_MutexMap.ReleaseReaderLock(iid)
// ���빲����
#define MAP_EXCLUSIVE_LOCK_ENTER(iid) g_MutexMap.AcquireWriterLock(iid)
// �뿪������
#define MAP_EXCLUSIVE_LOCK_LEAVE(iid) g_MutexMap.ReleaseWriterLock(iid)

#endif // !defined(AFX_BFX_POST_MUTEXMAP_H__CB2E7499_2818_426B_967C_1C5479CB3E00__INCLUDED_)
