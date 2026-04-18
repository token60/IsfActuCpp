# gas_analytics (C++ / Meson)

Projet C++ (L3 Actuariat) : **récupérer des données ouvertes de consommation de gaz industriel**, les modéliser en objets, puis calculer quelques statistiques utiles (moyenne, écart-type, ratios, moyenne glissante).

## Données (API publique)

Les données proviennent d’une API Opendatasoft (JSON) :

- `https://odre.opendatasoft.com/api/explore/v2.1/catalog/datasets/conso-journa-industriel-grtgazterega/records?limit=20`

La réponse est une enveloppe du type :

- `total_count`
- `results`: tableau d’enregistrements, avec notamment :
  - `date`
  - `operateur`
  - `secteur_activite`
  - `consommation_journaliere_mwh_pcs`
  - champs horaires `00_00`, `01_00`, …, `23_00`

## Architecture (classes)

- **`HttpClient`** (`include/gas_analytics/http_client.hpp`, `src/http_client.cpp`)
  - encapsule **libcurl**
  - expose un `GET` qui renvoie le corps de réponse en `std::string`

- **`HourlyRecord`** (`include/gas_analytics/hourly_record.hpp`)
  - stocke les **24 valeurs horaires**
  - calcule : total, total jour/nuit, pic, creux, ratios (pic/creux, nuit/jour)

- **`DailyConsumption`** (`include/gas_analytics/daily_consumption.hpp`, `src/daily_consumption.cpp`)
  - représente **une journée**
  - construit l’objet à partir d’un JSON (mapping sur les champs réels ci-dessus)

- **`TeregaApi`** (`include/gas_analytics/terega_api.hpp`, `src/terega_api.cpp`)
  - télécharge le JSON via `HttpClient`
  - parse `results` et renvoie un `vector<DailyConsumption>`

- **`StatisticsEngine`** (`include/gas_analytics/statistics_engine.hpp`, `src/statistics_engine.cpp`)
  - calcule des statistiques sur une série de `DailyConsumption` :
    - moyenne / écart-type / coefficient de variation des totaux journaliers
    - moyenne des ratios pic/creux et nuit/jour
    - moyenne glissante sur les totaux

## Organisation du dépôt

```text
include/gas_analytics/   # headers des classes
src/                    # implémentations + main.cpp
subprojects/            # wrap Meson (nlohmann/json)
meson.build             # config build
```

## Dépendances

- **Meson** + **Ninja** (build)
- **libcurl** (HTTP)
- **nlohmann/json** (JSON) via wrap Meson (`subprojects/nlohmann_json.wrap`)

## Compiler et exécuter (WSL / Ubuntu)

Depuis la racine du projet :

```bash
sudo apt update
sudo apt install -y meson ninja-build pkg-config cmake libcurl4-openssl-dev

meson setup build
meson compile -C build
./build/gas_analytics
```

## Exécution

Le programme :

- appelle l’URL publique (limite 20 enregistrements)
- propose un **menu** en console pour afficher les résultats de façon structurée :
  - table des enregistrements (colonnes alignées)
  - statistiques globales (moyenne, écart-type, coefficient de variation, ratios)
  - moyenne glissante (fenêtre paramétrable)
  - détection d’anomalies (seuil \(k\) écarts-types)
  - comparaison par opérateur / secteur (moyenne des totaux)
  - export CSV de la série (métadonnées + 24 valeurs horaires)

## À faire (extensions possibles)

- détection d’anomalies (jours à \(k\) écarts-types)
- export CSV / rapport texte
- ajout de données météo et corrélations
- comparaison par région / secteur / opérateur

## Build sous Windows (PowerShell)

Si `meson` n’est pas trouvé, tu peux l’installer via Python :

```powershell
py -m pip install --user meson ninja
```

Puis (à la racine du projet) :

```powershell
meson setup build
meson compile -C build
.\build\gas_analytics.exe
```

