#ifndef MILLI_SECOND_TIME_H_DEF
#define MILLI_SECOND_TIME_H_DEF 1

#include "base_time.h"

HT_NAMESPACE_BEGIN

//this is a class based on second,that can handle time operater

/**
 *	时
 */
class HT_CLASS_EXPORT MilliTimeSpan : public TimeSpan
{
public:
	
	// Constructors
	MilliTimeSpan();
	MilliTimeSpan(time_t time, int nMins);
	MilliTimeSpan(long lDays, int nHours, int nMins, int nSecs, int nMills);
	
	MilliTimeSpan(const MilliTimeSpan& timeSpanSrc);
	const MilliTimeSpan& operator=(const MilliTimeSpan& timeSpanSrc);
	
	short GetMillisecond () const { return m_nMilli; }
	
	// Operations
	// milli time math
	MilliTimeSpan operator-(const MilliTimeSpan& timeSpan) const;
	MilliTimeSpan operator+(const MilliTimeSpan& timeSpan) const;
	const MilliTimeSpan& operator+=(const MilliTimeSpan& timeSpan);
	const MilliTimeSpan& operator-=(const MilliTimeSpan& timeSpan);
	bool operator==(const MilliTimeSpan& timeSpan) const;
	bool operator!=(const MilliTimeSpan& timeSpan) const;
	bool operator<(const MilliTimeSpan& timeSpan) const;
	bool operator>(const MilliTimeSpan& timeSpan) const;
	bool operator<=(const MilliTimeSpan& timeSpan) const;
	bool operator>=(const MilliTimeSpan& timeSpan) const;
	
protected:
	short m_nMilli;	// 侄
	friend class MilliTime;
};

/**
 *	时洌盪TC时
 */
class HT_CLASS_EXPORT MilliTime : public Time
{
public: // CONSTRUCTS
	// 始时为
	MilliTime();
	// UTC时
	MilliTime(time_t tm, int nMills);
	// 址?时
	MilliTime(Time& t) : Time(t), m_nMilli(0)	{}
	MilliTime(const char * str)	{	SetLocalByStringA(str);	}
	MilliTime(const wchar_t* str)	{	SetLocalByStringW(str);	}

	// 浔臼?
	MilliTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMilli);
	MilliTime (const MilliTime& right);

public:	// FOR GET VALUE
	void SetMillisecond(int nMilli) {	m_nMilli = nMilli;	}
	short GetMillisecond () const { return m_nMilli; }
	void SetUTCByStringA (const char * str);
	void SetUTCByStringW (const wchar_t* str);
	void SetLocalByStringA (const char * str);
	void SetLocalByStringW (const wchar_t* str);
	std::string ToStringA ();
	std::wstring ToStringW ();
	static bool ParseStrW (const wchar_t* str, int &nYear, int &nMonth, int &nDay, int &nHour, int &nMin, int &nSec, int &nMilli);
	static bool ParseStrA (const char * str, int &nYear, int &nMonth, int &nDay, int &nHour, int &nMin, int &nSec, int &nMilli);

public:	// FOR OPERATOR
	MilliTimeSpan operator-(const MilliTime& time) const;
	MilliTime operator-(const MilliTimeSpan& timeSpan) const;
	MilliTime operator+(const MilliTimeSpan& timeSpan) const;
	const MilliTime& operator+=(const MilliTimeSpan& timeSpan);
	const MilliTime& operator-=(const MilliTimeSpan& timeSpan);
	bool operator==(const MilliTime& time) const;
	bool operator!=(const MilliTime& time) const;
	bool operator<(const MilliTime& time) const;
	bool operator>(const MilliTime& time) const;
	bool operator<=(const MilliTime& time) const;
	bool operator>=(const MilliTime& time) const;
	
protected:
	short m_nMilli;	// 侄
};

/************************************************************************/
/* MilliTimeSpan Inline  */
/************************************************************************/
inline MilliTimeSpan::MilliTimeSpan() : m_nMilli(0)
{ }

inline MilliTimeSpan::MilliTimeSpan(time_t time, int nMilli) : TimeSpan(time)
{
	assert(nMilli >= 0 && nMilli < 1000);
	m_nMilli = nMilli;
}

