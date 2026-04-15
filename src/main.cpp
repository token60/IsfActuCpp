#include "gas_analytics/statistics_engine.hpp"
#include "gas_analytics/terega_api.hpp"
#include "gas_analytics/http_client.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main() {
  cout << "gas_analytics — lecture API Opendatasoft\n";

  const string url =
      "https://odre.opendatasoft.com/api/explore/v2.1/catalog/datasets/"
      "conso-journa-industriel-grtgazterega/records?limit=20";

  try {
    gas::HttpClient http;
    gas::TeregaApi api(http);
    vector<gas::DailyConsumption> series = api.fetch_daily_consumptions(url);

    cout << "Nb enregistrements: " << series.size() << '\n';
    for (const auto& d : series) {
      const auto [h_peak, v_peak] = d.hourly().peak_hour();
      cout << d.date() << " | " << d.operateur() << " | " << d.secteur()
           << " | total=" << d.total_consumption_mwh() << " MWh PCS"
           << " | pic=" << v_peak << " (h=" << h_peak << ")\n";
    }

    gas::StatisticsEngine stats(std::move(series));
    cout << "Moyenne totaux journaliers: " << stats.mean_daily_total() << " MWh\n";
    cout << "Écart-type: " << stats.std_dev_daily_total() << '\n';
    cout << "Moyenne ratio nuit/jour: " << stats.mean_night_to_day_ratio() << '\n';
  } catch (const exception& e) {
    cerr << "Erreur: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
