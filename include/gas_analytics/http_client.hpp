#pragma once

#include <string>

namespace gas {

/// Encapsule libcurl : requêtes GET, corps de réponse en `std::string`.
class HttpClient {
public:
  HttpClient() = default;

  /// Effectue un GET sur `url`. Lève `std::runtime_error` en cas d’échec réseau / HTTP.
  std::string get(const std::string& url);
};

} // namespace gas