inline MilliTimeSpan::MilliTimeSpan(long lDays, int nHours, int nMins, int nSecs, int nMilli)
: TimeSpan (lDays, nHours, nMins, nSecs)
{
	assert(nMilli >= 0 && nMilli < 1000);
	m_nMilli = nMilli;
}

inline MilliTimeSpan::MilliTimeSpan(const MilliTimeSpan& timeSpanSrc)
: TimeSpan (timeSpanSrc), m_nMilli(timeSpanSrc.m_nMilli)
{	}

inline const MilliTimeSpan& MilliTimeSpan::operator=(const MilliTimeSpan& timeSpanSrc)
{ m_timeSpan = timeSpanSrc.m_timeSpan, m_nMilli = timeSpanSrc.m_nMilli; return *this; }

inline MilliTimeSpan MilliTimeSpan::operator-(const MilliTimeSpan& timeSpan) const
{	if (m_nMilli < timeSpan.m_nMilli)
		return MilliTimeSpan(m_timeSpan - timeSpan.m_timeSpan - 1, m_nMilli - timeSpan.m_nMilli + 1000);
	else
		return MilliTimeSpan(m_timeSpan - timeSpan.m_timeSpan, m_nMilli - timeSpan.m_nMilli);	}

inline const MilliTimeSpan& MilliTimeSpan::operator-=(const MilliTimeSpan& timeSpan)
{	if (m_nMilli < timeSpan.m_nMilli)
		m_timeSpan -= (timeSpan.m_timeSpan + 1), m_nMilli -= (timeSpan.m_nMilli - 1000);
	else
		m_timeSpan -= timeSpan.m_timeSpan, m_nMilli -= timeSpan.m_nMilli;
	return *this; }

inline MilliTimeSpan MilliTimeSpan::operator+(const MilliTimeSpan& timeSpan) const
{	int nMilli = m_nMilli + timeSpan.m_nMilli;
	if (nMilli>=1000)
		return MilliTimeSpan(m_timeSpan + timeSpan.m_timeSpan + 1, nMilli - 1000);
	else
		return MilliTimeSpan(m_timeSpan + timeSpan.m_timeSpan, nMilli); }

inline const MilliTimeSpan& MilliTimeSpan::operator+=(const MilliTimeSpan& timeSpan)
{	int nMilli = m_nMilli + timeSpan.m_nMilli;
	if (nMilli>=1000)
		m_timeSpan += (timeSpan.m_timeSpan + 1), m_nMilli = (nMilli - 1000);
	else
		m_timeSpan += timeSpan.m_timeSpan, m_nMilli = nMilli;
	return *this; }

inline bool MilliTimeSpan::operator==(const MilliTimeSpan& timeSpan) const
{ return m_timeSpan == timeSpan.m_timeSpan && m_nMilli == timeSpan.m_nMilli; }

inline bool MilliTimeSpan::operator!=(const MilliTimeSpan& timeSpan) const
{ return m_timeSpan != timeSpan.m_timeSpan || m_nMilli != timeSpan.m_nMilli; }

inline bool MilliTimeSpan::operator<(const MilliTimeSpan& timeSpan) const
{	return (m_timeSpan == timeSpan.m_timeSpan)	? (m_nMilli < timeSpan.m_nMilli) : (m_timeSpan < timeSpan.m_timeSpan); }

inline bool MilliTimeSpan::operator>(const MilliTimeSpan& timeSpan) const
{	return (m_timeSpan == timeSpan.m_timeSpan)	? (m_nMilli > timeSpan.m_nMilli) : (m_timeSpan > timeSpan.m_timeSpan); }

inline bool MilliTimeSpan::operator<=(const MilliTimeSpan& timeSpan) const
{	return (m_timeSpan == timeSpan.m_timeSpan)	? (m_nMilli <= timeSpan.m_nMilli) : (m_timeSpan <= timeSpan.m_timeSpan); }

inline bool MilliTimeSpan::operator>=(const MilliTimeSpan& timeSpan) const
{	return (m_timeSpan == timeSpan.m_timeSpan)	? (m_nMilli >= timeSpan.m_nMilli) : (m_timeSpan >= timeSpan.m_timeSpan); }

