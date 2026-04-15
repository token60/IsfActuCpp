#include "gas_analytics/terega_api.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>

using namespace std;

namespace gas {

TeregaApi::TeregaApi(HttpClient& client) : client_(&client) {}

std::vector<DailyConsumption> TeregaApi::parse_response(const std::string& json_body) {
  nlohmann::json root = nlohmann::json::parse(json_body, nullptr, false);
  if (root.is_discarded()) {
    throw std::runtime_error("TeregaApi: JSON invalide");
  }

  std::vector<DailyConsumption> out;

  // Cas Opendatasoft : enveloppe { "total_count": ..., "results": [ ... ] }
  if (root.is_object() && root.contains("results") && root["results"].is_array()) {
    for (const auto& item : root["results"]) {
      out.push_back(DailyConsumption::from_json(item));
    }
    return out;
  }

  // Cas 1 : tableau racine [ {...}, {...} ]
  if (root.is_array()) {
    for (const auto& item : root) {
      out.push_back(DailyConsumption::from_json(item));
    }
    return out;
  }

  // Cas 2 : enveloppe { "data": [ ... ] } (à adapter au vrai schéma)
  if (root.is_object() && root.contains("data") && root["data"].is_array()) {
    for (const auto& item : root["data"]) {
      out.push_back(DailyConsumption::from_json(item));
    }
    return out;
  }

  throw std::runtime_error(
      "TeregaApi: format JSON non reconnu — adapter parse_response au schéma API");
}

std::vector<DailyConsumption> TeregaApi::fetch_daily_consumptions(const std::string& url) {
  const std::string body = client_->get(url);
  return parse_response(body);
}

} // namespace gas
