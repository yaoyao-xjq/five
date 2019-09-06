#ifndef BFX_TIME_H_DEF
#define BFX_TIME_H_DEF 1

#include <time.h>
#include "object.h"

HT_NAMESPACE_BEGIN

//this is a class based on second,that can handle time operater

/**
 *	ʱ
 */
class HT_CLASS_EXPORT TimeSpan
{
public:
	
	// Constructors
	TimeSpan();
	TimeSpan(time_t time);
	TimeSpan(long lDays, int nHours, int nMins, int nSecs);
	
	TimeSpan(const TimeSpan& timeSpanSrc);
	const TimeSpan& operator=(const TimeSpan& timeSpanSrc);
	
	// Attributes
	// extract parts
	long GetDays() const;   // total # of days
	long GetTotalHours() const;
	int GetHours() const;
	long GetTotalMinutes() const;
	int GetMinutes() const;
	long GetTotalSeconds() const;
	int GetSeconds() const;
	
	// Operations
	// time math
	TimeSpan operator-(TimeSpan timeSpan) const;
	TimeSpan operator+(TimeSpan timeSpan) const;
	const TimeSpan& operator+=(TimeSpan timeSpan);
	const TimeSpan& operator-=(TimeSpan timeSpan);
	bool operator==(TimeSpan timeSpan) const;
	bool operator!=(TimeSpan timeSpan) const;
	bool operator<(TimeSpan timeSpan) const;
	bool operator>(TimeSpan timeSpan) const;
	bool operator<=(TimeSpan timeSpan) const;
	bool operator>=(TimeSpan timeSpan) const;
	
protected:
	time_t m_timeSpan;
	friend class Time;
};

/**
 *	ʱ䣬ΪʱUTCʱ
 */
class HT_CLASS_EXPORT Time
{
public: // CONSTRUCTS
	// ʼʱΪ
	Time();
	// UTCʱ
	Time(time_t tm);	
	// 䱾�?
	Time(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);

public:	// STATIC FUNCTIONS
	static Time GetCurrentTime();
	
public:	// FOR GET VALUE
	bool SetUTCTime (int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);
	void GetUTCTime (int &nYear, int &nMonth, int &nDay, int &nHour, int &nMin, int &nSec) const;
	void GetLocalTm(struct tm* lc) const;
	void GetGmTm (struct tm* gm) const;
	StdString GetDisplayText() const;
	StdString FormatUTC() const;
	
	time_t	GetTime() const;		//gain time_t value
	
	int GetUTCYear() const;
	int GetUTCMonth() const;
	int GetUTCDay() const;
	int GetUTCHour() const;
	int GetUTCMinute() const;
	int GetUTCSecond() const;
	int GetUTCDayOfWeek() const;
	int GetUTCDayOfMonth() const;
	int GetUTCDayOfYear() const;
	
	int	GetYear() const;		//gain year
	int	GetMonth() const;		//gain month
	int	GetDay() const;			//gain day
	int	GetHour() const;		//gain hour
	int	GetMinute() const;		//gain minute
	int	GetSecond() const;		//gain second
	int	GetDayOfWeek() const;	//gain the current day of week(from 1 to 7)
	int	GetDayOfMonth() const;	//gain the last days of current month
	int	GetDayOfYear() const;	//gain the last days of current year(from 1.1)

public:	// FOR OPERATOR
	TimeSpan operator-(Time time) const;
	Time operator-(TimeSpan timeSpan) const;
	Time operator+(TimeSpan timeSpan) const;
	const Time& operator+=(TimeSpan timeSpan);
	const Time& operator-=(TimeSpan timeSpan);
	bool operator==(Time time) const;
	bool operator!=(Time time) const;
	bool operator<(Time time) const;
	bool operator>(Time time) const;
	bool operator<=(Time time) const;
	bool operator>=(Time time) const;
	
protected:
	time_t m_time;
	static Mutex m_mutex;
};

