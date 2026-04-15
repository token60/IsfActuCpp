#include "gas_analytics/daily_consumption.hpp"

#include <sstream>
#include <stdexcept>

using namespace std;

namespace gas {

namespace {

[[noreturn]] void throw_schema_error(const char* msg) {
  throw runtime_error(string("DailyConsumption::from_json: ") + msg);
}

string hour_key(int h) {
  ostringstream oss;
  if (h < 10) {
    oss << '0';
  }
  oss << h << "_00";
  return oss.str();
}

void fill_hourly_from_fields(const nlohmann::json& obj, HourlyRecord& out) {
  for (int h = 0; h < 24; ++h) {
    const string key = hour_key(h);
    if (!obj.contains(key) || !obj[key].is_number()) {
      ostringstream oss;
      oss << "champ horaire '" << key << "' manquant ou non-numérique";
      throw_schema_error(oss.str().c_str());
    }
    out.mwh[static_cast<size_t>(h)] = obj[key].get<double>();
  }
}

} // namespace

DailyConsumption::DailyConsumption(
    std::string date,
    std::string operateur,
    std::string secteur,
    double consommation_totale_mwh,
    HourlyRecord hourly)
    : date_(std::move(date)),
      operateur_(std::move(operateur)),
      secteur_(std::move(secteur)),
      total_mwh_(consommation_totale_mwh),
      hourly_(hourly) {}

DailyConsumption DailyConsumption::from_json(const nlohmann::json& j) {
  if (!j.is_object()) {
    throw_schema_error("racine attendue: objet JSON");
  }

  // Schéma réel Opendatasoft (dataset conso-journa-industriel-grtgazterega).
  if (!j.contains("date") || !j["date"].is_string()) {
    throw_schema_error("champ 'date' manquant ou non-string");
  }
  if (!j.contains("operateur") || !j["operateur"].is_string()) {
    throw_schema_error("champ 'operateur' manquant ou non-string");
  }
  if (!j.contains("secteur_activite") || !j["secteur_activite"].is_string()) {
    throw_schema_error("champ 'secteur_activite' manquant ou non-string");
  }
  if (!j.contains("consommation_journaliere_mwh_pcs") ||
      !j["consommation_journaliere_mwh_pcs"].is_number()) {
    throw_schema_error("champ 'consommation_journaliere_mwh_pcs' manquant ou non-numérique");
  }

  const string date = j["date"].get<string>();
  const string operateur = j["operateur"].get<string>();
  const string secteur = j["secteur_activite"].get<string>();
  const double total = j["consommation_journaliere_mwh_pcs"].get<double>();

  HourlyRecord hr;
  fill_hourly_from_fields(j, hr);

  return DailyConsumption(date, operateur, secteur, total, hr);
}

} // namespace gas
