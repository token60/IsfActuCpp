#include "gas_analytics/csv_exporter.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace gas {

namespace {

std::string csv_escape(const std::string& s) {
  bool needs_quotes = false;
  for (char c : s) {
    if (c == ',' || c == '"' || c == '\n' || c == '\r') {
      needs_quotes = true;
      break;
    }
  }
  if (!needs_quotes) {
    return s;
  }

  std::string out;
  out.reserve(s.size() + 2);
  out.push_back('"');
  for (char c : s) {
    if (c == '"') {
      out.push_back('"');
      out.push_back('"');
    } else {
      out.push_back(c);
    }
  }
  out.push_back('"');
  return out;
}

std::string hour_header(int h) {
  std::ostringstream oss;
  oss << std::setw(2) << std::setfill('0') << h << "_00";
  return oss.str();
}

} // namespace

void CsvExporter::export_daily_series(const std::string& path,
                                      const std::vector<DailyConsumption>& data) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("CsvExporter: impossible d'ouvrir '" + path + "'");
  }

  out << "date,operateur,secteur_activite,consommation_journaliere_mwh_pcs";
  for (int h = 0; h < 24; ++h) {
    out << ',' << hour_header(h);
  }
  out << '\n';

  out.setf(std::ios::fixed);
  out << std::setprecision(6);

  for (const auto& d : data) {
    out << csv_escape(d.date()) << ',';
    out << csv_escape(d.operateur()) << ',';
    out << csv_escape(d.secteur()) << ',';
    out << d.total_consumption_mwh();
    for (double v : d.hourly().mwh) {
      out << ',' << v;
    }
    out << '\n';
  }
}

void CsvExporter::export_statistics_summary(const std::string& path,
                                            const StatisticsEngine& stats) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("CsvExporter: impossible d'ouvrir '" + path + "'");
  }

  out << "statistique,valeur\n";
  out << "mean_daily_total," << stats.mean_daily_total() << "\n";
  out << "std_dev_daily_total," << stats.std_dev_daily_total() << "\n";
  out << "coefficient_of_variation_daily_total," << stats.coefficient_of_variation_daily_total() << "\n";
  out << "mean_peak_to_valley_ratio," << stats.mean_peak_to_valley_ratio() << "\n";
  out << "mean_night_to_day_ratio," << stats.mean_night_to_day_ratio() << "\n";
}

void CsvExporter::export_moving_average_totals(
    const std::string& path,
    const std::vector<DailyConsumption>& data,
    const std::vector<double>& moving_average,
    std::size_t window) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("CsvExporter: impossible d'ouvrir '" + path + "'");
  }

  out << "date,moving_average_total,window\n";
  for (std::size_t i = 0; i < moving_average.size(); ++i) {
    out << csv_escape(data[i].date().substr(0, 10)) << ',' << moving_average[i]
        << ',' << window << "\n";
  }
}

void CsvExporter::export_anomalies(const std::string& path,
                                   double k,
                                   const std::vector<DailyConsumption>& data,
                                   const std::vector<Anomaly>& anomalies) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("CsvExporter: impossible d'ouvrir '" + path + "'");
  }

  out << "date,total_mwh,z_score,k\n";
  for (const auto& a : anomalies) {
    const auto& d = data[a.index];
    out << csv_escape(d.date().substr(0, 10)) << ',' << a.value << ',' << a.z_score
        << ',' << k << "\n";
  }
}

void CsvExporter::export_group_stats(const std::string& path,
                                     const std::string& category,
                                     const std::map<std::string, GroupStats>& group) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("CsvExporter: impossible d'ouvrir '" + path + "'");
  }

  out << "group,n,mean_total,category\n";
  for (const auto& [k, st] : group) {
    out << csv_escape(k) << ',' << st.n << ',' << st.mean_total << ','
        << csv_escape(category) << "\n";
  }
}

} // namespace gas

