/*
 * Flags.cpp
 *
 *  Created on: 2019年12月11日
 *      Author: qpzhou
 */

#include "Flags.h"
#include "DiskInterface.h"
#include "StringUtils.hpp"
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
using namespace std;

namespace utility {

DEFINE_FLAG(bool, print_flags, false, "Print flags as they are being parsed.");
DEFINE_FLAG(bool, ignore_unrecognized_flags, true, "Ignore unrecognized flags.");

bool Flags::initialized_ = false;

// List of registered flag.
Flag** Flags::flags_ = nullptr;
intptr_t Flags::capacity_ = 0;
intptr_t Flags::num_flags_ = 0;

class Flag {
public:
	Flag(const char* name, const char* comment, void* addr, Flags::FlagType type)
		: name_(name), comment_(comment), addr_(addr), type_(type) {}
	Flag(const char* name, const char* comment, FlagHandler handler)
		: name_(name), comment_(comment), handler_(handler), type_(Flags::kFunc) {}

	bool IsUnrecognized() const { return type_ == Flags::kUnknow; }
	void Print(std::ostream& out) {
		if (IsUnrecognized()) {
			out << name_ << ": unrecognized" << std::endl;
			return;
		}
		out << name_ << ": ";
		switch(type_) {
		case Flags::kBoolean: {
			out << (*this->bool_ptr_ ? "true" : "false");
			break;
		}
		case Flags::kInteger: {
			out << *this->int_ptr_;
			break;
		}
		case Flags::kUint64: {
			out << *this->uint64_ptr_;
			break;
		}
		case Flags::kString: {
			if (*this->charp_ptr_ != nullptr)
				out << "'" << *this->charp_ptr_ << "'";
			else
				out << "(null)";
			break;
		}
		case Flags::kFunc: {
			break;
		}
		default:
			UNREACHABLE();
			break;
		}
		out << " (" << comment_ << ")" << std::endl;
	}

	void Recognize(const char* comment, void* addr, Flags::FlagType type)  {
		DCHECK(type == Flags::kUnknow);
		const char* argument = argument_;
		addr_ = addr;
		type_ = type;
		comment_ = comment;
		if (argument) {
			const char* err;
			bool ok = Flags::SetFlag(name_, argument, &err);
			if (!ok)
				LOG(WARNING) << "set flag: " << name_ << "=" << *argument << " failed:" << err;
		}
	}

	void Recognize(const char* comment, FlagHandler handler)  {
		DCHECK(type == Flags::kUnknow);
		handler_ = handler;
		type_ = Flags::kFunc;
		comment_ = comment;
	}

