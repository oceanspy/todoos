#include "../../Command/SmartCommand.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SmartCommand Tests", "[SmartCommand]")
{
    SECTION("Apply add command")
    {
        SmartCommand smartCommand(Command("add", { "list", "item" }, {}));
        Command result = smartCommand.apply();
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "add", "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply remove command")
    {
        SmartCommand smartCommand(Command("remove", { "list", "item" }, {}));
        Command result = smartCommand.apply();
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "remove", "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply list command with one argument")
    {
        SmartCommand smartCommand(Command("list", { "name" }, {}));
        Command result = smartCommand.apply();
        REQUIRE(result.getName() == "use");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "name" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply list command with 'current' argument")
    {
        SmartCommand smartCommand(Command("current", {}, {}));
        Command result = smartCommand.apply();
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "current" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply list command without transformation")
    {
        SmartCommand smartCommand(Command("list", {}, {}));
        Command result = smartCommand.apply();
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments().empty());
        REQUIRE(result.getOptions().empty());
    }
}
