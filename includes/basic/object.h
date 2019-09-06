/**
 *	@file object.h
 *	��������
 */

#ifndef	HT_OBJECTS
#define HT_OBJECTS 1

#ifdef _MSC_VER
// ���� stl warning
#pragma warning(disable:4786)
#pragma warning(disable:4291)
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
#pragma warning(disable: 4018)
#pragma warning(disable: 4800)
#pragma warning(disable: 4244)
// ���� VS2005 ʹ�þɺ��������� warning
#pragma warning(disable: 4996)
#endif // _MSC_VER

#include "config.h"
#include "threadbase.h"
#include "ht_except.h"
#include <iostream>

/*
#ifdef _WIN32

namespace HTSTD {

HT_FUNC_EXPORT void *__ht_alloc (size_t s);
HT_FUNC_EXPORT void __ht_free (void * _Memory);

// TEMPLATE CLASS ht_allocator
template<class _Ty>
class ht_allocator : public HTSTD::allocator<_Ty>
{	// generic ht_allocator for objects of class _Ty
public:
	template<class _Other>
	struct rebind
	{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef ht_allocator<_Other> other;
	};

	ht_allocator() _THROW0()	{	}
	ht_allocator(const ht_allocator<_Ty>&) _THROW0() {	}

	template<class _Other>
	ht_allocator(const ht_allocator<_Other>&) _THROW0()	{	}

	template<class _Other>
	ht_allocator<_Ty>& operator=(const ht_allocator<_Other>&) {	return (*this);
	}

	void deallocate(pointer _Ptr, size_type)
	{	// deallocate object at _Ptr, ignore size
		__ht_free(_Ptr);
	}

	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
		if (_Count <= 0)
			_Count = 0;
		else if (((_SIZT)(-1) / _Count) < sizeof (_Ty))
			throw std::bad_alloc();
		// allocate storage for _Count elements of type _Ty
		return ((_Ty _FARQ *)__ht_alloc(_Count * sizeof (_Ty)));
	}

	pointer allocate(size_type _Count, const void _FARQ *)
	{	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	}
};

#define allocator ht_allocator

}; // HTSTD

#endif // _WIN32
*/

HT_NAMESPACE_BEGIN

class HObject;

/*
 *	��׼���ݼ��϶���
 */
/// StdArray
template<class TYPE>
class StdArray : public HTSTD::vector<TYPE>
{
public:
	TYPE *pbegin () { return &((*this)[0]); }
	const TYPE *pbegin () const { return &*this->begin();}
	TYPE *pend () { return &*this->end ();}
	const TYPE *pend () const { return &*this->end();}
};

/// StdList
template<class TYPE>
class StdList : public HTSTD::list<TYPE>{};

/// StdString
class HT_CLASS_EXPORT StdString : public HTSTD::string
{
	typedef HTSTD::string TBaseString;
public:
		//construct
	StdString (const char *S = NULL)
		{ if (S) assign (S); }
	StdString (const TBaseString &X)
		{ assign(X, 0, npos); }
	StdString(const TBaseString& X, size_type P, size_type M)
		{ assign(X, P, M); }
	StdString(const char *S, size_type N)
		{ assign(S, N); }
	StdString(size_type N, char C)
		{ assign(N, C); }
	StdString(const_iterator F, const_iterator L)
		{ assign(F, L); }

	char *pbegin () { return (char *)c_str(); }
	const char *pbegin () const { return c_str(); }
	char *pend () { return &*end (); }
	const char *pend () const { return &*end(); }
		// Extern functions
	int printf (const char *lpszFormat, ...);
	void trim_left ();
	void trim_right ();
	void make_upper ()
	{ //strupr(pbegin());
		HTSTD::transform(begin(), end(), begin(), (int(*)(int))toupper);
	}
	void make_lower ()
	{// _strlwr(pbegin());
		HTSTD::transform(begin(), end(), begin(), (int(*)(int))tolower);
	}
	float64 to_double (int npos = 0) const;
	int to_int (int npos = 0) const;
	int compare_no_case (const char *right);
	int compare_no_case (const StdString& right);
	//
	// ��ϧstring[]���� char *[]��ͻ��û�취�ˣ��������ֱ��?
	// ��ˬ
	//	operator const char *() const { return c_str();	}
	void Replace(const char* oldstr, const char* newstr);
	void trim_all();
};

