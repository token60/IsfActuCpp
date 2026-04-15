#include "gas_analytics/http_client.hpp"

#include <curl/curl.h>

#include <mutex>
#include <sstream>
#include <stdexcept>
using namespace std;


namespace gas {

namespace {

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
  auto* out = static_cast<string*>(userdata);
  out->append(ptr, size * nmemb);
  return size * nmemb;
}

void ensure_curl_global_init() {
  static std::once_flag once;
  std::call_once(once, [] {
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
      throw std::runtime_error("curl_global_init a échoué");
    }
  });
}

} // namespace

string HttpClient::get(const string& url) {
  ensure_curl_global_init();

  CURL* curl = curl_easy_init();
  if (!curl) {
    throw runtime_error("curl_easy_init a échoué");
  }

  string body;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    curl_easy_cleanup(curl);
    throw runtime_error(string("curl_easy_perform: ") + curl_easy_strerror(res));
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  curl_easy_cleanup(curl);

  if (http_code < 200 || http_code >= 300) {
    ostringstream oss;
    oss << "HTTP " << http_code << " pour " << url;
    throw runtime_error(oss.str());
  }

  return body;
}

} // namespace gas
