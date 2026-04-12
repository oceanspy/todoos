#include "../../Command/CommandRegistry.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandRegistry Tests", "[CommandRegistry]")
{
    SECTION("Check if command exists")
    {
        CommandRegistry commandList;
        REQUIRE(commandList.isValid("commands") == true);
        REQUIRE(commandList.isValid("add") == true);
        REQUIRE(commandList.isValid("truc") == false);
    }
}
