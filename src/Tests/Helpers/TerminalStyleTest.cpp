#include "../../Helpers/TerminalStyle.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TerminalStyleTest", "[TerminalStyle]")
{
    SECTION("color codes")
    {
        REQUIRE(TerminalStyle::getBashCode("RED") == "\033[31m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_RED") == "\033[91m");
        REQUIRE(TerminalStyle::getBashCode("GREEN") == "\033[32m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_GREEN") == "\033[92m");
        REQUIRE(TerminalStyle::getBashCode("YELLOW") == "\033[33m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_YELLOW") == "\033[93m");
        REQUIRE(TerminalStyle::getBashCode("BLUE") == "\033[34m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_BLUE") == "\033[94m");
        REQUIRE(TerminalStyle::getBashCode("MAGENTA") == "\033[35m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_MAGENTA") == "\033[95m");
        REQUIRE(TerminalStyle::getBashCode("CYAN") == "\033[36m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_CYAN") == "\033[96m");
        REQUIRE(TerminalStyle::getBashCode("ORANGE") == "\033[38;5;208m");
    }

    SECTION("gray shades")
    {
        REQUIRE(TerminalStyle::getBashCode("GRAY") == "\033[90m");
        REQUIRE(TerminalStyle::getBashCode("LIGHT_GRAY") == "\033[37m");
        REQUIRE(TerminalStyle::getBashCode("DARK_GRAY") == "\033[38;5;242m");
        REQUIRE(TerminalStyle::getBashCode("WHITE") == "\033[39m");
    }

    SECTION("formatting styles")
    {
        REQUIRE(TerminalStyle::getBashCode("BOLD") == "\033[1m");
        REQUIRE(TerminalStyle::getBashCode("ITALIC") == "\033[3m");
        REQUIRE(TerminalStyle::getBashCode("RESET") == "\033[0m");
    }

    SECTION("unknown style returns empty string")
    {
        REQUIRE(TerminalStyle::getBashCode("UNKNOWN") == "");
        REQUIRE(TerminalStyle::getBashCode("") == "");
        REQUIRE(TerminalStyle::getBashCode("red") == "");
        REQUIRE(TerminalStyle::getBashCode("bold") == "");
    }
}
