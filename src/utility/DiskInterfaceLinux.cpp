/*
 * DiskInterfaceLinux.cpp
 *
 *  Created on: 2019年12月13日
 *      Author: qpzhou
 */
#if !defined(_WIN32)
#include "DiskInterfaceLinux.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/sendfile.h>

namespace utility {

typedef std::basic_string<char>::size_type S_T;
static const S_T npos = -1;

DiskInterfaceLinux::DiskInterfaceLinux() {}
DiskInterfaceLinux::~DiskInterfaceLinux() {}
void DiskInterfaceLinux::CheckIoError() {}

int DiskInterfaceLinux::do_mkdir(const char* path, mode_t mode) {
    struct stat st;
    int status = 0;
    if (stat(path, &st) != 0) {
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    } else if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        status = -1;
    }
    return (status);
}

int DiskInterfaceLinux::mkpath(const char* path, mode_t mode) {
    return -1;
}

void DiskInterfaceLinux::delete_path(const char* path) {

}

bool DiskInterfaceLinux::GetChildrenInDir(const std::string& dir, std::vector<std::string>* result) {
    return false;
}
bool DiskInterfaceLinux::RemoveFile(const std::string& fname) {
    return false;
}
bool DiskInterfaceLinux::CreateDirIfNotExists(const std::string& dir) {
    return false;
}
bool DiskInterfaceLinux::MakeDirIfNotExists(const std::string& dir) {
    return false;
}
int64_t DiskInterfaceLinux::GetFileSize(const std::string& fname) {
    return -1;
}
bool DiskInterfaceLinux::FileExists(const std::string& fname) {
    return false;
}
bool DiskInterfaceLinux::DirExists(const std::string& dir) {
    return false;
}
bool DiskInterfaceLinux::MakeDir(const std::string& dir) {
    return false;
}
bool DiskInterfaceLinux::MakeDirRecursive(const std::string& dir) {
    return false;
}
bool DiskInterfaceLinux::RemoveDir(const std::string& dir) {
    return false;
}
bool DiskInterfaceLinux::DiskInterfaceLinux::RemoveDirRecursive(const std::string& dir) {
    return false;
}
bool DiskInterfaceLinux::CopyFile(const std::string& src, const std::string& dst, bool fail_if_exists/* = false*/) {
    return false;
}
bool DiskInterfaceLinux::RenameFile(const std::string& src, const std::string& target) {
    return false;
}

} // namespace of utility
#endif