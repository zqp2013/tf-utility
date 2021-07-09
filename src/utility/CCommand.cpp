/*
 * CCommand.cpp
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */

#include "CCommand.h"
#include "Macros.h"
#include "StringUtils.hpp"
#if _WIN32
#include <Windows.h>
#endif
#include <iostream>
#include <string.h>
#include <fstream>
#include <list>
#include <algorithm>
#include <limits>
#include "Metrics.h"
#include "Flags.h"
using namespace std;

namespace utility {

//清除控制台
static void ClearConsole(char fill = ' ') {
#if _WIN32
	COORD t1 = {0, 0};
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, t1, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, t1, &written);
	SetConsoleCursorPosition(console, t1);
#endif
}

static list<CCommand*>& commands() {
	static list<CCommand*> data;
	return data;
}

void CCommandManager::PrintHelp(ostream& out) {
	int width = 0;
	for (auto it = commands().begin(); it != commands().end(); it++) {
		width = max((int)(*it)->name().size(), width);
	}

	size_t buff_size = 120;
	char* buff = (char*)malloc(buff_size);
	snprintf(buff, buff_size, "%-*s\t%-6s", width, "command", "usage");
	out << buff << endl;
	for (auto it = commands().begin(); it != commands().end(); it++) {
		CCommand* cmd = *it;
		if (strchr(cmd->Help(), '\n') == 0) {
			snprintf(buff, buff_size, "%-*s\t%-6s", width, cmd->name().c_str(), cmd->Help());
			out << buff << endl;
		} else {
			char* help = strdup(cmd->Help());
			char* token = help;
			char* p = help;
			for(; *p; p++) {
				if (*p == '\n') {
					*p = '\0';
					if (p - token > 1) {
						snprintf(buff, buff_size, "%-*s\t%-6s", width, (token == help) ? cmd->name().c_str() : "", token);
						out << buff << endl;
					}
					token = p + 1;
				}
			}
			if (p - token > 1) {
				snprintf(buff, buff_size, "%-*s\t%-6s", width, (token == help) ? cmd->name().c_str() : "", token);
				out << buff << endl;
				token = p + 1;
			}
			free(help);
		}
	}
	free(buff);
}

CCommand* CCommandManager::FindCommand(const std::string& prefix) {
	auto it = find_if(commands().begin(), commands().end(), [&prefix](const CCommand* cmd){
		return prefix.size() == cmd->name().size()
				&& (strnicmp(cmd->name().data(), prefix.data(), prefix.size()) == 0);
	});
	if (it != commands().end())
		return *it;
	return nullptr;
}

void CCommandManager::RegisterCommand(CCommand* cmd) {
	commands().emplace_back(cmd);
}

void CCommandManager::UnregisterCommand(CCommand* cmd) {
	commands().remove(cmd);
}

bool CCommandManager::RunCommand(std::string command, std::ostream& xout) {
	// 不同环境下，getline返回的结果可能会包含'\r'，滤掉
	if (command.back() == '\r')
		command.pop_back(); //去掉末尾字符

	size_t i = 0;
	for(; i < command.length(); i++) {
		if (command[i] != ' ') break;
	}

	// 如果命令前后有引号，滤掉
	size_t len = command.size();
	while(i < len && command.front() == '"' && command.back() == '"') {
		i++;
		len--;
	}

	// 如果命令中有分号，认为是多个命令连续执行。先分号分割，在逐个执行。以最后一个为主，确认是否需要退出
	if (command.find(';') != string::npos) {
		command = command.substr(i, len);
		auto commands = utility::Split(command, ';');
		bool ok = true;
		for (auto cmd : commands)
			ok = RunCommand(cmd, xout);
		return ok;
	}

	ostream* out = &xout;
	fstream fs;

	// 输出文件引导符
	const char kFilePrefix[] = ">>";
	size_t out_pos = command.find(kFilePrefix, i);
	if (out_pos != string::npos) {
		size_t file_name_pos = out_pos + sizeof(kFilePrefix) - 1;
		while(file_name_pos < command.size() && isspace(command[file_name_pos]))
			file_name_pos++;

		string file_name = command.substr(file_name_pos);
		fs.open("." PATH_SEPARATOR_STR "" + file_name, fstream::out | fstream::trunc);
		if (!fs) {
			xout << "Invalid command:" << command << endl;
		} else {
			out = &fs;
			command = command.substr(i, out_pos - i);
		}
	}

	string prefix = command.substr(i, command.find(' ', i));

	if (prefix == "quit")
		return false;

	CCommand* cmd = nullptr;
	if (prefix.empty() || (cmd = CCommandManager::FindCommand(prefix)) == nullptr) {
		xout << "Invalid command:" << prefix << endl;
		CCommandManager::PrintHelp(*out);
	} else {
		if (!cmd->Run(command, *out)) {
			xout << "Usage:" << cmd->Help() << endl;
		}
	}
	return true;
}

