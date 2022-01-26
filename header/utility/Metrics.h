/*
 * Metrics.h
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */

#ifndef UTILITY_METRICS_H_
#define UTILITY_METRICS_H_

#include <string>
#include <list>
#include <ostream>
#include "Stopwatch.hpp"
#include "tf_export.h"

namespace utility {

struct TF_EXT_CLASS Metric {
	std::string name;
	size_t count;
	int64_t sum;
	int64_t max;
};

struct ScopedMetric {
	explicit ScopedMetric(Metric* metric);
	~ScopedMetric();
private:
	Metric* metric_;
	int64_t start_;
};

struct TF_EXT_CLASS Metrics {
	static Metrics* GetInstance();
	~Metrics();

	Metric* NewMetric(const std::string& name);
	void Report(std::ostream& out);
	void Reset();
private:
	std::list<Metric*> metrics_;
};

#define METRIC_RECORD(name) \
	static utility::Metric* metrics_h_metric = \
		utility::Metrics::GetInstance() ? utility::Metrics::GetInstance()->NewMetric(name) : nullptr; \
	utility::ScopedMetric metrics_h_scoped(metrics_h_metric);

inline ScopedMetric::ScopedMetric(Metric* metric) {
	metric_ = metric;
	if (!metric_)
		return;
	start_ = HighResTimer();
}

inline ScopedMetric::~ScopedMetric() {
	if (!metric_)
		return;
	metric_->count++;
	int64_t dt = TimerToMicros(HighResTimer() - start_);
	metric_->sum += dt;
	if (dt > metric_->max)
		metric_->max = dt;
}

inline Metric* Metrics::NewMetric(const std::string& name) {
	Metric* metric = new Metric;
	metric->name = name;
	metric->count = 0;
	metric->sum = 0;
	metric->max = 0;
	metrics_.emplace_back(metric);
	return metric;
}

} // end of namespace utility

#endif /* UTILITY_METRICS_H_ */
