/*
 * DiskInterfaceWin.cpp
 *
 *  Created on: 2019年12月13日
 *      Author: qpzhou
 */

#if defined(_WIN32)
#include "DiskInterfaceWin.h"
namespace utility {

typedef std::basic_string<char>::size_type S_T;
static const S_T npos = -1;

DiskInterfaceWin::DiskInterfaceWin() {}
DiskInterfaceWin::~DiskInterfaceWin() {}
void DiskInterfaceWin::CheckIoError() {}

bool DiskInterfaceWin::GetChildrenInDir(const std::string& dir, std::vector<std::string>* result) {
	return false;
}
bool DiskInterfaceWin::RemoveFile(const std::string& fname) {
	return false;
}
bool DiskInterfaceWin::CreateDirIfNotExists(const std::string& dir) {
	return false;
}
bool DiskInterfaceWin::MakeDirIfNotExists(const std::string& dir) {
	return false;
}
int64_t DiskInterfaceWin::GetFileSize(const std::string& fname) {
	return -1;
}
bool DiskInterfaceWin::FileExists(const std::string& fname) {
	return false;
}
bool DiskInterfaceWin::DirExists(const std::string& dir) {
	return false;
}
bool DiskInterfaceWin::MakeDir(const std::string& dir) {
	return false;
}
bool DiskInterfaceWin::MakeDirRecursive(const std::string& dir) {
	return false;
}
bool DiskInterfaceWin::RemoveDir(const std::string& dir) {
	return false;
}
bool DiskInterfaceWin::RemoveDirRecursive(const std::string& dir) {
	return false;
}
bool DiskInterfaceWin::CopyFile(const std::string& src, const std::string& dst, bool fail_if_exists/* = false*/) {
	return false;
}
bool DiskInterfaceWin::RenameFile(const std::string& src, const std::string& target) {
	return false;
}

} // namespace of utility

#endif


