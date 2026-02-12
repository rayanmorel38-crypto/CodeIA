#include <catch2/catch_all.hpp>
#include "../include/core/engine.h"

TEST_CASE("CPPEngine: Initialisation et état", "[core]") {
    cppengine::core::CPPEngine engine;

    SECTION("Santé du moteur") {
        REQUIRE(engine.is_healthy());
    }

    SECTION("Version du moteur") {
        REQUIRE(engine.get_version() == "1.0.0");
    }

    SECTION("Capacités du moteur") {
        std::string caps = engine.get_capabilities();
        REQUIRE(caps.find("image_generation") != std::string::npos);
    }
}

TEST_CASE("CPPEngine: Exécution de tâches", "[core]") {
    cppengine::core::CPPEngine engine;

    SECTION("Ajout et exécution de tâche") {
        engine.add_task("test_task", "demo");
        engine.execute_task("test_task");
        // Task execution is asynchronous, just check it doesn't crash
        REQUIRE(engine.is_healthy());
    }
}

