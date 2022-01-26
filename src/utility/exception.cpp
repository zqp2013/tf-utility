/*
 * exception.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: qpzhou
 *      from: https://github.com/TarsCloud/TarsCpp/tree/master/util
 */

#include "utility/exception.h"
#include "utility/platform.h"
#if TARGET_PLATFORM_LINUX
#include <execinfo.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <cerrno>
#include <iostream>

namespace tf
{

TC_Exception::TC_Exception(const string &buffer)
: _code(0), _buffer(buffer)
{
}


TC_Exception::TC_Exception(const string &buffer, int err)
{
	_buffer = buffer + " :" + parseError(err);
    _code   = err;
}

TC_Exception::~TC_Exception() throw()
{
}

const char* TC_Exception::what() const throw()
{
    return _buffer.c_str();
}

void TC_Exception::getBacktrace()
{
#if TARGET_PLATFORM_LINUX
    void * array[64];
    int nSize = backtrace(array, 64);
    char ** symbols = backtrace_symbols(array, nSize);

    for (int i = 0; i < nSize; i++)
    {
        _buffer += symbols[i];
        _buffer += "\n";
    }
	free(symbols);
#endif
}

#if TARGET_PLATFORM_WINDOWS
static std::string Unicode2ANSI(LPCWSTR lpszSrc)
{
    std::string sResult;
    if (lpszSrc != NULL) {
        int  nANSILen = WideCharToMultiByte(CP_ACP, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
        char* pANSI = new char[nANSILen + 1];
        if (pANSI != NULL) {
            ZeroMemory(pANSI, nANSILen + 1);
            WideCharToMultiByte(CP_ACP, 0, lpszSrc, -1, pANSI, nANSILen, NULL, NULL);
            sResult = pANSI;
            delete[] pANSI;
        }
    }
    return sResult;
}
#endif

string TC_Exception::parseError(int err)
{
    string errMsg;

#if TARGET_PLATFORM_LINUX || TARGET_PLATFORM_IOS
    errMsg = strerror(err);
#else
    // LPTSTR lpMsgBuf;
    LPSTR lpMsgBuf;

    FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            (LPTSTR) & lpMsgBuf, 0, NULL);

    // errMsg = Unicode2ANSI((LPCWSTR)lpMsgBuf);
    if(lpMsgBuf != NULL)
    {
        errMsg = lpMsgBuf;
    }
    LocalFree(lpMsgBuf);
#endif

    return errMsg;
}

int TC_Exception::getSystemCode()
{
#if TARGET_PLATFORM_WINDOWS
    int ret = GetLastError();
    // cout << "getSystemCode:" << ret << endl;

    return ret;
#else
    return errno;
#endif
}

}

