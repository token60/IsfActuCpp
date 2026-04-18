#include "gas_analytics/anomaly_detector.hpp"

#include <cmath>
#include <stdexcept>

namespace gas {

namespace {

double mean_of(const std::vector<double>& v) {
  if (v.empty()) {
    return 0.0;
  }
  double s = 0.0;
  for (double x : v) {
    s += x;
  }
  return s / static_cast<double>(v.size());
}

double std_dev_sample(const std::vector<double>& v, double mean) {
  if (v.size() < 2) {
    return 0.0;
  }
  double acc = 0.0;
  for (double x : v) {
    const double d = x - mean;
    acc += d * d;
  }
  return std::sqrt(acc / static_cast<double>(v.size() - 1));
}

} // namespace

std::vector<Anomaly> AnomalyDetector::detect(const std::vector<DailyConsumption>& data,
                                             double k) const {
  if (k <= 0.0) {
    throw std::runtime_error("AnomalyDetector::detect: k doit être > 0");
  }

  std::vector<double> totals;
  totals.reserve(data.size());
  for (const auto& d : data) {
    totals.push_back(d.total_consumption_mwh());
  }

  const double m = mean_of(totals);
  const double sd = std_dev_sample(totals, m);
  if (sd == 0.0) {
    return {};
  }

  std::vector<Anomaly> out;
  for (std::size_t i = 0; i < data.size(); ++i) {
    const double z = (data[i].total_consumption_mwh() - m) / sd;
    if (std::fabs(z) > k) {
      out.push_back(Anomaly{.index = i, .value = data[i].total_consumption_mwh(), .z_score = z});
    }
  }
  return out;
}

} // namespace gas

