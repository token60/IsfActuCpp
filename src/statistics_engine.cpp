#include "gas_analytics/statistics_engine.hpp"

#include <cmath>
#include <stdexcept>

using namespace std;

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

StatisticsEngine::StatisticsEngine(std::vector<DailyConsumption> data)
    : data_(std::move(data)) {}

double StatisticsEngine::mean_daily_total() const {
  std::vector<double> t;
  t.reserve(data_.size());
  for (const auto& d : data_) {
    t.push_back(d.total_consumption_mwh());
  }
  return mean_of(t);
}

double StatisticsEngine::std_dev_daily_total() const {
  std::vector<double> t;
  t.reserve(data_.size());
  for (const auto& d : data_) {
    t.push_back(d.total_consumption_mwh());
  }
  const double m = mean_of(t);
  return std_dev_sample(t, m);
}

double StatisticsEngine::coefficient_of_variation_daily_total() const {
  const double m = mean_daily_total();
  if (m == 0.0) {
    throw std::runtime_error("coefficient_of_variation: moyenne nulle");
  }
  return std_dev_daily_total() / m;
}

double StatisticsEngine::mean_peak_to_valley_ratio() const {
  std::vector<double> ratios;
  ratios.reserve(data_.size());
  for (const auto& d : data_) {
    try {
      ratios.push_back(d.hourly().peak_to_valley_ratio());
    } catch (const std::runtime_error&) {
      // jour non exploitable pour ce ratio
    }
  }
  return mean_of(ratios);
}

double StatisticsEngine::mean_night_to_day_ratio() const {
  std::vector<double> ratios;
  ratios.reserve(data_.size());
  for (const auto& d : data_) {
    try {
      ratios.push_back(d.hourly().night_to_day_ratio());
    } catch (const std::runtime_error&) {
    }
  }
  return mean_of(ratios);
}

std::vector<double> StatisticsEngine::moving_average_totals(std::size_t window) const {
  if (window < 1) {
    throw std::runtime_error("moving_average_totals: fenêtre < 1");
  }
  std::vector<double> totals;
  totals.reserve(data_.size());
  for (const auto& d : data_) {
    totals.push_back(d.total_consumption_mwh());
  }
  if (totals.empty()) {
    return {};
  }

  std::vector<double> out;
  const std::size_t n = totals.size();
  out.reserve(n);

  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t start = (i + 1 >= window) ? (i + 1 - window) : 0;
    double s = 0.0;
    for (std::size_t j = start; j <= i; ++j) {
      s += totals[j];
    }
    out.push_back(s / static_cast<double>(i - start + 1));
  }
  return out;
}

} // namespace gas
