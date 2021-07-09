/*
 * DiskInterfaceLinux.h
 *
 *  Created on: 2019年12月13日
 *      Author: qpzhou
 */
#if !defined(_WIN32)
#ifndef UTILITY_DISKINTERFACELINUX_H_
#define UTILITY_DISKINTERFACELINUX_H_
#include "DiskInterface.h"

namespace utility {
    class DiskInterfaceLinux : public DiskInterface {
    private:
        int do_mkdir(const char* path, mode_t mode);
        int mkpath(const char* path, mode_t mode);
        void delete_path(const char* path);
    protected:
        virtual void CheckIoError() override;
    public:
        DiskInterfaceLinux();
        virtual ~DiskInterfaceLinux();

        virtual bool GetChildrenInDir(const std::string& dir, std::vector<std::string>* result) override;
		virtual bool RemoveFile(const std::string& fname) override;
		virtual bool CreateDirIfNotExists(const std::string& dir) override;
		virtual bool MakeDirIfNotExists(const std::string& dir) override;
		virtual int64_t GetFileSize(const std::string& fname) override;
		virtual bool FileExists(const std::string& fname) override;
		virtual bool DirExists(const std::string& dir) override;
		virtual bool MakeDir(const std::string& dir) override;
		virtual bool MakeDirRecursive(const std::string& dir) override;
		virtual bool RemoveDir(const std::string& dir) override;
		virtual bool RemoveDirRecursive(const std::string& dir) override;
		virtual bool CopyFile(const std::string& src, const std::string& dst, bool fail_if_exists = false) override;
		virtual bool RenameFile(const std::string& src, const std::string& target) override;
    };
} // namespace of utility


#endif /* UTILITY_DISKINTERFACELINUX_H_ */
#endif