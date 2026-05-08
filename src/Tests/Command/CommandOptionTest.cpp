#include "../../Command/CommandOption.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandOptionTest", "[CommandOption]")
{
    CommandOption commandOption;

    SECTION("isValidOption with short options")
    {
        REQUIRE(commandOption.isValidOption("l") == true);
        REQUIRE(commandOption.isValidOption("p") == true);
        REQUIRE(commandOption.isValidOption("s") == true);
        REQUIRE(commandOption.isValidOption("A") == true);
        REQUIRE(commandOption.isValidOption("D") == true);
        REQUIRE(commandOption.isValidOption("d") == true);
        REQUIRE(commandOption.isValidOption("f") == true);
        REQUIRE(commandOption.isValidOption("h") == true);
        REQUIRE(commandOption.isValidOption("v") == true);
        REQUIRE(commandOption.isValidOption("c") == true);
        REQUIRE(commandOption.isValidOption("i") == true);
        REQUIRE(commandOption.isValidOption("Q") == true);
    }

    SECTION("isValidOption with long options")
    {
        REQUIRE(commandOption.isValidOption("list") == true);
        REQUIRE(commandOption.isValidOption("priority") == true);
        REQUIRE(commandOption.isValidOption("status") == true);
        REQUIRE(commandOption.isValidOption("archived") == true);
        REQUIRE(commandOption.isValidOption("deleted") == true);
        REQUIRE(commandOption.isValidOption("described") == true);
        REQUIRE(commandOption.isValidOption("force") == true);
        REQUIRE(commandOption.isValidOption("help") == true);
        REQUIRE(commandOption.isValidOption("version") == true);
        REQUIRE(commandOption.isValidOption("config") == true);
        REQUIRE(commandOption.isValidOption("deadline") == true);
        REQUIRE(commandOption.isValidOption("no-quit") == true);
    }

    SECTION("isValidOption with invalid options")
    {
        REQUIRE(commandOption.isValidOption("x") == false);
        REQUIRE(commandOption.isValidOption("z") == false);
        REQUIRE(commandOption.isValidOption("unknown") == false);
        REQUIRE(commandOption.isValidOption("") == false);
    }

    SECTION("isOptionWithValue")
    {
        REQUIRE(commandOption.isOptionWithValue("l") == true);
        REQUIRE(commandOption.isOptionWithValue("list") == true);
        REQUIRE(commandOption.isOptionWithValue("p") == true);
        REQUIRE(commandOption.isOptionWithValue("priority") == true);
        REQUIRE(commandOption.isOptionWithValue("s") == true);
        REQUIRE(commandOption.isOptionWithValue("status") == true);
        REQUIRE(commandOption.isOptionWithValue("i") == true);
        REQUIRE(commandOption.isOptionWithValue("deadline") == true);

        REQUIRE(commandOption.isOptionWithValue("A") == false);
        REQUIRE(commandOption.isOptionWithValue("archived") == false);
        REQUIRE(commandOption.isOptionWithValue("D") == false);
        REQUIRE(commandOption.isOptionWithValue("deleted") == false);
        REQUIRE(commandOption.isOptionWithValue("d") == false);
        REQUIRE(commandOption.isOptionWithValue("described") == false);
        REQUIRE(commandOption.isOptionWithValue("f") == false);
        REQUIRE(commandOption.isOptionWithValue("force") == false);
        REQUIRE(commandOption.isOptionWithValue("h") == false);
        REQUIRE(commandOption.isOptionWithValue("help") == false);
        REQUIRE(commandOption.isOptionWithValue("v") == false);
        REQUIRE(commandOption.isOptionWithValue("version") == false);
    }

    SECTION("shortToLongOption")
    {
        REQUIRE(commandOption.shortToLongOption("l") == "list");
        REQUIRE(commandOption.shortToLongOption("p") == "priority");
        REQUIRE(commandOption.shortToLongOption("s") == "status");
        REQUIRE(commandOption.shortToLongOption("A") == "archived");
        REQUIRE(commandOption.shortToLongOption("D") == "deleted");
        REQUIRE(commandOption.shortToLongOption("d") == "described");
        REQUIRE(commandOption.shortToLongOption("f") == "force");
        REQUIRE(commandOption.shortToLongOption("h") == "help");
        REQUIRE(commandOption.shortToLongOption("v") == "version");
        REQUIRE(commandOption.shortToLongOption("c") == "config");
        REQUIRE(commandOption.shortToLongOption("i") == "deadline");
        REQUIRE(commandOption.shortToLongOption("Q") == "no-quit");
    }

    SECTION("shortToLongOption returns input when not a short option")
    {
        REQUIRE(commandOption.shortToLongOption("list") == "list");
        REQUIRE(commandOption.shortToLongOption("unknown") == "unknown");
    }

    SECTION("getLongOptionName with short options")
    {
        REQUIRE(commandOption.getLongOptionName("l") == "list");
        REQUIRE(commandOption.getLongOptionName("p") == "priority");
        REQUIRE(commandOption.getLongOptionName("A") == "archived");
    }

    SECTION("getLongOptionName with long options")
    {
        REQUIRE(commandOption.getLongOptionName("list") == "list");
        REQUIRE(commandOption.getLongOptionName("priority") == "priority");
        REQUIRE(commandOption.getLongOptionName("archived") == "archived");
    }

    SECTION("getLongOptionName throws on invalid option")
    {
        REQUIRE_THROWS_AS(commandOption.getLongOptionName("x"), std::invalid_argument);
        REQUIRE_THROWS_AS(commandOption.getLongOptionName("unknown"), std::invalid_argument);
        REQUIRE_THROWS_AS(commandOption.getLongOptionName(""), std::invalid_argument);
    }
}
