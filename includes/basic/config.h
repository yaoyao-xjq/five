// config.h

#ifndef __SYS_TYPES_H_AKPIOOPE90GHLSX566WJH
#define __SYS_TYPES_H_AKPIOOPE90GHLSX566WJH 1

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <typeinfo>

#ifdef linux
	using std::type_info;
	#include <unistd.h>
	#include <sys/time.h>
	#include <sys/times.h>
	#include <pthread.h>
	#include <bits/stringfwd.h>
	#define LPCTSTR const char *
	#define LPCWSTR const wchar_t *
	#define HANDLE void*
	#define DWORD unsigned long
	
	extern long _wtoi(const wchar_t *ptr);
	
#endif // _Linux

#ifdef HT_USE_DLL

	#ifdef _MSC_VER
		#include <windows.h>

		//  stl warning
		#pragma warning(disable:4786)
		#pragma warning(disable:4291)
		#pragma warning(disable: 4275)
		#pragma warning(disable: 4251)

		#ifdef HT_EXPORT_DLL
			#define HT_CLASS_EXPORT	__declspec(dllexport)
			#define HT_FUNC_EXPORT	__declspec(dllexport)
		#else
			#define HT_CLASS_EXPORT	__declspec(dllimport)
			#define HT_FUNC_EXPORT	__declspec(dllimport)
		#endif // HT_EXPORT_DLL
	#else
	// For other platform
		#define HT_CLASS_EXPORT	
		#define HT_FUNC_EXPORT	
	#endif //_MSC_VER*/

#else	// HT_USE_DLL
	#define HT_CLASS_EXPORT
	#define HT_FUNC_EXPORT

#endif // HT_USE_DLL

// ------------------ level of debugging --------------------------
#define HT_DEBUG_NONE  0
#define HT_DEBUG_CHECK 1
#define HT_DEBUG_TRACE 2

#ifndef HT_DEBUG_LEVEL
#ifdef NDEBUG
#define HT_DEBUG_LEVEL HT_DEBUG_NONE
#define _Assert(f)          ((void)(f))
#else
#define HT_DEBUG_LEVEL HT_DEBUG_CHECK
#define _Assert(f)          assert(f)
#endif
#endif

#define HT_NAMESPACE	HT
#define HT_NAMESPACE_BEGIN	namespace HT_NAMESPACE {
#define HT_NAMESPACE_END	};

// ---------- Align value 'x' to boundary 'b' which should be power of 2  ---
#define ALIGN(x,b)   (((x) + (b) - 1) & ~((b) - 1))

//--------------------------  std data defines ------------------------------
typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
#ifndef int32
#define int32 long
#endif
typedef unsigned int   uint32;

#if defined(_WIN32)
typedef unsigned __int64 uint64;
typedef __int64          int64;
#define INT8_FORMAT "I64"
#define CONST64(c)  c
#else
#if defined(__osf__ )
typedef unsigned long uint64;
typedef signed   long int64;
#define INT8_FORMAT "l"
#define CONST64(c)  c##L
#else
#if defined(__GNUC__) || defined(__SUNPRO_CC)
typedef unsigned long long uint64;
typedef signed   long long int64;
#define INT8_FORMAT "ll"
#define CONST64(c)  c##LL
#else
#error "integer 8 byte type is not defined" 
#endif
#endif
#endif
typedef float  float32;
typedef double float64; 

//--------------------------  stl defines  ----------------------------
#ifdef _MSC_VER
#	pragma warning(disable:4786)
#endif

#define HTSTD std
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
using HTSTD::min;
using HTSTD::max;

#endif // __SYS_TYPES_H_AKPIOOPE90GHLSX566WJH
