/*
 * cron_test.cc
 *
 *  Created on: Jul 30, 2021
 *      Author: qpzhou
 */

#include "utility/TestCommand.h"
#include "utility/cron.h"
using namespace tf;

void test_cron() {
	//bitset test
	cout << "__CHAR_BIT__:" << __CHAR_BIT__ <<
			" __SIZEOF_LONG__:" << __SIZEOF_LONG__ <<
			" sizeof(long):" << sizeof(long) << endl;
	std::bitset<9> orders;  // 底层以若干个8字节(long)存储, 7 -> 1, 9 -> 2
	TC_Cron::CronField field = TC_Cron::CronField::year;
	if (!orders.test(static_cast<size_t>(field)))
		orders.set(static_cast<size_t>(field));

	// cron test
	auto cron = TC_Cron::makecron("0 59 23 * * *"); //23:59 every day
	int i = 0;
	while (i++ < 5) {
		auto interval = cron.nextcron(cron) - time(0); //返回的是下次触发的时间点，而非时间间距
		std::cout << "To 23:59:00 left " << interval << "s." << endl;
		std::this_thread::sleep_for(1s);
	}
}

TEST_FUNC_ENTRY(cron)
