#include <catch2/catch_test_macros.hpp>
#include "../../Command/CommandList.h"

TEST_CASE("CommandList Tests", "[CommandList]")
{
    SECTION("Check if command exists") {
        CommandList commandList;
        REQUIRE(commandList.isValid("commands") == true);
        REQUIRE(commandList.isValid("add") == true);
        REQUIRE(commandList.isValid("truc") == false);
    }
}