/************************************************************************/
/* TimeSpan Inline */
/************************************************************************/
inline TimeSpan::TimeSpan()
{ }
inline TimeSpan::TimeSpan(time_t time)
{ m_timeSpan = time; }
inline TimeSpan::TimeSpan(long lDays, int nHours, int nMins, int nSecs)
{ m_timeSpan = nSecs + 60* (nMins + 60* (nHours + 24* lDays)); }
inline TimeSpan::TimeSpan(const TimeSpan& timeSpanSrc)
{ m_timeSpan = timeSpanSrc.m_timeSpan; }
inline const TimeSpan& TimeSpan::operator=(const TimeSpan& timeSpanSrc)
{ m_timeSpan = timeSpanSrc.m_timeSpan; return *this; }
inline long TimeSpan::GetDays() const
{ return m_timeSpan / (24*3600L); }
inline long TimeSpan::GetTotalHours() const
{ return m_timeSpan/3600; }
inline int TimeSpan::GetHours() const
{ return (int)(GetTotalHours() - GetDays()*24); }
inline long TimeSpan::GetTotalMinutes() const
{ return m_timeSpan/60; }
inline int TimeSpan::GetMinutes() const
{ return (int)(GetTotalMinutes() - GetTotalHours()*60); }
inline long TimeSpan::GetTotalSeconds() const
{ return m_timeSpan; }
inline int TimeSpan::GetSeconds() const
{ return (int)(GetTotalSeconds() - GetTotalMinutes()*60); }
inline TimeSpan TimeSpan::operator-(TimeSpan timeSpan) const
{ return TimeSpan(m_timeSpan - timeSpan.m_timeSpan); }
inline TimeSpan TimeSpan::operator+(TimeSpan timeSpan) const
{ return TimeSpan(m_timeSpan + timeSpan.m_timeSpan); }
inline const TimeSpan& TimeSpan::operator+=(TimeSpan timeSpan)
{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
inline const TimeSpan& TimeSpan::operator-=(TimeSpan timeSpan)
{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
inline bool TimeSpan::operator==(TimeSpan timeSpan) const
{ return m_timeSpan == timeSpan.m_timeSpan; }
inline bool TimeSpan::operator!=(TimeSpan timeSpan) const
{ return m_timeSpan != timeSpan.m_timeSpan; }
inline bool TimeSpan::operator<(TimeSpan timeSpan) const
{ return m_timeSpan < timeSpan.m_timeSpan; }
inline bool TimeSpan::operator>(TimeSpan timeSpan) const
{ return m_timeSpan > timeSpan.m_timeSpan; }
inline bool TimeSpan::operator<=(TimeSpan timeSpan) const
{ return m_timeSpan <= timeSpan.m_timeSpan; }
inline bool TimeSpan::operator>=(TimeSpan timeSpan) const
{ return m_timeSpan >= timeSpan.m_timeSpan; }

/************************************************************************/
/* Time Inline */
/************************************************************************/
inline time_t Time::GetTime() const{ return m_time; }
// LOCALTIME
inline int Time::GetYear() const
{ HT_CS(m_mutex);	return (localtime(&m_time)->tm_year) + 1900; }
inline int Time::GetMonth() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_mon + 1; }
inline int Time::GetDay() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_mday; }
inline int Time::GetHour() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_hour; }
inline int Time::GetMinute() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_min; }
inline int Time::GetSecond() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_sec; }
inline int Time::GetDayOfWeek() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_wday + 1; }
inline int Time::GetDayOfMonth() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_mday;	}
inline int Time::GetDayOfYear() const
{ HT_CS(m_mutex); return localtime(&m_time)->tm_yday + 1; }
// UTC
inline int Time::GetUTCYear() const
{ HT_CS(m_mutex);	return (gmtime(&m_time)->tm_year) + 1900; }
inline int Time::GetUTCMonth() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_mon + 1; }
inline int Time::GetUTCDay() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_mday; }
inline int Time::GetUTCHour() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_hour; }
inline int Time::GetUTCMinute() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_min; }
inline int Time::GetUTCSecond() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_sec; }
inline int Time::GetUTCDayOfWeek() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_wday + 1; }
inline int Time::GetUTCDayOfMonth() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_mday;	}
inline int Time::GetUTCDayOfYear() const
{ HT_CS(m_mutex); return gmtime(&m_time)->tm_yday + 1; }
// With TimeSpan
inline TimeSpan Time::operator-(Time time) const
{ return TimeSpan(m_time - time.m_time); }
inline Time Time::operator-(TimeSpan timeSpan) const
{ return Time(m_time - timeSpan.m_timeSpan); }
inline Time Time::operator+(TimeSpan timeSpan) const
{ return Time(m_time + timeSpan.m_timeSpan); }
inline const Time& Time::operator+=(TimeSpan timeSpan)
{ m_time += timeSpan.m_timeSpan; return *this; }
inline const Time& Time::operator-=(TimeSpan timeSpan)
{ m_time -= timeSpan.m_timeSpan; return *this; }
inline bool Time::operator==(Time time) const
{ return m_time == time.m_time; }
inline bool Time::operator!=(Time time) const
{ return m_time != time.m_time; }
inline bool Time::operator<(Time time) const
{ return m_time < time.m_time; }
inline bool Time::operator>(Time time) const
{ return m_time > time.m_time; }
inline bool Time::operator<=(Time time) const
{ return m_time <= time.m_time; }
inline bool Time::operator>=(Time time) const
{ return m_time >= time.m_time; }

HT_NAMESPACE_END

#endif	// BFX_TIME_H_DEF
