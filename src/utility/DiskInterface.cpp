/*
 * DiskInterface.cpp
 *
 *  Created on: 2019年12月13日
 *      Author: qpzhou
 */
#include "utility/DiskInterface.h"
#include "DiskInterfaceLinux.h"
#include "DiskInterfaceWin.h"
#include "utility/Macros.h"
#include <string.h>

namespace utility {

DiskInterface::DiskInterface() {}
DiskInterface::~DiskInterface() {}
bool DiskInterface::SkipDir(const char* s) {
	if (*s == '.') {
		if (s[1] == 0)
			return true;
		return ((s[1] == '.') && (s[2] == 0));
	}
	return false;
}

bool DiskInterface::IsPathSep(int c) {
	return ((c == '\\') || (c == '/'));
}

std::string& DiskInterface::JoinPath(std::string& result, const std::string& part) {
	if (!result.empty()) {
		if (!IsPathSep(result.back()))
			result.push_back(PATH_SEPARATOR_CHAR);
	}
	return result.append(part);
}

bool DiskInterface::WriteFile(const std::string& path, const std::string& contents, bool truncate) {
	FILE* fp = fopen(path.c_str(), truncate ? "w" : "a");
	if (!fp)
		return false;
	if (fwrite(contents.data(), 1, contents.length(), fp) < contents.length()) {
		fclose(fp);
		return false;
	}
	if (fclose(fp) == EOF)
		return false;
	return true;
}

bool DiskInterface::WriteFile(const std::string& path, const std::string& contents) {
	return WriteFile(path, contents, true);
}

bool DiskInterface::AppendFile(const std::string& path, const std::string& contents) {
	return WriteFile(path, contents, false);
}

bool DiskInterface::ReadFile(const std::string& path, std::string* contents) {
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp)
		return false;
	char buf[64 << 10];
	size_t len;
	while ((len = fread(buf, 1, sizeof(buf), fp)) > 0) {
		contents->append(buf, len);
	}
	if (ferror(fp)) {
		contents->clear();
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}

bool DiskInterface::IsSameDir(const char* dir1, const char* dir2) {
	size_t l1 = strlen(dir1);
	size_t l2 = strlen(dir2);
	size_t i = 0, h = 0;
	for (; i < l1 && h < l2; i++,h++) {
		while(dir1[i] == PATH_SEPARATOR_CHAR && dir1[i] != '\0') i++;
		while(dir2[h] == PATH_SEPARATOR_CHAR && dir2[h] != '\0') h++;
		if (dir1[i] == '\0')
			break;
		if (dir2[h] == '\0')
			break;
		if (dir1[i] != dir2[h])
			return false;
	}
	if (i != l1)
		while(dir1[i] == PATH_SEPARATOR_CHAR && dir1[i] != '\0') i++;
	if (h != l2)
		while(dir2[h] == PATH_SEPARATOR_CHAR && dir2[h] != '\0') h++;
	if (i == l1 && h == l2)
		return true;
	return false;
}

DiskInterface& DiskInterface::getInstance() {
#ifdef WIN32
	static DiskInterfaceWin diskInstance;
#else
	static DiskInterfaceLinux diskInstance;
#endif
	return diskInstance;
}

} // namespace of utility