/// �ַ�����ʽ������
class HT_CLASS_EXPORT StdFormat : private StdString
{
public:
	StdFormat (const char *lpszFormat, ...);
	operator const char *() const { return c_str();	}
	operator StdString& () { return *this; }
	operator const StdString& () const { return *this; }
};

// string ��û��format�������Դ˺�����??
#ifdef _WIN32		/* ANSI compatible version*/
extern int HT_CLASS_EXPORT g_FormatString(HTSTD::string &str, char *lpszFormat, ...);
#else                   /* UNIX compatible version*/
	extern int HT_CLASS_EXPORT g_FormatString(HTSTD::string &str, char *lpszFormat);
#endif

// �����ʽ���ַ�������־��?v
extern void HT_CLASS_EXPORT AppLog (const char* strFileName, const char *strFormat, ...);

typedef HTSTD::map<long, void *>			StdPtrMap;
typedef HTSTD::multimap<long, void *>		StdPtrMultiMap;
typedef	HTSTD::map<StdString, void *>		StdStringPtrMap;
typedef	HTSTD::map<StdString, StdString>	StdStringStringMap;
typedef HTSTD::map<StdString, HObject>		StdStringHPtrMap;

typedef StdArray<char>		StdCharArray;
typedef StdArray<short>		StdShortArray;
typedef StdArray<long>		StdLongArray, StdIntArray;
typedef StdArray<float32>	StdFloatArray;
typedef StdArray<float64>	StdDoubleArray;
typedef StdArray<void *>	StdPtrArray;
typedef StdArray<StdString>	StdStringArray;

typedef	StdList<long>		StdLongList, StdIntList;
typedef StdList<float32>	StdFloatList;
typedef StdList<float64>	StdDoubleList;
typedef StdList<void *>		StdPtrList;

typedef std::ios			StdIOBase;
typedef std::istream		StdIStream;
typedef std::ostream		StdOStream;

class Object;
	class GmIStream;
	class GmOStream;

/**
* HT Standard exception class
* all of exception use this class or derived from this in HT
*/
class HT_CLASS_EXPORT Except : public std::exception
{
public:
	Except (int code, const char *const strWhat = NULL);
	virtual ~Except() throw();

	int32 GetCode () const { return m_code; }
	virtual const char* what() const throw()
	{
		return m_strWhat.c_str();
	};

protected:
	int m_code;
	HTSTD::string m_strWhat;
};

/************************************************************************/
/* ClassInfo                                                          */
/************************************************************************/
#ifdef UNICODE
#define GetClassInfo  GetClassInfoW
#else
#define GetClassInfo  GetClassInfoA
#endif // !UNICODE
class HT_CLASS_EXPORT ClassInfo 
{
  public:
    const char*  name;

	// size of fixed part of the class
    const size_t size;

	// �벻Ҫ�Լ��ô˹��캯������GmClassInfo�����ʵ�����⽫Ӱ����������б����ȷ??
	ClassInfo(const char* name, 
		size_t size, void (*cons)(void*), 
		ClassInfo *p_base_class=NULL);

	int getclassid(){return id;}

	bool IsClassof (const char *name) const;
	bool IsClassof (int id) const;
	bool IsClassof (const ClassInfo &Info) const;

  public:
      ~ClassInfo();

      static ClassInfo * GetClassInfo (int Id);

      static ClassInfo * GetClassInfo (char const* class_name);

      static int GetClassNum ();

      static bool LoadClassInfo ();

      static bool SaveClassInfo ();

  protected:

	int id;
	ClassInfo* collision_chain;
	ClassInfo* next;
	ClassInfo *base_class;
	void (*constructor)(void*);

