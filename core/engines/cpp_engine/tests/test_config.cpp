#include <catch2/catch_all.hpp>
#include "../include/utils/config.h"

TEST_CASE("Config: Chargement par défaut", "[config]") {
    config::Config cfg;

    SECTION("Valeurs par défaut") {
        REQUIRE(cfg.getValue("app_name") == "CppEngine");
        REQUIRE(cfg.getValue("version") == "1.0.0");
    }
}

TEST_CASE("Config: Lecture/Écriture", "[config]") {
    config::Config cfg;

    SECTION("Écriture et lecture") {
        cfg.setValue("max_threads", "8");
        REQUIRE(cfg.getValue("max_threads") == "8");
    }

    SECTION("Lecture d'une clé inexistante") {
        REQUIRE(cfg.getValue("unknown_key") == "");
    }
}

TEST_CASE("Config: Chargement depuis fichier", "[config]") {
    config::Config cfg;

    SECTION("Fichier existant") {
        bool loaded = cfg.loadFromFile("../resources/config_test.ini");
        REQUIRE(loaded);
        REQUIRE(cfg.getValue("app_name") == "CppEngineTest");
    }

    SECTION("Fichier inexistant") {
        bool loaded = cfg.loadFromFile("../resources/nonexistent.ini");
        REQUIRE_FALSE(loaded);
    }
}

