/*
 * RunDemoCommand.cpp
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */
#include "utility/TestCommand.h"
#include "utility/CCommand.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "utility/Metrics.h"

 static std::list<TestFunc>& test_funcs() {
 	static std::list<TestFunc> funcs;
 	return funcs;
 }

TestFuncRegister::TestFuncRegister(const char* func_name, FuncPtr func) {
	test_funcs().emplace_back(TestFunc{ func_name, func });
}

bool TestCommand::Run(const std::string& cmd, std::ostream& out) {
	METRIC_RECORD(__FUNCTION__);
	//MSEC_SLEEP(500);
	auto n = cmd.find(' ');
	if (n != cmd.npos) {
		auto func = cmd.substr(n + 1);
		for (auto f : test_funcs()) {
			if (stricmp(f.func_name, func.c_str()) == 0) {
				f.ptr();
				return true;
			}
		}
		out << "test func:" << func << " is not found!!!" << std::endl;
	}
	else {
		//out << Help() << std::endl;
		std::vector<TestFunc> vec_func;
		for (auto f : test_funcs())
			vec_func.emplace_back(f);
		std::sort(vec_func.begin(), vec_func.end(), [](const TestFunc& a, const TestFunc& b) {
			return stricmp(a.func_name, b.func_name) < 0;
		});
		out << "-----All test functions-----" << std::endl;
		for (auto f : vec_func)
			out << f.func_name << std::endl;
		out << "----------------------------" << std::endl;
	}
	return true;
}

static TestCommand test_command;
