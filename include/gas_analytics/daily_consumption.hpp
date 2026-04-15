#pragma once

#include "gas_analytics/hourly_record.hpp"

#include <nlohmann/json.hpp>

#include <string>

using namespace std;

namespace gas {

/// Une journée de consommation : métadonnées + profil 24h + total agrégé API.
class DailyConsumption {
public:
  DailyConsumption() = default;

  DailyConsumption(
      std::string date,
      std::string operateur,
      std::string secteur,
      double consommation_totale_mwh,
      HourlyRecord hourly);

  /// Parse JSON — les clés ci-dessous sont des placeholders à mapper sur la
  /// réponse réelle de l’API Teréga (voir doc officielle des champs).
  static DailyConsumption from_json(const nlohmann::json& j);

  const std::string& date() const { return date_; }
  const std::string& operateur() const { return operateur_; }
  const std::string& secteur() const { return secteur_; }
  double total_consumption_mwh() const { return total_mwh_; }
  const HourlyRecord& hourly() const { return hourly_; }

private:
  std::string date_;
  std::string operateur_;
  std::string secteur_;
  double total_mwh_{0.0};
  HourlyRecord hourly_;
};

} // namespace gas
