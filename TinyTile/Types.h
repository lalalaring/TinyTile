#ifndef _TYPES_H
#define _TYPES_H

#include <string>

//////////////////////////////////////////////////////////////////////////
// Type definition
typedef double          f64;
typedef __int64         s64;
typedef float           f32;
typedef unsigned        u32;
typedef int             s32;
typedef unsigned short  u16;
typedef short           s16;
typedef unsigned char   u8;

enum ValueType : u32
{
	VT_UNKNOWN,
	VT_NUMBER,
	VT_STRING,
};

struct Value
{
	ValueType     type;
	union number
	{
		s64     i;
		f64		f;

	}n;

	std::string   s;
};

#endif  // _TYPES_H