	static int count;
	static ClassInfo* list;
	static ClassInfo* hash_table[];
	static ClassInfo * _GetClassInfo (char const* class_name);
	static ClassInfo** m_pDbsClassMapping;

  private:
      static Mutex m_Mutex;
};

// C++ ��չ���͹���?
class HT_CLASS_EXPORT ClassInfoEx : public ClassInfo
{
public:
	ClassInfoEx(
		const char* name,
		size_t size,
		void (*cons)(void*),
		ClassInfo *baseclass,
		Object *(*obj_new)(void *p),
		long obj_type);

	Object* CreateInstance (size_t nMaxSize=0, void *p=NULL)
	{
		if (NULL == p_Create_Obj)
			return NULL;
		if(p == NULL)
		{
			p = malloc(size);
			if (NULL == p)
				return NULL;
		}
		else if(nMaxSize < size)
			return NULL;
		Object *pObj = p_Create_Obj(p);
		return pObj;
	}

	//����p�ṩ���ڴ�ռ�����ڴ棬���pΪNULL�������ڲ�����ȫ���ڴ�
	static Object *CreateObjectByType(long nObjType, size_t nMaxSize=0, void *p=NULL);
	static Object *CreateObjectByClass(const char * pstrClassName, size_t nMaxSize=0, void *p=NULL);
	static ClassInfoEx *GetVertexClassInfo(long nObjType);

private:
	long Obj_Type;
	Object *(*p_Create_Obj)(void *p);
	static HTSTD::map<long, ClassInfoEx *> *p_ObjInfoMap;
};

