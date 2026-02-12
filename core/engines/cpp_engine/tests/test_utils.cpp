#include <catch2/catch_all.hpp>
#include "../include/utils/utils.h"
#include <vector>
#include <string>

TEST_CASE("Utils: toUpper()", "[utils]") {
    REQUIRE(utils::toUpper("hello") == "HELLO");
    REQUIRE(utils::toUpper("WORLD") == "WORLD");
    REQUIRE(utils::toUpper("TesT123") == "TEST123");
}

TEST_CASE("Utils: trim()", "[utils]") {
    REQUIRE(utils::trim("   hello   ") == "hello");
    REQUIRE(utils::trim("data") == "data");
    REQUIRE(utils::trim("\t  code\t ") == "code");
}

