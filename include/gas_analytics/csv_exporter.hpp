#pragma once

#include "gas_analytics/anomaly_detector.hpp"
#include "gas_analytics/daily_consumption.hpp"
#include "gas_analytics/grouping.hpp"
#include "gas_analytics/statistics_engine.hpp"

#include <map>
#include <string>
#include <vector>

namespace gas {

class CsvExporter {
public:
  /// Exporte une série: 1 ligne par jour, avec métadonnées + 24 valeurs horaires.
  static void export_daily_series(const std::string& path,
                                  const std::vector<DailyConsumption>& data);
  static void export_statistics_summary(const std::string& path,
                                        const StatisticsEngine& stats);
  static void export_moving_average_totals(
      const std::string& path,
      const std::vector<DailyConsumption>& data,
      const std::vector<double>& moving_average,
      std::size_t window);
  static void export_anomalies(const std::string& path,
                               double k,
                               const std::vector<DailyConsumption>& data,
                               const std::vector<Anomaly>& anomalies);
  static void export_group_stats(const std::string& path,
                                 const std::string& category,
                                 const std::map<std::string, GroupStats>& group);
};

} // namespace gas