	const char* name_;
	const char* comment_;
	union {
		void* addr_;
		bool* bool_ptr_;
		int* int_ptr_;
		uint64_t* uint64_ptr_;
		charp* charp_ptr_;
		const char* argument_;
		FlagHandler handler_;
	};
	Flags::FlagType type_;
	bool changed_ { false };
};

Flag* Flags::Lookup(const char* name) {
	for (intptr_t i = 0; i < num_flags_; i++) {
		Flag* flag = flags_[i];
		if (strcmp(flag->name_, name) == 0)
			return flag;
	}
	return nullptr;
}

bool Flags::Initialized() { return initialized_; }

bool Flags::IsSet(const char* name) {
	Flag* flag = Lookup(name);
	return flag && (Flags::kBoolean == flag->type_) && flag->bool_ptr_ && (*flag->bool_ptr_);
}

void Flags::AddFlag(Flag* flag) {
	if (num_flags_ == capacity_) {
		if (!flag) {
			capacity_ = 256;
			flags_ = new Flag*[capacity_];
		} else {
			intptr_t new_capacity = capacity_ * 2;
			Flag** new_flags = new Flag*[new_capacity];
			for (intptr_t i = 0; i < num_flags_; i++)
				new_flags[i] = flags_[i];
			delete[] flags_;
			flags_ = new_flags;
			capacity_ = new_capacity;
		}
	}
	flags_[num_flags_++] = flag;
}

bool Flags::Register_bool(bool* addr, const char* name, bool default_value, const char* comment) {
	Flag* flag = Lookup(name);
	if (!flag) {
		flag = new Flag(name, comment, addr, Flags::kBoolean);
		AddFlag(flag);
	} else if(flag->IsUnrecognized()) {
		flag->Recognize(comment, addr, Flags::kBoolean);
	}
	return default_value;
}

int Flags::Register_int(int* addr, const char* name, int default_value, const char* comment) {
	Flag* flag = Lookup(name);
	if (!flag) {
		flag = new Flag(name, comment, addr, Flags::kInteger);
		AddFlag(flag);
	} else if(flag->IsUnrecognized()) {
		flag->Recognize(comment, addr, Flags::kInteger);
	}
	return default_value;
}

uint64_t Flags::Register_uint64_t(uint64_t* addr, const char* name, uint64_t default_value, const char* comment) {
	Flag* flag = Lookup(name);
	if (!flag) {
		flag = new Flag(name, comment, addr, Flags::kUint64);
		AddFlag(flag);
	} else if(flag->IsUnrecognized()) {
		flag->Recognize(comment, addr, Flags::kUint64);
	}
	return default_value;
}

const char* Flags::Register_charp(charp* addr, const char* name, charp default_value, const char* comment) {
	Flag* flag = Lookup(name);
	if (!flag) {
		flag = new Flag(name, comment, addr, Flags::kString);
		AddFlag(flag);
	} else if(flag->IsUnrecognized()) {
		flag->Recognize(comment, addr, Flags::kString);
	}
	return default_value;
}

bool Flags::Register_func(FlagHandler handler, const char* name, const char* comment) {
	Flag* flag = Lookup(name);
	if (!flag) {
		flag = new Flag(name, comment, handler);
		AddFlag(flag);
	} else if(flag->IsUnrecognized()) {
		flag->Recognize(comment, handler);
	}
	return false;
}

void Flags::Register_unknown(const char* name, const char* value) {
	Flag* flag = new Flag(strdup(name), "unrecognized", value ? strdup(value) : nullptr, Flags::kUnknow);
	AddFlag(flag);
}

static void Normalize(char* s) {
	intptr_t len = strlen(s);
	for (intptr_t i = 0; i < len; i++) {
		if (s[i] == '-')
			s[i] = '_';
	}
}

bool Flags::SetFlagFromString(Flag* flag, const char* argument) {
	if (flag->IsUnrecognized() || !argument)
		return false;

	switch(flag->type_) {
	case Flags::kBoolean: {
		if (strcmp(argument, "true") == 0) {
			*flag->bool_ptr_ = true;
		} else if (strcmp(argument, "false") == 0) {
			*flag->bool_ptr_ = false;
		} else {
			return false;
		}
		break;
	}
	case Flags::kString: {
		*flag->charp_ptr_ = argument ? strdup(argument) : nullptr;
		break;
	}
	case Flags::kInteger: {
		char* endptr = nullptr;
		const intptr_t len = strlen(argument);
		int base = 10;
		if ((len > 2) && (argument[0] == '0') && (argument[1] == 'x')) {
			base = 16;
		}
		int val = strtol(argument, &endptr, base);
		if (endptr == argument + len) {
			*flag->int_ptr_ = val;
		} else {
			return false;
		}
		break;
	}
	case Flags::kUint64: {
		char* endptr = nullptr;
		const intptr_t len = strlen(argument);
		int base = 10;
		if ((len > 2) && (argument[0] == '0') && (argument[1] == 'x')) {
			base = 16;
		}
		int64_t val = strtoll(argument, &endptr, base);
		if (endptr == argument + len) {
			*flag->uint64_ptr_ = static_cast<int64_t>(val);
		} else {
			return false;
		}
		break;
	}
	case Flags::kFunc: {
		if (strcmp(argument, "true") == 0) {
			(flag->handler_)(true);
		} else if (strcmp(argument, "false") == 0) {
			(flag->handler_)(false);
		} else {
			return false;
		}
		break;
	}
	default: {
		UNREACHABLE();
		return false;
	}
	}
	flag->changed_ = true;
	return true;
}

void Flags::Parse(const char* option) {
	const char* equals = option;
	while ((*equals != '\0') && (*equals != '=')) {
		equals++;
	}

	const char* argument = nullptr;
	if (*equals != '=') {
		const char* kNo1Prefix = "no_";
		const char* kNo2Prefix = "no-";
		const intptr_t kNo1PrefixLen = strlen(kNo1Prefix);
		const intptr_t kNo2PrefixLen = strlen(kNo2Prefix);
		if (strncmp(option, kNo1Prefix, kNo1PrefixLen) == 0) {
			option += kNo1PrefixLen;
			argument = "false";
		} else if (strncmp(option, kNo2Prefix, kNo2PrefixLen) == 0) {
			option += kNo2PrefixLen;
			argument = "false";
		} else {
			argument = "true";
		}
	} else {
		argument = equals + 1;
	}

	intptr_t name_len = equals - option;
	char* name = new char[name_len + 1];
	strncpy(name, option, name_len);
	name[name_len] = '\0';
	Normalize(name);

	Flag* flag = Flags::Lookup(name);
	if (!flag) {
		Register_unknown(name, argument);
	} else {
		if (flag->IsUnrecognized()) {
			if (!SetFlagFromString(flag, argument)) {
				printf("Ignoring flag: %s %s is an invalid value for flag", argument, name);
			}
		}
	}
	delete[] name;
}

static bool IsValidFlag(const char* name, const char* prefix, intptr_t prefix_length) {
	intptr_t name_length = strlen(name);
	return ((name_length > prefix_length) && (strncmp(name, prefix, prefix_length) == 0));
}

int Flags::CompareFlagNames(const void* left, const void* right) {
	const Flag* left_flag = reinterpret_cast<const Flag* const>(left);
	const Flag* right_flag = reinterpret_cast<const Flag* const>(right);
	return strcmp(left_flag->name_, right_flag->name_);
}

bool Flags::ProcessCommandLineFlags(int number_of_vm_flags, const char** vm_flags) {
	if (initialized_)
		return false;

	qsort(flags_, num_flags_, sizeof flags_[0], CompareFlagNames);

	const char* kPrefix = "--";
	const intptr_t kPrefixLen = strlen(kPrefix);

	int i = 0;
	while (i < number_of_vm_flags) {
		if (IsValidFlag(vm_flags[i], kPrefix, kPrefixLen)) {
			const char* option = vm_flags[i] + kPrefixLen;
			Parse(option);
		}
		i++;
	}

	if (!FLAG_ignore_unrecognized_flags) {
		int unrecognized_count = 0;
		for (intptr_t j = 0; j < num_flags_; j++) {
			Flag* flag = flags_[j];
			if (flag->IsUnrecognized()) {
				if (unrecognized_count == 0)
					fprintf(stderr, "Unrecognized flags: %s", flag->name_);
				else
					fprintf(stderr, ", %s", flag->name_);
				unrecognized_count++;
			}
		}
		if (unrecognized_count > 0) {
			fprintf(stderr, "\n");
			exit(255);
		}
	}
	if (FLAG_print_flags)
		PrintFlags(std::cout);

	initialized_ = true;
	return true;
}

#if !defined(_WIN32)
static vector<string> Tokenize(const std::string& src, const std::string& tok, bool trim = false, const std::string& null_subst = "") {
	std::vector<std::string> v;
	if (src.empty() || tok.empty())
		return v;

	size_t pre_index = 0, index = 0, len = 0;
	while ((index = src.find_first_of(tok, pre_index)) != string::npos)
	{
		if ((len = index - pre_index) != 0)
			v.emplace_back(src.substr(pre_index, len));
		else if (trim == false)
			v.emplace_back(null_subst);
		pre_index = index + 1;
	}
	std::string endstr = src.substr(pre_index);
	if (trim == false) v.emplace_back(endstr.empty() ? null_subst : endstr);
	else if (!endstr.empty())
		v.emplace_back(endstr);
	return v;
}
#endif // !defined(_WIN32)

bool Flags::ProcessCommandLineFlags() {
#if defined(_WIN32)
	int argc = 0;
	LPCWSTR cmd = GetCommandLineW();
	wchar_t** argvw = (wchar_t**)CommandLineToArgvW(cmd, &argc);
	char** argv2 = new char*[argc];
	for (int i = 0; i < argc; i++)
		argv2[i] = utility::WideToUtf8(argvw[i], -1, nullptr);
	LocalFree(argvw);
	Flags::ProcessCommandLineFlags(argc - 1, (const char**)argv2 + 1);
	for (int i = 0; i < argc; i++)
		free(argv2[i]);
	delete argv2;
#else
	std::string cmdline;
	if (utility::DiskInterface::getInstance().ReadFile("/proc/self/cmdline", &cmdline)) {
		for (size_t i = 0; i < cmdline.size(); i++) {
			if (cmdline.data()[i] == 0)
				const_cast<char*>(cmdline.data())[i] = ' ';
		}
		std::vector<std::string> argsList = Tokenize(cmdline, " ", true);
		int argc = argsList.size();
		char** argv2 = new char*[argsList.size()];
		for (size_t i = 0; i < argsList.size(); i++)
			argv2[i] = strdup(argsList[i].c_str());
		Flags::ProcessCommandLineFlags(argc - 1, (const char**)argv2 + 1);
		for (int i = 0; i < argc; i++)
			free(argv2[i]);
		delete argv2;
	}
#endif
	return true;
}

bool Flags::LoadFromFile(const char* file) {
	std::string content;
	auto& fs = utility::DiskInterface::getInstance();
	if (fs.FileExists(file) && fs.ReadFile(file, &content)) {
		std::stringstream s(content);
		Flags::LoadFrom(s);
		return true;
	}
	return false;
}

bool Flags::SaveToFile(const char* file) {
	std::stringstream ss;
	Flags::SaveTo(ss);
	return utility::DiskInterface::getInstance().WriteFile(file, ss.str());
}

bool Flags::SetFlag(const char* name, const char* value, const char** error) {
	Flag* flag = Lookup(name);
	if (!flag) {
		*error = "Cannot set flag: flag not found";
		return false;
	}
	if (!SetFlagFromString(flag, value)) {
		*error = "Cannot set flag: invalid value";
		return false;
	}
	return true;
}

void Flags::PrintFlags(std::ostream& out) {
	out << "Flag settings:" << std::endl;
	for (intptr_t i = 0; i < num_flags_; i++)
		flags_[i]->Print(out);
}

void Flags::Shutdown() {
	delete[] flags_;
	capacity_ = 0;
	num_flags_ = 0;
	initialized_ = false;
}

std::unique_ptr<Flags::Iterator> Flags::NewIterator() {
	return std::unique_ptr<Flags::Iterator>(new Iterator());
}

// 按行解析FLAGS文件，每行一个FLAG=value，支持#注释
struct LineScanner {
	const std::string& line;
	int kind { kUnknown };
	std::string token;
	std::string::const_iterator it;
	int ch;

