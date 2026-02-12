#include <catch2/catch_all.hpp>
#include "../include/macros.h"

TEST_CASE("Macros: Constantes et valeurs", "[macros]") {
    SECTION("Version") {
        REQUIRE(std::string(CPP_ENGINE_VERSION) == "1.0.0");
    }

    SECTION("Nom de l'application") {
        REQUIRE(std::string(CPP_ENGINE_NAME) == "CppEngine");
    }

    SECTION("Valeurs numériques") {
        REQUIRE(CPP_ENGINE_MAX_THREADS > 0);
    }
}

TEST_CASE("Macros: Utilitaires", "[macros]") {
    SECTION("MIN et MAX") {
        REQUIRE(MIN(1, 2) == 1);
        REQUIRE(MAX(1, 2) == 2);
        REQUIRE(MIN(-5, 5) == -5);
        REQUIRE(MAX(-5, 5) == 5);
    }

    SECTION("ARRAY_SIZE") {
        int arr[10];
        REQUIRE(ARRAY_SIZE(arr) == 10);
    }
}

