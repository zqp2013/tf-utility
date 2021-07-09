/*
 * macro.h
 *
 *  Created on: 2019年12月2日
 *      Author: qpzhou
 */

#ifndef UTILITY_MACRO_H_
#define UTILITY_MACRO_H_
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <memory>

# if __WORDSIZE == 64 //#if defined(_AMD64_) || defined(_WIN64)
/* 64位系统 */
//#define PRIu PRIu64
#else
/* 32位系统 */
//#define PRIu PRIu32
#endif


// make_unique is not part of c++11 but c++14
//http://coliru.stacked-crooked.com/a/cb9baad37307f5cd
#if defined(_WIN32) || __cplusplus == 201402L // C++14

using std::make_unique;

#else // C++11

template < typename T, typename... CONSTRUCTOR_ARGS >
std::unique_ptr<T> make_unique(CONSTRUCTOR_ARGS&&... constructor_args) {
	return std::unique_ptr<T>(new T(std::forward<CONSTRUCTOR_ARGS>(constructor_args)...));
}
#endif // __cplusplus == 201402L


#if !defined(UNUSED)
#define UNUSED(var) (void)var;
#endif //!defined(UNUSED)

#if !defined(UNREACHABLE)
#define UNREACHABLE() assert(!"UNREACHABLE CODE");
#endif //!defined(UNREACHABLE)

#if !defined(DCHECK)
#define DCHECK(cond)
#endif //!defined(DCHECK)

#if !defined(LOG)
#define LOG(level) std::cout
#endif //!defined(LOG)

#if defined(max)
#undef max
#endif
#if defined(min)
#undef min
#endif

#ifdef _WIN32
#define stricmp(x,y) _stricmp(x,y)
#define strnicmp(x,y,count) _strnicmp(x,y,count)
#undef strdup
#define strdup(s) _strdup(s)
#define strlwr(s) _strlwr(s)

#if !defined(MSEC_SLEEP)
#define MSEC_SLEEP(msec) Sleep(msec)
#endif //!defined(MSEC_SLEEP)

#define PATH_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR_STR "\\"

#else
#include <unistd.h>
#define stricmp(x,y) strcasecmp(x,y)
#define strnicmp(x,y,count) strncasecmp(x,y,count)

#if !defined(MSEC_SLEEP)
#define MSEC_SLEEP(msec) usleep(1000 * msec)
#endif //!defined(MSEC_SLEEP)

#define PATH_SEPARATOR_CHAR '/'
#define PATH_SEPARATOR_STR "/"
#endif

// 禁止拷贝构造和赋值操作
#if !defined(DISALLOW_COPY_AND_ASSIGN)
#define DISALLOW_COPY_AND_ASSIGN(TypeName)			\
private:											\
	TypeName(const TypeName&) = delete;				\
	void operator=(const TypeName&) = delete;
#endif //!defined(DISALLOW_COPY_AND_ASSIGN)


// 禁止动态创建只允许在栈上创建
#if !defined(DISALLOW_ALLOCATION)
#define DISALLOW_ALLOCATION()						\
public:												\
	void operator delete(void* p) {					\
		UNUSED(p);									\
		UNREACHABLE();								\
	}												\
private:											\
	void* operator new(size_t sz);
#endif //!defined(DISALLOW_ALLOCATION)


// 禁用构造函数
#if !defined(DISALLOW_IMPLICIT_CONSTRUCTORS)
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName)	\
private:											\
	TypeName();										\
	DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif //!defined(DISALLOW_IMPLICIT_CONSTRUCTORS)

// 要求所有成员都为静态成员，不允许创建实例
class AllStatic {
private:
	DISALLOW_ALLOCATION();
	DISALLOW_IMPLICIT_CONSTRUCTORS(AllStatic);
};



#endif /* UTILITY_MACRO_H_ */