/************************************************************************/
/* MilliTime Inline   */
/************************************************************************/
inline MilliTime::MilliTime()
{ 
#ifdef _WIN32
    SYSTEMTIME   st; 
    GetLocalTime(&st);
    m_nMilli = st.wMilliseconds;
#else
    struct timeval nowtimeval;
    gettimeofday(&nowtimeval,0);
    m_nMilli = nowtimeval.tv_usec/1000;
    m_time = nowtimeval.tv_sec;
#endif
}

// UTC时
inline MilliTime::MilliTime(time_t tm, int nMills) : Time(tm), m_nMilli(nMills)
{
	assert(m_nMilli >= 0 && m_nMilli < 1000);
}

// 浔臼?
inline MilliTime::MilliTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMilli)
: Time (nYear, nMonth, nDay, nHour, nMin, nSec)
{
	assert(nMilli >= 0 && nMilli < 1000);
	m_nMilli = nMilli;
}

inline MilliTime::MilliTime (const MilliTime& right) : Time (right), m_nMilli(right.m_nMilli)
{ }


// With MilliTimeSpan
inline MilliTimeSpan MilliTime::operator-(const MilliTime& right) const
{	if (m_nMilli < right.m_nMilli)
		return MilliTimeSpan(m_time - right.m_time - 1, m_nMilli - right.m_nMilli + 1000);
	else
		return MilliTimeSpan(m_time - right.m_time, m_nMilli - right.m_nMilli);	}

inline MilliTime MilliTime::operator-(const MilliTimeSpan& right) const
{	if (m_nMilli < right.m_nMilli)
		return MilliTime(m_time - right.m_timeSpan - 1, m_nMilli - right.m_nMilli + 1000);
	else
		return MilliTime(m_time - right.m_timeSpan, m_nMilli - right.m_nMilli);	}

inline const MilliTime& MilliTime::operator-=(const MilliTimeSpan& timeSpan)
{	if (m_nMilli < timeSpan.m_nMilli)
		m_time -= timeSpan.m_timeSpan - 1, m_nMilli -= (timeSpan.m_nMilli - 1000);
	else
		m_time -= timeSpan.m_timeSpan, m_nMilli -= timeSpan.m_nMilli;
	return *this; }

inline MilliTime MilliTime::operator+(const MilliTimeSpan& timeSpan) const
{	if ((m_nMilli + timeSpan.m_nMilli)>=1000)
		return MilliTime(m_time + timeSpan.m_timeSpan + 1, m_nMilli + timeSpan.m_nMilli - 1000);
	else
		return MilliTime(m_time + timeSpan.m_timeSpan, m_nMilli + timeSpan.m_nMilli);}

inline const MilliTime& MilliTime::operator+=(const MilliTimeSpan &timeSpan)
{	if ((m_nMilli + timeSpan.m_nMilli)>=1000)
		m_time += (timeSpan.m_timeSpan + 1), m_nMilli += (timeSpan.m_nMilli - 1000);
	else
		m_time += timeSpan.m_timeSpan, m_nMilli += timeSpan.m_nMilli;
	return *this; }

inline bool MilliTime::operator==(const MilliTime& time) const
{ return (m_time == time.m_time) && (m_nMilli == time.m_nMilli); }
inline bool MilliTime::operator!=(const MilliTime& time) const
{ return (m_time != time.m_time) || (m_nMilli != time.m_nMilli); }
inline bool MilliTime::operator<(const MilliTime& time) const
{ return (m_time == time.m_time) ? (m_nMilli < time.m_nMilli) : (m_time < time.m_time); }
inline bool MilliTime::operator>(const MilliTime& time) const
{ return (m_time == time.m_time) ? (m_nMilli > time.m_nMilli) : (m_time > time.m_time); }
inline bool MilliTime::operator<=(const MilliTime& time) const
{ return (m_time == time.m_time) ? (m_nMilli <= time.m_nMilli) : (m_time <= time.m_time); }
inline bool MilliTime::operator>=(const MilliTime& time) const
{ return (m_time == time.m_time) ? (m_nMilli >= time.m_nMilli) : (m_time >= time.m_time); }

HT_NAMESPACE_END

#endif	// MILLI_SECOND_TIME_H_DEF