bool CCommandManager::DoREPL() {
	string command;
	while (command.empty()) {
		cout << ">>>";
		std::getline(std::cin, command, '\n');
		// ^Z会使用cin进入!cin.
		if (!cin) {
			cin.clear();
			cin.ignore(10000, '\n');
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			cout << endl;
			continue;
		}
	}
	return RunCommand(command, cout);
}


class HelpCommand : public CCommand {
public:
	HelpCommand() : CCommand("help") {}
	const char* Help() override {
		return "print command list.";
	}
	bool Run(const std::string&, ostream& out) {
		CCommandManager::PrintHelp(out);
		return true;
	}
};

class ClearConsoleCommand : public CCommand {
public:
	ClearConsoleCommand() : CCommand("cls") {}
	const char* Help() override {
		return "clear console.";
	}
	bool Run(const std::string&, ostream&) {
		ClearConsole();
		return true;
	}
};

class QuitCommand : public CCommand {
public:
	QuitCommand() : CCommand("quit") {}
	const char* Help() override {
		return "quit console.";
	}
	bool Run(const std::string&, ostream&) {
		return false;
	}
};

class CrashCommand : public CCommand {
public:
	CrashCommand() : CCommand("crash") {}
	const char* Help() override {
		return "crash process.";
	}
	bool Run(const std::string&, ostream& out) {
		char* s = nullptr;
		*s = 'c';
		out << s << endl;
		return false;
	}
};

class ShowMetricsCommand : public CCommand {
public:
	ShowMetricsCommand() : CCommand("metrics") {}
	const char* Help() override {
		return "metrics [reset|dump metrics.txt] show/reset metrics.";
	}
	bool Run(const std::string& cmd, ostream& out) {
		utility::Metrics::GetInstance()->Report(out);

		auto n = cmd.find(' ');
		if (n != cmd.npos) {
			auto sub = cmd.substr(n + 1);
			if (sub == "reset") {
				utility::Metrics::GetInstance()->Reset();
				out << "reset metrics." << endl;
			} else if (sub == "dump") {
				ofstream file("metrics.txt");
				utility::Metrics::GetInstance()->Report(file);
			} else {
				return false;
			}
		}
		return true;
	}
};

class FlagsCommand : public CCommand {
public:
	FlagsCommand() : CCommand("flags") {}
	const char* Help() override {
		return "list flags settings.\n\nflags name=value; set flag value.\n\n";
	}
	bool Run(const std::string& cmd, ostream& out) {
		auto n = cmd.find(' ');
		if (n != cmd.npos) {
			auto kv = cmd.substr(n + 1);
			auto p = kv.find('=');
			if (p != kv.npos) {
				const char* error = nullptr;
				if (!Flags::SetFlag(kv.substr(0, p).c_str(), kv.substr(p + 1).c_str(), &error)) {
					out << error << endl;
					return false;
				} else {
					const char* error = nullptr;
					if (!Flags::SetFlag(kv.substr(0, p).c_str(), nullptr, &error)) {
						out << error << endl;
						return false;
					}
				}
			}
		}
		Flags::PrintFlags(out);
		return true;
	}
};

static HelpCommand help_command;
static ClearConsoleCommand clear_console_command;
static QuitCommand quit_command;
static CrashCommand crash_command;
static ShowMetricsCommand show_metrics_command;
static FlagsCommand flags_command;

} // end of namespace utility
