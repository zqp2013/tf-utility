/*
 * PerfStatistics.hpp
 *
 *  Created on: Jul 20, 2020
 *      Author: qpzhou
 */
#include <string>
#include <string.h>
#include <chrono>
#include <stdio.h>

#ifndef SRC_UTILITY_PERFSTATISTICS_HPP_
#define SRC_UTILITY_PERFSTATISTICS_HPP_

namespace utility {
	static std::chrono::steady_clock::time_point PerfStatTimeBasePoint = std::chrono::steady_clock::now();

	//用于循环处理的性能统计
	class PerfStatistics {
	private:
		char* info_;
		std::uint32_t loopCount_;
		std::int64_t beginTime_;
		std::int64_t endTime_;
		//返回当前基于CPU时间点单调时间轴上过去的微秒数
		std::int64_t getCurUSTimeBaseCPU() {
			std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
			std::int64_t nsCount = (nowTime - PerfStatTimeBasePoint).count();//ns
			std::int64_t sCount = nsCount / 1000;
			return sCount;
		}

	public:
		PerfStatistics(const char* info): info_(nullptr) {
			loopCount_ = 1;
			if (info)
				info_ = strdup(info);
			beginTime_ = getCurUSTimeBaseCPU();
		}

		PerfStatistics(uint32_t loopCount, const char* info): info_(nullptr) {
			loopCount_ = loopCount;
			if (info)
				info_ = strdup(info);
			beginTime_ = getCurUSTimeBaseCPU();
		}

		void resetLoopCount(uint32_t loopCount) {
			loopCount_ = loopCount;
		}

		virtual ~PerfStatistics() {
			endTime_ = getCurUSTimeBaseCPU();
			int passedUS = (int)(endTime_ - beginTime_);
			if (passedUS < 0)
				passedUS = 0;
			double pcost = (passedUS*1.0)/loopCount_;
			printf("%s test, loop count is %u, total cost is %dms avg cost is %fus\n", info_?info_:"", loopCount_, passedUS/1000, pcost);
			if (info_) {
				free(info_);
				info_ = nullptr;
			}
		}
	};
}

#endif /* SRC_UTILITY_PERFSTATISTICS_HPP_ */
