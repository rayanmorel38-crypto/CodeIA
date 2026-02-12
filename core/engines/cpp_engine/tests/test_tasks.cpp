#include <catch2/catch_all.hpp>
#include "../include/tasks.h"
#include "../include/utils.h"

TEST_CASE("Tasks: Initialisation", "[tasks]") {
    init_tasks();
    REQUIRE(get_task_count() == 0);
}

TEST_CASE("Tasks: Ajout de tâches", "[tasks]") {
    init_tasks();

    SECTION("Ajout simple") {
        REQUIRE(add_task("Task 1"));
        REQUIRE(add_task("Task 2"));
        REQUIRE(get_task_count() == 2);
        REQUIRE(task_exists("Task 1"));
        REQUIRE(task_exists("Task 2"));
    }

    SECTION("Ajout doublon") {
        REQUIRE(add_task("Duplicate"));
        REQUIRE_FALSE(add_task("Duplicate"));
    }
}

TEST_CASE("Tasks: Complétion de tâches", "[tasks]") {
    init_tasks();
    add_task("Task A");
    add_task("Task B");

    SECTION("Compléter existante") {
        REQUIRE(complete_task("Task A"));
    }

    SECTION("Compléter inexistante") {
        REQUIRE_FALSE(complete_task("Task X"));
    }
}

TEST_CASE("Tasks: Vérification globale", "[tasks]") {
    init_tasks();
    add_task("Analyse");
    add_task("Compile");
    add_task("Test");

    REQUIRE(get_task_count() == 3);
    complete_task("Compile");

    REQUIRE(task_exists("Analyse"));
    REQUIRE(task_exists("Compile"));
    REQUIRE(task_exists("Test"));
}

