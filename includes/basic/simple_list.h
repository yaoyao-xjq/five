// simple_list.h
#ifndef _SIMPLE_LIST_H__00021EFWEF
#define _SIMPLE_LIST_H__00021EFWEF 1
#include "stdinc.h"

using COM::l2elem;
class simple_list
{
public: 
	simple_list () : n_items(0)
	{	}
	~simple_list () { clear(); }

	void push_back(l2elem* val)
	{ 
		val->link_before(&m_Head);
		n_items ++;
	}

	void push_front(l2elem* val)
	{ 
		val->link_after(&m_Head);
		n_items ++;
	}

	void pop_front(bool bDelete = true)
	{
		if (begin() == end())
			return;
		l2elem* p = begin();
		p->unlink();
		if (bDelete)
			delete p;
		n_items --;
	}

	void pop_back(bool bDelete = true)
	{
		if (begin() == end())
			return;
		l2elem* p = last();
		p->unlink();
		if (bDelete)
			delete p;
		n_items --;
	}

	l2elem* begin()	{ return m_Head.next; }
	l2elem* last()	{ return m_Head.prev; }
	l2elem* end()	{ return &m_Head; }
	void clear(bool bDelete = true)
	{
		for (l2elem* p=begin(); p!=end(); )
		{
			l2elem* next = p->next;
			p->unlink();
			if (bDelete)
				delete p;
			p = next;
		}
		m_Head.prune();
		n_items = 0;
	}

	size_t size() const { return n_items; }
	bool empty() const { return n_items == 0; }

protected:
	l2elem m_Head;
	size_t n_items;
};

#endif	// _SIMPLE_LIST_H__00021EFWEF
