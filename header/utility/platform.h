/*
 * platform.h
 *
 *  Created on: Jul 30, 2021
 *      Author: qpzhou
 *      from: https://github.com/TarsCloud/TarsCpp/tree/master/util
 */

#ifndef SRC_UTILITY_PLATFORM_H_
#define SRC_UTILITY_PLATFORM_H_
#include "tf_export.h"
#include <iostream>

#if defined _WIN32 || defined _WIN64
#define TARGET_PLATFORM_WINDOWS		1

#elif __APPLE__
#define TARGET_PLATFORM_IOS			1

#elif defined ANDROID
#define TARGET_PLATFORM_ANDROID		1
#define TARGET_PLATFORM_LINUX		1

#elif __linux__
#define TARGET_PLATFORM_LINUX		1

#else
#error Unsupported platform.

#endif


#if TARGET_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>

#elif TARGET_PLATFORM_LINUX
#include <unistd.h>
#endif


//UTIL 动态库编译的导入和导出
#if TARGET_PLATFORM_WINDOWS

#ifdef UTIL_DLL_EXPORT
#define UTIL_DLL_API __declspec(dllexport)
#else

#ifdef UTIL_USE_DLL
#define UTIL_DLL_API __declspec(dllimport)
#else
#define UTIL_DLL_API
#endif

#endif

#else
#define UTIL_DLL_API
#endif

//servant 动态库编译的导入和导出
#if TARGET_PLATFORM_WINDOWS

#ifdef SVT_DLL_EXPORT
#define SVT_DLL_API __declspec(dllexport)
#else

#ifdef SVT_USE_DLL
#define SVT_DLL_API __declspec(dllimport)
#else
#define SVT_DLL_API
#endif

#endif

#else
#define SVT_DLL_API
#endif

class TF_EXT_CLASS OS {
public:
	static void SetCurrentThreadName(const std::string& name);
};

#endif /* SRC_UTILITY_PLATFORM_H_ */
