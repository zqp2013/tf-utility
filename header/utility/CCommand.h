#pragma once
#include <string>
#include "Macros.h"
#include "tf_export.h"

namespace utility {
class CCommand;

class TF_EXT_CLASS CCommandManager : public AllStatic {
public:
	static void RegisterCommand(CCommand* cmd);
	static void UnregisterCommand(CCommand* cmd);
	static void PrintHelp(std::ostream& out);
	static CCommand* FindCommand(const std::string& prefix);
	static bool DoREPL();
	static bool RunCommand(std::string command, std::ostream& out);
};

class TF_EXT_CLASS CCommand {
public:
	/**
	 * boot参数：是否独立的程序入口。true 使用--run_command=xxx来启动。
	 */
	CCommand(const std::string& name, bool boot = false) : name_(name), boot_(boot) {
		CCommandManager::RegisterCommand(this);
	}
	virtual ~CCommand() {
		CCommandManager::UnregisterCommand(this);
	}
	virtual const char* Help() = 0;
	virtual bool Run(const std::string& cmd, std::ostream& out) = 0;
	const std::string& name() const { return name_; }
	bool boot() const { return boot_; }
private:
	std::string name_;
	bool boot_;
};

} // end of namespace utility
