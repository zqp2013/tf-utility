/*
 * Flags.h
 *
 *  Created on: 2019年12月11日
 *      Author: qpzhou
 */

#ifndef UTILITY_FLAGS_H_
#define UTILITY_FLAGS_H_

#include <iostream>
#include <memory>
#include "Macros.h"

typedef const char* charp;

#define DECLARE_FLAG(type, name) extern type FLAG_##name
#define DEFINE_FLAG(type, name, default_value, comment) \
	type FLAG_##name = utility::Flags::Register_##type(&FLAG_##name, #name, default_value, comment)
#define DEFINE_FLAG_HANDLER(handler, name, comment) \
	bool DUMMY_##name = utility::Flags::Register_func(handler, #name, comment)

namespace utility {
typedef void (*FlagHandler)(bool value);

class Flag;
class Flags : public AllStatic {
public:
	enum FlagType {
		kUnknow,
		kBoolean,
		kInteger,
		kUint64,
		kString,
		kFunc,
		kNumFlagTypes
	};

	class Iterator {
	public:
		~Iterator();
		void BeforeFirst();
		bool Next();
		FlagType Type() const;
		const char* Name() const;
		const char* Comment() const;
		bool Repr(std::stringstream& out) const;
		int* AsInt() const;
		uint64_t* AsUint64() const;
		const char* AsString() const;
		bool* AsBoolean() const;
	private:
		friend class Flags;
		Iterator();
		Flag** flag_;
	};

	static bool Register_bool(bool* addr, const char* name, bool default_value, const char* comment);
	static int Register_int(int* addr, const char* name, int default_value, const char* comment);
	static uint64_t Register_uint64_t(uint64_t* addr, const char* name, uint64_t default_value, const char* comment);
	static const char* Register_charp(charp* addr, const char* name, charp default_value, const char* comment);
	static bool Register_func(FlagHandler handler, const char* name, const char* comment);

	static bool ProcessCommandLineFlags(int argc, const char** argv);
	static bool ProcessCommandLineFlags();
	static bool LoadFromFile(const char* file);
	static bool SaveToFile(const char* file);

	static Flag* Lookup(const char* name);
	static bool IsSet(const char* name);
	static bool Initialized();
	static bool SetFlag(const char* name, const char* value, const char** error);
	static void PrintFlags(std::ostream& out);
	static void Shutdown();

	static std::unique_ptr<Iterator> NewIterator();
	//一行一个。#注释行
	static bool LoadFrom(std::stringstream& lines);
	// 内置配置序列化到文件
	static void SaveTo(std::stringstream& out);
private:
	static Flag** flags_;
	static intptr_t capacity_;
	static intptr_t num_flags_;
	static bool initialized_;

	static void Register_unknown(const char* name, const char* value);
	static void AddFlag(Flag* flag);
	static bool SetFlagFromString(Flag* flag, const char* argument);
	static void Parse(const char* option);
	static int CompareFlagNames(const void* left, const void* right);
};
} // namespace of utility

#endif /* UTILITY_FLAGS_H_ */