/**
 *	ֻ��?vObject �����������֧��ע��
 *	ʹ��������Ϣ��Ҫ�������裺
 *	- ����Ķ����м��� OBJ_..._CLASS_INFO�꣬����ȱʡ�ĺ���
 *	- Ȼ����Ҫ��.cpp�е���OBJ_..._REGISTER
 *	
 *	C++ ��������

*/
#define OBJ_CLASS_INFO(CLASS_NAME)									\
public:																\
	static CLASS_NAME * CreateInstance();							\
	static HT_NAMESPACE::ClassInfoEx m_ClassInfo;							\
	static int getclassid() {return m_ClassInfo.getclassid();}		\
	static uint32 getobjtype();										\
	static const char * getclassname() {return #CLASS_NAME;}		\
	static HT_NAMESPACE::ClassInfo *getbaseclass () { return NULL; }			\
	static const type_info &get_type_info ()						\
			{ return typeid(CLASS_NAME);}							\
	virtual uint32 GetObjType() const;								\
	virtual const char * GetClassName() const {return #CLASS_NAME;}	\
	virtual HT_NAMESPACE::Object *CreateSameObject() const;						\
protected:															\
    static void constructor(void* ptr);								\
	static HT_NAMESPACE::Object * new_obj(void *p);

/**
 *	������ע��(��֧��ʵ��������)
 */
#define OBJ_REGISTER_ABSTRACT(CLASS_NAME)							\
	HT_NAMESPACE::ClassInfoEx CLASS_NAME::m_ClassInfo(					\
							#CLASS_NAME,							\
							sizeof(CLASS_NAME),						\
							&CLASS_NAME::constructor,				\
							NULL,									\
							&CLASS_NAME::new_obj,					\
							CLASS_NAME::getobjtype());				\
	void CLASS_NAME::constructor(void* ptr) {}						\
	HT_NAMESPACE::Object * CLASS_NAME::new_obj(void *p)							\
	{ return NULL; }												\
	CLASS_NAME * CLASS_NAME::CreateInstance()						\
	{ return NULL; }												\
	uint32 CLASS_NAME::getobjtype() { return(0); }					\
	HT_NAMESPACE::Object *CLASS_NAME::CreateSameObject()  const					\
	{ return NULL; }												\
	uint32 CLASS_NAME::GetObjType() const { return(0); }

/// ������ע��(��֧���Զ����� GetObjType () == 0)
#define OBJ_REGISTER(CLASS_NAME)									\
	HT_NAMESPACE::ClassInfoEx CLASS_NAME::m_ClassInfo(					\
							#CLASS_NAME,							\
							sizeof(CLASS_NAME),						\
							&CLASS_NAME::constructor,				\
							NULL,									\
							&CLASS_NAME::new_obj,					\
							CLASS_NAME::getobjtype());				\
	void CLASS_NAME::constructor(void* ptr) {}						\
	HT_NAMESPACE::Object * CLASS_NAME::new_obj(void *p)							\
	{ return (new(p)CLASS_NAME()); }								\
	CLASS_NAME * CLASS_NAME::CreateInstance()						\
	{ return (new CLASS_NAME()); }									\
	uint32 CLASS_NAME::getobjtype() { return(0); }					\
	HT_NAMESPACE::Object *CLASS_NAME::CreateSameObject()  const					\
	{ return (new CLASS_NAME); }									\
	uint32 CLASS_NAME::GetObjType() const { return(0); }

/// ������ע���֧�������Զ����� (GetObjType () == Type)
#define OBJ_REGISTER_TYPE(CLASS_NAME, OBJTYPE)						\
	HT_NAMESPACE::ClassInfoEx CLASS_NAME::m_ClassInfo(					\
							#CLASS_NAME,							\
							sizeof(CLASS_NAME),						\
							&CLASS_NAME::constructor,				\
							CLASS_NAME::getbaseclass(),				\
							&CLASS_NAME::new_obj,					\
							CLASS_NAME::getobjtype());				\
	void CLASS_NAME::constructor(void* ptr) {						\
		memset(ptr, 0, sizeof(CLASS_NAME));							\
		new (ptr) CLASS_NAME ();}									\
	HT_NAMESPACE::Object * CLASS_NAME::new_obj(void *p)							\
	{ return (new(p)CLASS_NAME()); }								\
	CLASS_NAME * CLASS_NAME::CreateInstance()						\
	{ return (new CLASS_NAME()); }									\
	HT_NAMESPACE::Object *CLASS_NAME::CreateSameObject()  const					\
	{ return (new CLASS_NAME); }									\
	uint32 CLASS_NAME::getobjtype() { return(OBJTYPE); }				\
	uint32 CLASS_NAME::GetObjType() const { return(OBJTYPE); }

/************************************************************************/
/* Object                                                               */
/************************************************************************/
class HT_CLASS_EXPORT Object
{
	OBJ_CLASS_INFO (Object)

/*	static Object * CreateInstance();
	static HT_NAMESPACE::ClassInfoEx m_ClassInfo;
	static int getclassid() {return m_ClassInfo.getclassid();}	
	static uint32 getobjtype();
	static const char * getclassname() {return "Object";}	
	static HT_NAMESPACE::ClassInfo *getbaseclass() { return NULL; }
	static const type_info &get_type_info()	{ return typeid(Object);}	
	virtual uint32 GetObjType() const;	
	virtual const char * GetClassName() const {return "Object";}
	virtual HT_NAMESPACE::Object *CreateSameObject() const;
protected:
	static void constructor(void* ptr);
	static HT_NAMESPACE::Object * new_obj(void *p);
*/

public:
	Object () : m_RefCount (0)
	{

	}

	virtual ~Object()
	{

	}
	
	// Ref
#ifdef _WIN32
	void AddRef () const
	{
		InterlockedIncrement(&m_RefCount);
	};
	void Release () const
	{
		if (InterlockedDecrement(&m_RefCount) <= 0)
		{
			try {
				::delete this;
			}
			catch (...) {
			}
		}
	}

	long GetRefCount () const { return m_RefCount; };
#else // _WIN32
	void AddRef () const
	{
		m_ObjectMutex.enter();
		m_RefCount++;
		m_ObjectMutex.leave();
	};

	void Release () const
	{
		m_ObjectMutex.enter();
		if (-- m_RefCount <= 0)
		{
			try {
			    ::delete this;
			}
			catch (...) {
			}
		}
		m_ObjectMutex.leave();
	}

	long GetRefCount () const { HT_CS(m_ObjectMutex); return m_RefCount; };

#endif // _WIN32

	void * operator new (size_t size)
	{
		return ::new char[size];
	}
	void * operator new (size_t size, void *p)	{  return p;  }
	// debug new
	void * operator new (size_t size, const char *lpszFileName, int nLine)	{	return ::new char[size];	}
	// delete
	void operator delete (void *p)	
    {  
        if (NULL != p) 
            delete[] p; 
        return; 
    }

private:
	mutable long m_RefCount;
	static Mutex m_ObjectMutex;
	friend class HObject;
};

/************************************************************************/
/* ����ָ��                                                             */
/************************************************************************/
class HT_CLASS_EXPORT HObject
{
public:
	// construct
	inline HObject();
	inline HObject(const HObject &right);
	inline HObject (const Object *pObj);
	inline ~HObject();
	
	// operator
	inline HObject & operator=(const HObject &right);
	inline HObject & operator = (const Object *pObj);
	inline bool operator < (const HObject &right) const	{	return right.GetObject() < m_pObject;	}
	inline bool operator == (const HObject &right) const	{	return right.GetObject() == m_pObject;	}
	inline bool operator != (const HObject &right) const	{	return right.GetObject() != m_pObject;	}
	inline bool operator == (const Object *right) const;
	inline bool operator != (const Object *right) const;
		
	inline Object * GetObject ();
	inline const Object * GetObject () const;

protected:
	Object* m_pObject;
	inline void Attach (const Object *pObj);
	inline void Detach ();
};

/**
 *	@class HObject object.h "object.h"
 *	����ָ���࣬?Object �������ʵ���Զ��������չ���.
 *	HObject ��ֵʱ���?\Object ʵ�������ã���֮�ͷ�����?
 *	��û�������������������£�ȷ���ھ��ָ��˶���ʱ�Զ�ɾ�����ͷ��ڴ�?
 *	���ԣ�HObject ��ϵͳ�е���õĻ�����?
 *
 *	HObject ������֤������Զ�ɾ�����������ֶ�������ͣ����?
 *	Ϊ�˱�֤���Ͱ�ȫ�����ⲿ����ʹ��ʱ������ģ?dHPtr<T> ����ʹ��?
 */

/**
 *	Ĭ�Ϲ��캯?`
 */
HObject::HObject() : m_pObject(NULL)
{

}

/**
 *	�������캯�����Զ��ͷžɵ����ã���ָ���µĶ���
 *	@param right ��������HObject�������
 */
HObject::HObject(const HObject &right) : m_pObject(NULL)
{
	Attach (right.m_pObject);
}

/**
 *	���캯?` ֱ���������� Object ָ����г�ʼ��
 *	@param pObj ��Ҫ���õĶ������ NULL�����ͬ��Ĭ�Ϲ��캯?`
 */
HObject::HObject (const Object *pObj) : m_pObject(NULL)
{
	Attach (pObj);
}

/**
 *	��������������˶������ڴ��Զ������ͷ�����
 */
HObject::~HObject()
{
	Detach ();
}

/**
 *	��ֵ�������Զ��ͷžɵ����ã���ָ���µĶ���
 */
HObject & HObject::operator=(const HObject &right)
{
	Attach (right.GetObject());
	return *this;
}

/**
 *	ָ�����ö����Զ��ͷžɵ����ã���ָ���µĶ���
 */
HObject & HObject::operator = (const Object *pObj)
{
	Attach(pObj);
	return *this;
}

/**
 *	�жϵ�ǰ����������= right
 *	@return ����true��ʾ��ȣ����򲻸�
 */
bool HObject::operator == (const Object *right) const
{
	return m_pObject == right;
}

/**
 *	�жϵ�ǰ���õĶ�������!= right���˲���?== �����෴
 *	@return ����false��ʾ��ȣ����򲻸�
 */
bool HObject::operator != (const Object *right) const
{
	return m_pObject != right;
}

/** @name ��ȡ����ԭʼָ��
 *  Ϊ��֧�ֻ�״�����ã��ṩ�ӿ�ֱ�ӻ�ȡ�����ԭʼָ�룬
 *	ʹ��ʱ��ע���������ù�ϵ����ֹ��ָ��ĳ���
 *	@return ���������ö����ԭʼָ?
 */
//@{
/**
 *	@return ���������ö����ԭʼָ?
 */
Object * HObject::GetObject ()
{
	return m_pObject;
}

/**
 *	@return ���������ö����constԭʼָ��
 */
const Object * HObject::GetObject () const
{
	return m_pObject;
}
//@}

#ifdef _WIN32
/**
 *	Attach the pObj to HObject
 */
/**
 *	ָ�����ö����Զ��ͷžɵ����ã���ָ���µĶ��󣬴˶���Ĺ����� operator = ��ͬ
 */
void HObject::Attach (const Object *pObj)
{
	if (m_pObject == pObj)
		return;

	if (NULL != pObj)
		pObj->AddRef();
	
	// Ϊ��ֹ���߳��·��ʳ�ͻ���ȱ���ָ�룬�����ã����Release
	Object *pOld = m_pObject;
	InterlockedExchangePointer((void **)&m_pObject, (void *)pObj);
	if (NULL != pOld)
		pOld->Release();
}

/**
 *	����������ã��������HObject������ΪNULL
 */
void HObject::Detach ()
{
	if (NULL != m_pObject)
	{
		m_pObject->Release();
		InterlockedExchangePointer((void **)&m_pObject, NULL);
	}
}

#else	// _Win32

void HObject::Attach (const Object *pObj)
{
	Object::m_ObjectMutex.enter();
	if (m_pObject == pObj)
	{
		Object::m_ObjectMutex.leave();
		return;
	}

	if (NULL != pObj)
		pObj->AddRef();
	if (NULL != m_pObject)
		m_pObject->Release();
	m_pObject = (Object *)pObj;

	Object::m_ObjectMutex.leave();
	return;
}

void HObject::Detach ()
{
	Object::m_ObjectMutex.enter();
	if (NULL != m_pObject)
	{
		m_pObject->Release();
		m_pObject = NULL;
	}
	Object::m_ObjectMutex.leave();
}
#endif // _Win32
/************************************************************************/
/* HPtr   */
/************************************************************************/
enum HPtr_Construct
{
	HP_NULL = 0,	///< Ĭ�Ϲ�?
	HP_NEW,			///< ����һ��ʵ����ʼ��??Ptr
};

template <class T>
class HPtr : public HObject
{
public:
	HPtr (HPtr_Construct bCreate = HP_NULL)
	{
		if (bCreate == HP_NEW)
			CreateInstance(); 
	}

	HPtr(const HPtr<T> &r) : HObject(r) {}
	HPtr(const HObject& r) { *this = r; }
	HPtr(const T *p) { *this = p; }
	~HPtr()	{}
	
	void CreateInstance ()
	{
#ifdef _DEBUG
		// assert (typeid(T) == T::get_type_info());	// ����Ƿ������ע��
		if (typeid(T) != T::get_type_info())
		{
			printf("THE CLASS %s IS NOT REGIST, CALL OBJ_CLASS_INFO AND OBJ_REGISTER...", typeid(T).name());
			assert (0);
		}
#endif // _DEBUG
		T *p = (T *)T::CreateInstance();
		assert(p != NULL);
		Attach(p);
	}

	T* operator -> () const		{ return (T*)m_pObject; }
	T* GetObject()				{ return (T*)m_pObject; }
	const T* GetObject() const	{ return (T*)m_pObject; }

	const type_info& GetTypeInfo ()
	{
		if (NULL == m_pObject)
			return typeid(NULL);
		else
			return typeid (*m_pObject);
	}

	inline bool operator==(const HObject& r) const { 
		return GetObject() == r.GetObject();
	}
	
	inline bool operator!=(const HObject& r) const { 
		return GetObject() != r.GetObject();
	}
	
	inline bool operator==(const T * p) const { 
		return m_pObject == (const Object *)p;
	}
	
	inline bool operator!=(const T * p) const { 
		return m_pObject != (const Object *)p;
	}
	
	inline bool operator < (const HObject& r) const {
		return m_pObject < r.GetObject ();
	}

	HPtr<T> & operator = (const HPtr<T> &r)
	{	Attach (r.GetObject());	return *this;	}

	HPtr<T> &operator = (const Object* p)
	{
		if (m_pObject != (const Object *)p)	{
			if (NULL != p) {
				const T* pp;
				try	{
					pp = dynamic_cast<const T *> (p);
				} catch (...) {
					pp = NULL;
				}

				if (NULL == pp)	{	
					if (p->GetRefCount () == 0)	p->Release ();
				} else {
					Attach(pp);
				}
			}
			else
				Detach ();
		}
		return *this;
	}

	HPtr<T> & operator = (const HObject& hnd)
	{
		if (hnd.GetObject() != m_pObject) {
			const T* pp;
			try {
				pp = dynamic_cast<const T *> (hnd.GetObject());
			} catch (...) {
				pp = NULL;
			}
			Attach (pp);
		}
		
		return *this;
	}
};

inline bool operator == (const Object *p, const HObject hObj)
{	return hObj == p;	}
inline bool operator != (const Object *p, const HObject hObj)
{	return hObj != p;	}

typedef HPtr<Object> GmObjectPtr;
typedef StdArray<HObject> StdHObjArray;
typedef StdList<HObject> StdHObjList;
typedef std::map<std::string, HObject> StdHObjMap;

/**
 * ����ִ��Ч��ͳ���࣬ͨ���Ժ���������ע�ᣬ����ͳ�ƺ�����ִ��ʱ��
 * ����Ч�ʷ���ע�ᣬ�����Ҫ����Ч�ʷ������ں������ʼ�������
 * ����?
 * <PRE>
 *
 *	void * Fn ()
 *	{
 *		FUN_TIME (Object::GetObject);
 *		....
 *	}
 *
 * </PRE>
 */
class HT_CLASS_EXPORT FunTimeHook
{
public:
	FunTimeHook (char *name);
	~FunTimeHook ();

	/**
	 *  @param pstrFunName ������
	 *	@param fRunTime ���غ���ִ����ʱ�䣬��λ��ms
	 *	@param nTimes ���غ�������ִ�д���
	 */
	static bool GetTimeByName (const char * pstrFunName, float64 &fRunTime, int &nTimes, int64 &maxTime, int64 &minTime);
	static void GetRegistedFuncs (StdStringArray& Funcs);
	static void ClearItems ();
private:
	char* m_name;
#ifdef _WIN32
	LARGE_INTEGER tick;
#else
    struct timeval tick;
#endif
};

#ifdef USE_FUN_TIME_HOOK
	#ifdef _WIN32
		#define FUN_TIME(FUN_NAME) FunTimeHook _____GmFunTimeHook(#FUN_NAME)
	#else
        #define FUN_TIME(FUN_NAME) FunTimeHook funTimeHook(FUN_NAME);
	#endif // _WIN32
#else	//USE_FUN_TIME_HOOK
	#define FUN_TIME(FUN_NAME)
#endif	//USE_FUN_TIME_HOOK

/************************************************************************/
/* ȫ�ֺ���                                                             */
/************************************************************************/
void HT_FUNC_EXPORT AppDebug(const char *strFormat, ...);
void HT_FUNC_EXPORT AppWarning (const char *strFormat, ...);
void HT_FUNC_EXPORT AppError (const char *strFormat, ...);
void HT_FUNC_EXPORT AppEvent (const char *strFormat, ...);

HObject
HT_FUNC_EXPORT g_CreateObjectByType(long nObjType, size_t nMaxSize=0, void *p=NULL);
HObject
HT_FUNC_EXPORT g_CreateObjectByClass(const char * pstrClassName, size_t nMaxSize=0, void *p=NULL);

HT_NAMESPACE_END

#endif // HT_OBJECTS