	enum {
		kUnknown,
		kName,
		kValue,
		kAssign,
	};

	LineScanner(const std::string& line) : line(line) {
		it = line.begin();
		if (it == line.end())
			ch = 0;
		else
			ch = *it;
	}

	bool End() { return ch == 0; }
	void Save() { token.push_back(ch); }
	void Advance() {
		if (it != line.end()) {
			it++;
			if (it != line.end()) {
				ch = *it;
				return;
			}
		}
		ch = 0;
	}
	void SaveAdvance() {
		Save();
		Advance();
	}
	bool Next() {
		kind = kUnknown;
		token.clear();
		if (End())
			return false;

		while(isblank(ch))
			Advance();

		if (ch == '#')
			return false;

		if (ch == '=') {
			kind = kAssign;
			SaveAdvance();
			return true;
		}

		if (ch == '\"' || ch == '\'') {
			auto q = ch;
			Advance();
			while(ch != 0 && ch != q) {
				SaveAdvance();
			}
			if (ch == q) {
				kind = kValue;
				return true;
			}
			// Expect quote.
			return false;
		}

		if (isalnum(ch) || ch == '_' || ch == '-') {
			do {
				SaveAdvance();
			} while(isalnum(ch) || ch == '_' || ch == '-');
			kind = kName;
			return true;
		}
		return false;
	}
};

bool Flags::LoadFrom(std::stringstream& lines) {
	bool ok = true;
	while(true) {
		std::string line;
		std::getline(lines, line);
		if (!lines)
			break;

		if (line.empty())
			continue;

		if (line.back() == '\r') {
			line.pop_back();
			if (line.empty())
				continue;
		}

		LineScanner scanner(line);
		if (!scanner.Next())
			continue;

		std::string name;
		std::string value;
		if (scanner.kind == LineScanner::kName) {
			name = scanner.token;
			if (!scanner.Next()) {
				std::cout << "Invalid flag line:" << line << std::endl;
				continue;
			}
			if (scanner.kind == LineScanner::kAssign) {
				scanner.Next();
				if (scanner.kind == LineScanner::kValue || scanner.kind == LineScanner::kName) {
					value = scanner.token;
					// 后面不应该再有内容了
					if (scanner.Next()) {
						std::cout << "Invalid flag line:" << line << std::endl;
						continue; // 出错
					}

					const char* err = nullptr;
					SetFlag(name.c_str(), value.c_str(), &err);
					if (err)
						std::cout << "Invalid flag line:" << line << ":" << err << std::endl;
					// 忽略后面的内容
					continue;
				}
			}
		}
	}
	return ok;
}

void Flags::SaveTo(std::stringstream& out) {
	auto it = NewIterator();
	bool first = true;
	for (it->BeforeFirst(); it->Next();) {
		if (it->Type() == Flags::kInteger
			|| it->Type() == Flags::kBoolean
			|| it->Type() == Flags::kUint64
			|| it->Type() == Flags::kString) {
			if (!first)
				out << std::endl;

			out << "# " << it->Comment() << std::endl;
			out << it->Name() << " = ";
			it->Repr(out);
			out << std::endl;
			first = false;
		}
	}
}

Flags::Iterator::Iterator() : flag_(nullptr) {}
Flags::Iterator::~Iterator() {}
void Flags::Iterator::BeforeFirst() { flag_ = nullptr; }
bool Flags::Iterator::Next() {
	if (num_flags_ == 0)
		return false;
	if (!flag_) {
		flag_ = Flags::flags_;
		return true;
	}
	if (flag_ + 1 < Flags::flags_ + num_flags_) {
		flag_++;
		return true;
	}
	flag_ = nullptr;
	return false;
}
Flags::FlagType Flags::Iterator::Type() const { return (*flag_)->type_; }
const char* Flags::Iterator::Name() const { return (*flag_)->name_; }
const char* Flags::Iterator::Comment() const { return (*flag_)->comment_; }
bool Flags::Iterator::Repr(std::stringstream& out) const {
	Flag* flag = *flag_;
	switch(flag->type_) {
	case Flags::kBoolean:
		out << (*AsBoolean() ? "true" : "false");
		break;
	case Flags::kInteger:
		out << *AsInt();
		break;
	case Flags::kUint64:
		out << *AsUint64();
		break;
	case Flags::kString:
		out << "\"" << *AsString() << "\"";
		break;
	default:
		return false;
	}
	return true;
}
int* Flags::Iterator::AsInt() const {
	Flag* flag = *flag_;
	if (Flags::kInteger == flag->type_)
		return flag->int_ptr_;
	return nullptr;
}
uint64_t* Flags::Iterator::AsUint64() const {
	Flag* flag = *flag_;
	if (Flags::kUint64 == flag->type_)
		return flag->uint64_ptr_;
	return nullptr;
}
const char* Flags::Iterator::AsString() const {
	Flag* flag = *flag_;
	if (Flags::kString == flag->type_)
		return (const char*)flag->charp_ptr_;
	return nullptr;
}
bool* Flags::Iterator::AsBoolean() const {
	Flag* flag = *flag_;
	if (Flags::kBoolean == flag->type_)
		return flag->bool_ptr_;
	return nullptr;
}

} // namespace of utility
