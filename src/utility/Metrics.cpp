/*
 * Metrics.cpp
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */
#include "utility/Metrics.h"
#include <vector>
#include <algorithm>
#include "utility/Macros.h"
#include "utility/Flags.h"

DEFINE_FLAG(int, dump_metrics, 30, "Dump top N metrics.");

namespace utility {

Metrics* Metrics::GetInstance() {
	static Metrics s_metrics;
	return &s_metrics;
}

Metrics::~Metrics() {
	for (auto it = metrics_.begin(); it != metrics_.end(); it++)
		delete *it;
}

void Metrics::Report(std::ostream& out) {
	int width = 0;

	std::vector<Metric*> metrics;
	metrics.reserve(metrics_.size());

	for (auto it = metrics_.begin(); it != metrics_.end(); it++)
		metrics.emplace_back(*it);

	std::sort(metrics.begin(), metrics.end(), [](const Metric* a, const Metric* b) {
		return a->sum > b->sum;
	});

	for (auto it = metrics.begin(); it != metrics.end(); it++)
		width = std::max((int)(*it)->name.size(), width);

	size_t buff_size = 120;
	char* buff = new char[buff_size];
	snprintf(buff, buff_size, "%-*s\t%-6s\t%-9s\t%-9s\t%s", width,
			"metrics", "count", "avg (us)", "max (us)", "total (ms)");
	out << buff << std::endl;

	size_t n = 0;
	for (auto it = metrics.begin(); it != metrics.end(); it++) {
		Metric* metric = *it;
		double total = metric->sum / (double)1000;
		double avg = metric->sum / (double)metric->count;
		double max = (double)metric->max;
		snprintf(buff, buff_size, "%-*s\t%-6" PRIu64 "\t%-8.1f\t%-8.1f\t%.1f", width, metric->name.c_str(), metric->count, avg, max, total);
		out << buff << std::endl;
		if (FLAG_dump_metrics != 0 && (int)n++ > FLAG_dump_metrics) {
			out << "List top" << FLAG_dump_metrics << " of " << metrics.size() << " metrics." << std::endl;
			break;
		}
	}
	delete[] buff;
}

void Metrics::Reset() {
	for (auto it = metrics_.begin(); it != metrics_.end(); it++) {
		Metric* metric = *it;
		metric->count = 0;
		metric->sum = 0;
		metric->max = 0;
	}
}

} // end of namespace utility


