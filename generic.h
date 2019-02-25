// The generic.h contains some useful templates and classes 
// for global use
// 

#ifndef __GENERIC_H__
#define __GENERIC_H__

#include <list>

#define DEF_ANY_SIZE 128
#define STR_RETURN(x) char* pRet = (char*)malloc(strlen(x) + 1);\
                      strcpy(pRet, x);\
                      return pRet;

template <class Tt>
class _baseCollection
{
	typedef Tt* type_pointer;
	
#ifdef _GLIBCPP_CONCEPT_CHECK
	typedef std::_List_iterator<type_pointer, type_pointer&, type_pointer*> type_iterator;
#else // _CONCEPT_CHECK_H
	typedef std::_List_iterator<type_pointer> type_iterator;
#endif
  
	std::list<type_pointer> m_list;
	char m_szError[256];
public:
	_baseCollection() {}
	virtual ~_baseCollection() {
		m_list.clear();
	}
	virtual void Add(type_pointer t) {m_list.push_back(t);}
	type_pointer Remove(unsigned index) {
		unsigned uCount = GetCount();
		if ((uCount == 0 && index == 0) || 
			(index > (unsigned)uCount - 1))
		{	
			sprintf(m_szError, "The specified index %d does not exists.", index);
			return 0;
		}

		type_iterator it = m_list.begin();
		while (index--)
			it++;
		type_pointer t = *it;
		m_list.erase(it);
		return t;
	}
	type_pointer GetItem(unsigned index) {
		unsigned uCount = GetCount();
		if ((uCount == 0 && index == 0) || 
			(index > (unsigned)uCount - 1))
		{	
			sprintf(m_szError, "The specified index %d does not exists.", index);
			return 0;
		}

		type_iterator it = m_list.begin();
		while (index--)
			it++;
		return (type_pointer)(*it);
	}
	type_pointer operator [](unsigned index) {return GetItem(index);}
	unsigned GetCount() {return (unsigned)m_list.size();}
	const char* GetLastError() {return m_szError;}
	void Clear(bool bDelete = false) {
		if (bDelete)
			while(GetCount() > 0)
			{
				type_pointer tp = Remove(0);
				delete tp;
			}
		m_list.clear();
	}
	void operator +=(_baseCollection<Tt>& bc) {
		for (int i = 0; i < bc.GetCount(); i++)
			Add(bc.GetItem(i));
	}
	void operator =(_baseCollection<Tt>& bc) {
		Clear(false);
		operator +=(bc);
	}
};

class ANY
{
	typedef unsigned char u_char;
	typedef unsigned int u_int;
	typedef unsigned short u_short;
	typedef void* binary_pointer;
	typedef char* string_pointer;
  
	binary_pointer p;
	int length;
	bool binary;
public:
	ANY()						: p(0), length(0), binary(false)	{}
	ANY(int val)				: p(0), length(0), binary(false)	{operator=(val);}
	ANY(u_int val)				: p(0), length(0), binary(false)	{operator=(val);}
	ANY(short val)				: p(0), length(0), binary(false)	{operator=(val);}
	ANY(u_short val)			: p(0), length(0), binary(false)	{operator=(val);}
	ANY(char val)				: p(0), length(0), binary(false)	{operator=(val);}
	ANY(u_char val)				: p(0), length(0), binary(false)	{operator=(val);}
	ANY(double val)				: p(0), length(0), binary(false)	{operator=(val);}
	ANY(string_pointer val)		: p(0), length(0), binary(false)	{operator=(val);}
	ANY(const ANY& val)			: p(0), length(0), binary(false)	{operator=(val);}

	~ANY() {clear();}
	
	void clear() {
		if (p)
			free(p), p = 0;
		binary = false;
		length = 0;
	}
	
	void operator=(int val) {
		clear();
		p = malloc(DEF_ANY_SIZE);
		sprintf((string_pointer)p, "%d", val);
	}
	void operator=(u_int val) {
		clear();
		p = malloc(DEF_ANY_SIZE);
		sprintf((string_pointer)p, "%u", val);
	}
	void operator=(short val) {operator=((int)val);}
	void operator=(char val) {operator=((int)val);}
	void operator=(u_short val) {operator=((u_int)val);}
	void operator=(u_char val) {operator=((u_int)val);}
	
	void operator=(double val) {
		clear();
		p = malloc(DEF_ANY_SIZE);
		sprintf((string_pointer)p, "%f", val);
	}

	void operator=(string_pointer val) {
		clear();
		if (val)
		{
			size_t size = strlen(val);
			if (size)
			{
				p = malloc(size + 1);
				strcpy((string_pointer)p, val);
			}
		}
	}
	void operator=(const ANY& any) {
		if (any.p == 0)
			clear();
		else if (!any.binary)
			operator=((string_pointer)any.p);
		else 
			set_real_value(any.p, any.length);
	}

	operator double() {return strtod((string_pointer)p, 0);}
	operator char() {return (char)strtol((string_pointer)p, 0, 10);}
	operator unsigned char() {return (u_char)strtoul((string_pointer)p, 0, 10);}
	operator short() {return (short)strtol((string_pointer)p, 0, 10);}
	operator unsigned short() {return (u_short)strtoul((string_pointer)p, 0, 10);}
	operator int() {return strtol((string_pointer)p, 0, 10);}
	operator unsigned int() {return strtoul((string_pointer)p, 0, 10);}
	operator char*() {/*STR_RETURN((string_pointer)p)*/return (string_pointer)p;}
	operator void*() {return (binary_pointer)p;}

	int get_length() {return length;}
	void set_real_value(binary_pointer val, int len) {
		clear();
		if (val && len > 0)
		{
			binary = true;
			p = malloc(length = len);
			memcpy(p, val, length);
		}
	}
	bool empty() {return p == 0;}
};

#endif // __GENERIC_H__
