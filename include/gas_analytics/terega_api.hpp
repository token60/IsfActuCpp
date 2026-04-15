#pragma once

#include "gas_analytics/daily_consumption.hpp"
#include "gas_analytics/http_client.hpp"

#include <vector>

using namespace std;

namespace gas {

/// Utilise `HttpClient` pour interroger l’API et produire des `DailyConsumption`.
/// L’URL exacte et le format de la réponse dépendent de la doc Teréga : compléter
/// `parse_response` quand vous avez un exemple JSON réel.
class TeregaApi {
public:
  explicit TeregaApi(HttpClient& client);

  /// Exemple : charge une URL qui renvoie un tableau d’objets journée.
  std::vector<DailyConsumption> fetch_daily_consumptions(const std::string& url);

private:
  HttpClient* client_{};

  static std::vector<DailyConsumption> parse_response(const std::string& json_body);
};

} // namespace gas
