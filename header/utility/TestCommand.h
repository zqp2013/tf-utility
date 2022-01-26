/*
 * RunDemoCommand.cpp
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */
#pragma once
#include "CCommand.h"
#include <iostream>
#include "tf_export.h"

typedef void(*FuncPtr)(void);
typedef struct _TestFunc {
	const char* func_name;
	FuncPtr ptr;
} TestFunc;

class TF_EXT_CLASS TestFuncRegister {
public:
	TestFuncRegister(const char* func_name, FuncPtr func);
	~TestFuncRegister() {}
};

#define TEST_FUNC_ENTRY(func) \
	static TestFuncRegister reg_##func(#func, test_##func);

class TF_EXT_CLASS TestCommand : public utility::CCommand {
public:
	TestCommand() : CCommand("test") {}
	virtual const char* Help() {
		return "test [func_name] func.";
	}
	virtual bool Run(const std::string& cmd, std::ostream& out);
};
