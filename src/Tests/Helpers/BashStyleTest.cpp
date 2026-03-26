#include "../../Helpers/BashStyle.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("BashStyleTest", "[BashStyle]")
{
    SECTION("color codes")
    {
        REQUIRE(BashStyle::getBashCode("RED") == "\033[31m");
        REQUIRE(BashStyle::getBashCode("LIGHT_RED") == "\033[91m");
        REQUIRE(BashStyle::getBashCode("GREEN") == "\033[32m");
        REQUIRE(BashStyle::getBashCode("LIGHT_GREEN") == "\033[92m");
        REQUIRE(BashStyle::getBashCode("YELLOW") == "\033[33m");
        REQUIRE(BashStyle::getBashCode("LIGHT_YELLOW") == "\033[93m");
        REQUIRE(BashStyle::getBashCode("BLUE") == "\033[34m");
        REQUIRE(BashStyle::getBashCode("LIGHT_BLUE") == "\033[94m");
        REQUIRE(BashStyle::getBashCode("MAGENTA") == "\033[35m");
        REQUIRE(BashStyle::getBashCode("LIGHT_MAGENTA") == "\033[95m");
        REQUIRE(BashStyle::getBashCode("CYAN") == "\033[36m");
        REQUIRE(BashStyle::getBashCode("LIGHT_CYAN") == "\033[96m");
        REQUIRE(BashStyle::getBashCode("ORANGE") == "\033[38;5;208m");
    }

    SECTION("gray shades")
    {
        REQUIRE(BashStyle::getBashCode("GRAY") == "\033[90m");
        REQUIRE(BashStyle::getBashCode("LIGHT_GRAY") == "\033[37m");
        REQUIRE(BashStyle::getBashCode("DARK_GRAY") == "\033[38;5;242m");
        REQUIRE(BashStyle::getBashCode("WHITE") == "\033[39m");
    }

    SECTION("formatting styles")
    {
        REQUIRE(BashStyle::getBashCode("BOLD") == "\033[1m");
        REQUIRE(BashStyle::getBashCode("ITALIC") == "\033[3m");
        REQUIRE(BashStyle::getBashCode("RESET") == "\033[0m");
    }

    SECTION("unknown style returns empty string")
    {
        REQUIRE(BashStyle::getBashCode("UNKNOWN") == "");
        REQUIRE(BashStyle::getBashCode("") == "");
        REQUIRE(BashStyle::getBashCode("red") == "");
        REQUIRE(BashStyle::getBashCode("bold") == "");
    }
}
