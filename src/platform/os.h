/*
 * os.h
 *
 *  Created on: 2019年11月29日
 *      Author: qpzhou
 */

#ifndef PLATFORM_OS_H_
#define PLATFORM_OS_H_
#include <iostream>

class OS {
public:
	static void SetCurrentThreadName(const std::string& name);
};


#endif /* PLATFORM_OS_H_ */
