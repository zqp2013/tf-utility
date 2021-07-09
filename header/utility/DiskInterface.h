/*
 * DiskInterface.h
 *
 *  Created on: 2019年12月13日
 *      Author: qpzhou
 */

#ifndef UTILITY_DISKINTERFACE_H_
#define UTILITY_DISKINTERFACE_H_
#include <iostream>
#include <vector>
#include "tf_export.h"

#if defined(_MSC_VER)
#include <Windows.h>
#pragma warning(push)
	// CopyFile声明为静态函数，如果未被使用的话，会警告，未引用的静态函数被删除。
#pragma warning(disable:4505)
#if defined(CopyFile)
	static BOOL _CopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists) {
		return CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
	}
#undef CopyFile
	static BOOL CopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists) {
		return _CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
	}
#endif
#pragma warning(pop)
#endif

namespace utility {
	class TF_EXT_CLASS DiskInterface {
	protected:
		bool WriteFile(const std::string& path, const std::string& contents, bool truncate);
		virtual void CheckIoError() = 0;
		bool SkipDir(const char* s);
	public:
		std::string& JoinPath(std::string& result, const std::string& part);
		bool WriteFile(const std::string& path, const std::string& contents);
		bool AppendFile(const std::string& path, const std::string& contents);
		bool ReadFile(const std::string& path, std::string* contents);
		bool IsSameDir(const char* dir1, const char* dir2);
		bool IsPathSep(int c);
	public:
		virtual bool GetChildrenInDir(const std::string& dir, std::vector<std::string>* result) = 0;
		virtual bool RemoveFile(const std::string& fname) = 0;
		virtual bool CreateDirIfNotExists(const std::string& dir) = 0;
		virtual bool MakeDirIfNotExists(const std::string& dir) = 0;
		virtual int64_t GetFileSize(const std::string& fname) = 0;
		virtual bool FileExists(const std::string& fname) = 0;
		virtual bool DirExists(const std::string& dir) = 0;
		virtual bool MakeDir(const std::string& dir) = 0;
		virtual bool MakeDirRecursive(const std::string& dir) = 0;
		virtual bool RemoveDir(const std::string& dir) = 0;
		virtual bool RemoveDirRecursive(const std::string& dir) = 0;
		virtual bool CopyFile(const std::string& src, const std::string& dst, bool fail_if_exists = false) = 0;
		/**
		 * 可以跨目录移动文件
		 */
		virtual bool RenameFile(const std::string& src, const std::string& target) = 0;
	public:
		DiskInterface();
		virtual ~DiskInterface();
		static DiskInterface& getInstance();
	};
} // namespace of utility


#endif /* UTILITY_DISKINTERFACE_H_ */
