#pragma once

#include "gas_analytics/daily_consumption.hpp"

#include <vector>

using namespace std;

namespace gas {

/// Calculs statistiques sur une série de journées (totaux journaliers + profils).
class StatisticsEngine {
public:
  explicit StatisticsEngine(std::vector<DailyConsumption> data);

  const std::vector<DailyConsumption>& data() const { return data_; }

  double mean_daily_total() const;
  double std_dev_daily_total() const;
  double coefficient_of_variation_daily_total() const;

  /// Moyenne des ratios pic/creux par jour (ignore les jours où le ratio est impossible).
  double mean_peak_to_valley_ratio() const;

  /// Moyenne des ratios nuit/jour par jour.
  double mean_night_to_day_ratio() const;

  /// Moyenne glissante des totaux journaliers (fenêtre `window`, taille >= 1).
  std::vector<double> moving_average_totals(std::size_t window) const;

private:
  std::vector<DailyConsumption> data_;
};

} // namespace gas
