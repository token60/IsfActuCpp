#pragma once

#include "gas_analytics/daily_consumption.hpp"

#include <cstddef>
#include <vector>

namespace gas {

struct Anomaly {
  std::size_t index{};
  double value{};
  double z_score{};
};

/// Détecte les jours dont la consommation totale s'écarte de plus de k écarts-types.
class AnomalyDetector {
public:
  std::vector<Anomaly> detect(const std::vector<DailyConsumption>& data, double k) const;
};

} // namespace gas

