/******************************************************************* 
FileName: macro.h
Author  : vee
Date    : 2015-07-01 00:59:06
Desc    : useful macros for c++ programming
*******************************************************************/ 

#ifndef _VEE_MACRO_H_
#define _VEE_MACRO_H_

#define _VEE_BEGIN	namespace vee {
#define _VEE_END	}
#define _VOOST_BEGIN	namespace voost {
#define _VOOST_END	}
#define _VEE	    ::vee::
#define _VOOST      ::vee::voost::
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&) = delete; \
void operator=(const TypeName&) = delete
#define DISALLOW_MOVE_AND_ASSIGN(TypeName) \
TypeName(const TypeName&&) = delete; \
void operator=(const TypeName&&) = delete
#define VEE_EMPTY_FUNCTION {}
#define _VEE_ENABLE_LOGGING 1
#ifdef _VEE_ENABLE_LOGGING
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( false )
#else
#define DEBUG_PRINT(...) {}
#endif
#ifdef _WIN32
#define VEE_PLATFORM_WINDOWS 1
#define VEE_PLATFORM_X32 1
#elif  _WIN64
#define VEE_PLATFORM_WINDOWS_ 1
#define VEE_PLATFORM_X64 1
#else
#define VEE_PLATFORM_WINDOWS 0
#endif
#define PRINT_LINE for(int i = 0; i < 8; ++i) printf("----------");

#endif // _VEE_MACRO_H_