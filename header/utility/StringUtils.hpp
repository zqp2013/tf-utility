/*
 * StringUtils.h
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */

#ifndef UTILITY_STRINGUTILS_HPP_
#define UTILITY_STRINGUTILS_HPP_
#include <vector>
#include <string>
#include <cctype>
#include <cstdarg>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace utility {

#ifdef _WIN32
static char* WideToUtf8(const wchar_t* wide, intptr_t len, intptr_t* result_len) {
	int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide, (int)len, NULL, (int)0, NULL, NULL);
	char* utf8 = reinterpret_cast<char*>(malloc(utf8_len));
	WideCharToMultiByte(CP_UTF8, 0, wide, (int)len, utf8, utf8_len, NULL, NULL);
	if (result_len != NULL)
		*result_len = utf8_len;
	return utf8;
}
static wchar_t* Utf8ToWide(const char* utf8, intptr_t len, intptr_t* result_len) {
	int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8, (int)len, NULL, 0);
	wchar_t* wide = reinterpret_cast<wchar_t*>(malloc(wide_len * sizeof(wchar_t)));
	MultiByteToWideChar(CP_UTF8, 0, utf8, (int)len, wide, wide_len);
	if (result_len != NULL)
		*result_len = wide_len;
	return wide;
}
//ConsoleStringToUtf8
//Utf8ToConsoleString
#endif

template <class T>
std::vector<std::string> Split(const std::string& str, T separator) {
	std::vector<std::string> strvec;
	std::string::size_type pos1, pos2;
	pos2 = str.find(separator);
	pos1 = 0;

	while (std::string::npos != pos2) {
		strvec.emplace_back(str.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = str.find(separator, pos1);
	}
	if (pos1 < str.length()) {
		strvec.emplace_back(str.substr(pos1));
	}
	return strvec;
}

// 注意不会自动null-terminated
inline size_t StringAppendFormatV(std::string& buf, const char* fmt, va_list vp) {
	va_list varg;
	va_copy(varg, vp);
#ifdef WIN32
	size_t sz = _vsnprintf(nullptr, 0, fmt, varg);
#else
	size_t sz = vsnprintf(nullptr, 0, fmt, varg);
#endif
	va_end(varg);
	if (sz > 0) {
		sz++;
		size_t old_size = buf.size(); 
		buf.resize(old_size + sz);
		va_copy(varg, vp);
#ifdef WIN32
		_vsnprintf(const_cast<char*>(buf.data()) + old_size, sz, fmt, varg);
#else
		vsnprintf(const_cast<char*>(buf.data()) + old_size, sz, fmt, varg);
#endif
		va_end(varg);
		buf.pop_back(); // 把最后的那个0给干掉。
	}
	return sz;
}
inline std::string StringFormat(const char* fmt, ...) {
	std::string buf;
	va_list varg;
	va_start(varg, fmt);
	StringAppendFormatV(buf, fmt, varg);
	va_end(varg);
	return buf;
}

} // end of namespace utility
#endif /* UTILITY_STRINGUTILS_HPP_ */
