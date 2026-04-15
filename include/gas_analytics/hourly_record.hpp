#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <utility>

using namespace std;

namespace gas {

/// Profil horaire sur 24 tranches : index 0 = 00h–01h, …, 23 = 23h–00h.
struct HourlyRecord {
  std::array<double, 24> mwh{};

  /// Convention indicative : jour = 06h–22h (h ∈ [6, 21]), nuit = le reste.
  /// À aligner avec la définition métier du sujet / de l’API si besoin.
  static constexpr bool is_day_hour(int hour_index) {
    return hour_index >= 6 && hour_index < 22;
  }

  double total() const {
    double s = 0.0;
    for (double v : mwh) {
      s += v;
    }
    return s;
  }

  double total_day() const {
    double s = 0.0;
    for (int h = 0; h < 24; ++h) {
      if (is_day_hour(h)) {
        s += mwh[static_cast<std::size_t>(h)];
      }
    }
    return s;
  }

  double total_night() const { return total() - total_day(); }

  /// Retourne (index_heure, MWh) du pic.
  std::pair<int, double> peak_hour() const {
    auto it = std::max_element(mwh.begin(), mwh.end());
    int idx = static_cast<int>(it - mwh.begin());
    return {idx, *it};
  }

  /// Retourne (index_heure, MWh) du creux.
  std::pair<int, double> valley_hour() const {
    auto it = std::min_element(mwh.begin(), mwh.end());
    int idx = static_cast<int>(it - mwh.begin());
    return {idx, *it};
  }

  /// Ratio pic / creux sur la journée (évite division par zéro).
  double peak_to_valley_ratio() const {
    auto [_, peak] = peak_hour();
    auto [__, valley] = valley_hour();
    if (valley <= 0.0) {
      throw std::runtime_error("peak_to_valley_ratio: creux nul ou négatif");
    }
    return peak / valley;
  }

  /// Ratio consommation nuit / jour (évite division par zéro).
  double night_to_day_ratio() const {
    double d = total_day();
    if (d <= 0.0) {
      throw std::runtime_error("night_to_day_ratio: total jour nul ou négatif");
    }
    return total_night() / d;
  }
};

} // namespace gas
