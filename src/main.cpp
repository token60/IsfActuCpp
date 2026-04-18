#include "gas_analytics/statistics_engine.hpp"
#include "gas_analytics/terega_api.hpp"
#include "gas_analytics/http_client.hpp"
#include "gas_analytics/anomaly_detector.hpp"
#include "gas_analytics/csv_exporter.hpp"
#include "gas_analytics/grouping.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

using namespace std;

namespace {

void print_line(char c = '-', int n = 100) {
  for (int i = 0; i < n; ++i) {
    cout << c;
  }
  cout << '\n';
}

template <class T>
T prompt_value(const string& label) {
  while (true) {
    cout << label;
    T v{};
    if (cin >> v) {
      return v;
    }
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Entrée invalide. Réessaie.\n";
  }
}

string prompt_line(const string& label) {
  cout << label;
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  string s;
  getline(cin, s);
  return s;
}

void print_series_table(const vector<gas::DailyConsumption>& series, size_t max_rows) {
  cout << left << setw(12) << "date" << " | " << setw(10) << "operateur"
       << " | " << setw(18) << "secteur" << " | " << right << setw(12) << "total(MWh)"
       << " | " << setw(10) << "pic(MWh)" << " | " << setw(4) << "h" << '\n';
  print_line('-', 78);

  const size_t n = series.size();
  const size_t shown = min(max_rows, n);
  cout.setf(ios::fixed);
  cout << setprecision(3);
  for (size_t i = 0; i < shown; ++i) {
    const auto& d = series[i];
    const auto [h_peak, v_peak] = d.hourly().peak_hour();
    cout << left << setw(12) << d.date().substr(0, 10) << " | " << setw(10)
         << d.operateur().substr(0, 10) << " | " << setw(18) << d.secteur().substr(0, 18)
         << " | " << right << setw(12) << d.total_consumption_mwh() << " | " << setw(10)
         << v_peak << " | " << setw(4) << h_peak << '\n';
  }
  if (shown < n) {
    cout << "... (" << (n - shown) << " lignes non affichées)\n";
  }
}

void print_group_table(const map<string, gas::GroupStats>& g, const string& title) {
  cout << title << '\n';
  cout << left << setw(28) << "groupe" << " | " << right << setw(6) << "n"
       << " | " << setw(14) << "moy_total" << '\n';
  print_line('-', 56);
  cout.setf(ios::fixed);
  cout << setprecision(3);
  for (const auto& [k, st] : g) {
    cout << left << setw(28) << k.substr(0, 28) << " | " << right << setw(6) << st.n
         << " | " << setw(14) << st.mean_total << '\n';
  }
}

void print_menu() {
  print_line('=');
  cout << "Menu\n";
  cout << " 1) Lister les enregistrements (table)\n";
  cout << " 2) Statistiques globales\n";
  cout << " 3) Moyenne glissante (totaux)\n";
  cout << " 4) Détection d'anomalies (k écarts-types)\n";
  cout << " 5) Comparer (moyenne des totaux) par opérateur / secteur\n";
  cout << " 6) Export CSV (série complète)\n";
  cout << " 7) Exporter résultats de menu en CSV\n";
  cout << " 0) Quitter\n";
  print_line('=');
}

} // namespace

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

    while (true) {
      print_menu();
      const int choice = prompt_value<int>("Choix: ");
      if (choice == 0) {
        break;
      }

      if (choice == 1) {
        const size_t n = static_cast<size_t>(prompt_value<int>("Nb de lignes à afficher: "));
        print_series_table(series, n);
        continue;
      }

      if (choice == 2) {
        gas::StatisticsEngine stats(series);
        cout.setf(ios::fixed);
        cout << setprecision(6);
        cout << "Moyenne totaux journaliers: " << stats.mean_daily_total() << " MWh\n";
        cout << "Écart-type: " << stats.std_dev_daily_total() << '\n';
        cout << "Coeff. de variation: " << stats.coefficient_of_variation_daily_total() << '\n';
        cout << "Moyenne ratio pic/creux: " << stats.mean_peak_to_valley_ratio() << '\n';
        cout << "Moyenne ratio nuit/jour: " << stats.mean_night_to_day_ratio() << '\n';
        continue;
      }

      if (choice == 3) {
        const size_t window = static_cast<size_t>(prompt_value<int>("Fenêtre (>=1): "));
        gas::StatisticsEngine stats(series);
        const auto ma = stats.moving_average_totals(window);
        cout << "Moyenne glissante (sur les totaux journaliers)\n";
        cout << left << setw(12) << "date" << " | " << right << setw(14) << "MA_total\n";
        print_line('-', 32);
        cout.setf(ios::fixed);
        cout << setprecision(6);
        for (size_t i = 0; i < ma.size(); ++i) {
          cout << left << setw(12) << series[i].date().substr(0, 10) << " | " << right
               << setw(14) << ma[i] << '\n';
        }
        continue;
      }

      if (choice == 4) {
        const double k = prompt_value<double>("Seuil k (ex: 2.0): ");
        gas::AnomalyDetector det;
        const auto anomalies = det.detect(series, k);
        cout << "Anomalies (|z| > " << k << "): " << anomalies.size() << '\n';
        if (!anomalies.empty()) {
          cout << left << setw(12) << "date" << " | " << right << setw(12) << "total(MWh)"
               << " | " << setw(10) << "z-score\n";
          print_line('-', 40);
          cout.setf(ios::fixed);
          cout << setprecision(6);
          for (const auto& a : anomalies) {
            const auto& d = series[a.index];
            cout << left << setw(12) << d.date().substr(0, 10) << " | " << right << setw(12)
                 << a.value << " | " << setw(10) << a.z_score << '\n';
          }
        }
        continue;
      }

      if (choice == 5) {
        const int sub = prompt_value<int>("1=opérateur, 2=secteur: ");
        if (sub == 1) {
          print_group_table(gas::group_by_operateur_mean_total(series),
                            "Moyenne des totaux par opérateur");
        } else if (sub == 2) {
          print_group_table(gas::group_by_secteur_mean_total(series),
                            "Moyenne des totaux par secteur");
        } else {
          cout << "Choix inconnu.\n";
        }
        continue;
      }

      if (choice == 6) {
        const string path = prompt_line("Chemin fichier CSV (ex: out.csv): ");
        gas::CsvExporter::export_daily_series(path, series);
        cout << "Export OK: " << path << '\n';
        continue;
      }

      if (choice == 7) {
        cout << " 1) Statistiques globales\n";
        cout << " 2) Moyenne glissante\n";
        cout << " 3) Anomalies\n";
        cout << " 4) Moyenne des totaux par opérateur\n";
        cout << " 5) Moyenne des totaux par secteur\n";
        const int sub = prompt_value<int>("Choix export: ");
        const string path = prompt_line("Chemin fichier CSV (ex: out.csv): ");

        try {
          if (sub == 1) {
            gas::StatisticsEngine stats(series);
            gas::CsvExporter::export_statistics_summary(path, stats);
          } else if (sub == 2) {
            const size_t window = static_cast<size_t>(prompt_value<int>("Fenêtre (>=1): "));
            gas::StatisticsEngine stats(series);
            const auto ma = stats.moving_average_totals(window);
            gas::CsvExporter::export_moving_average_totals(path, series, ma, window);
          } else if (sub == 3) {
            const double k = prompt_value<double>("Seuil k (ex: 2.0): ");
            gas::AnomalyDetector det;
            const auto anomalies = det.detect(series, k);
            gas::CsvExporter::export_anomalies(path, k, series, anomalies);
          } else if (sub == 4) {
            const auto group = gas::group_by_operateur_mean_total(series);
            gas::CsvExporter::export_group_stats(path, "operateur", group);
          } else if (sub == 5) {
            const auto group = gas::group_by_secteur_mean_total(series);
            gas::CsvExporter::export_group_stats(path, "secteur", group);
          } else {
            cout << "Choix inconnu.\n";
            continue;
          }
          cout << "Export OK: " << path << '\n';
        } catch (const exception& e) {
          cerr << "Erreur export CSV: " << e.what() << '\n';
        }
        continue;
      }

      cout << "Choix inconnu.\n";
    }
  } catch (const exception& e) {
    cerr << "Erreur: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
