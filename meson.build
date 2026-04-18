project(
  'gas_analytics',
  'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17', 'warning_level=2'],
)

json_dep = dependency(
  'nlohmann_json',
  fallback: ['nlohmann_json', 'nlohmann_json_dep'],
)
curl_dep = dependency('libcurl')

inc = include_directories('include')

sources = files(
  'src/main.cpp',
  'src/daily_consumption.cpp',
  'src/http_client.cpp',
  'src/terega_api.cpp',
  'src/statistics_engine.cpp',
  'src/anomaly_detector.cpp',
  'src/csv_exporter.cpp',
  'src/grouping.cpp',
)

executable(
  'gas_analytics',
  sources,
  include_directories: inc,
  dependencies: [json_dep, curl_dep],
)
