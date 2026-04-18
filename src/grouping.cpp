#include "gas_analytics/grouping.hpp"

#include <unordered_map>

namespace gas {

namespace {

template <class KeyFn>
std::map<std::string, GroupStats> group_mean_total(const std::vector<DailyConsumption>& data,
                                                   KeyFn key_fn) {
  struct Acc {
    std::size_t n{};
    double sum{};
  };
  std::unordered_map<std::string, Acc> acc;
  for (const auto& d : data) {
    auto& a = acc[key_fn(d)];
    a.n += 1;
    a.sum += d.total_consumption_mwh();
  }

  std::map<std::string, GroupStats> out;
  for (const auto& [k, a] : acc) {
    out.emplace(k, GroupStats{.n = a.n, .mean_total = (a.n ? (a.sum / static_cast<double>(a.n)) : 0.0)});
  }
  return out;
}

} // namespace

std::map<std::string, GroupStats> group_by_operateur_mean_total(
    const std::vector<DailyConsumption>& data) {
  return group_mean_total(data, [](const DailyConsumption& d) { return d.operateur(); });
}

std::map<std::string, GroupStats> group_by_secteur_mean_total(
    const std::vector<DailyConsumption>& data) {
  return group_mean_total(data, [](const DailyConsumption& d) { return d.secteur(); });
}

} // namespace gas

