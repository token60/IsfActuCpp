# gas_analytics (C++ / Meson)

## Contributeurs

Étudiants en L3 Actuariat :

- MBEYA NDONGO JOEL HYACINTHE
- ABOU YAPO ESDRAS ELISEE
- KOUADIO KOUAKOU ELYSEE

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

## Fonctionnalités implémentées

Selon le devoir, le projet implémente les fonctionnalités suivantes :

- **Récupération de données** : Utilisation de l'API Opendatasoft pour récupérer des données JSON de consommation de gaz industriel.
- **Modélisation des données** : Création d'objets C++ pour représenter les enregistrements quotidiens et horaires.
- **Calculs statistiques** :
  - Moyenne, écart-type et coefficient de variation des totaux journaliers.
  - Ratios pic/creux et nuit/jour.
  - Moyenne glissante sur les totaux journaliers.
- **Détection d'anomalies** : Identification des jours dont la consommation s'écarte de plus de k écarts-types.
- **Groupement et comparaison** : Moyenne des totaux par opérateur et par secteur d'activité.
- **Export CSV** : Export de la série complète (métadonnées + 24 valeurs horaires) et des résultats calculés (statistiques, anomalies, moyennes groupées).
- **Interface utilisateur** : Menu interactif en console pour afficher les résultats et exporter en CSV.

## Hiérarchie des classes

La hiérarchie des classes suit une architecture modulaire :

- **HttpClient** : Classe de base pour les requêtes HTTP (encapsule libcurl).
- **HourlyRecord** : Structure pour les données horaires (24 valeurs).
- **DailyConsumption** : Classe dérivée pour une journée complète (hérite conceptuellement de HourlyRecord via composition).
- **TeregaApi** : Classe pour l'interaction avec l'API (utilise HttpClient et DailyConsumption).
- **StatisticsEngine** : Classe pour les calculs statistiques (opère sur vector<DailyConsumption>).
- **AnomalyDetector** : Classe pour la détection d'anomalies (utilise StatisticsEngine).
- **CsvExporter** : Classe statique pour l'export CSV (utilise toutes les autres classes).
- **Grouping** : Fonctions pour le groupement par opérateur/secteur.

L'organisation suit le principe de séparation des responsabilités : réseau, données, calculs, export.

Puis (à la racine du projet) :

```powershell
meson setup build
meson compile -C build
.\build\gas_analytics.exe
```

