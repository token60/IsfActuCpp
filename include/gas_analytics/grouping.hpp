#pragma once

#include "gas_analytics/daily_consumption.hpp"

#include <map>
#include <string>
#include <vector>

namespace gas {

struct GroupStats {
  std::size_t n{};
  double mean_total{};
};

std::map<std::string, GroupStats> group_by_operateur_mean_total(
    const std::vector<DailyConsumption>& data);

std::map<std::string, GroupStats> group_by_secteur_mean_total(
    const std::vector<DailyConsumption>& data);

} // namespace gas

