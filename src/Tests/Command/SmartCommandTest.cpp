#include "../../Command/SmartCommand.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SmartCommand Tests", "[SmartCommand]")
{
    SECTION("Apply create command")
    {
        SmartCommand smartCommand;
        Command command("create", { "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "add");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply create list command")
    {
        SmartCommand smartCommand;
        Command command("create", { "list", "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "add", "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply lower command")
    {
        SmartCommand smartCommand;
        Command command("lower", { "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "decrease");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply low command")
    {
        SmartCommand smartCommand;
        Command command("low", { "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "decrease");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply upper command")
    {
        SmartCommand smartCommand;
        Command command("upper", { "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "increase");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply up command")
    {
        SmartCommand smartCommand;
        Command command("up", { "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "increase");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply add command")
    {
        SmartCommand smartCommand;
        Command command("add", { "list", "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "add", "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply remove command")
    {
        SmartCommand smartCommand;
        Command command("remove", { "list", "item" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "remove", "item" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply list command with 'current' argument")
    {
        SmartCommand smartCommand;
        Command command("current", {}, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments() == std::vector<std::string>{ "current" });
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply list command without transformation")
    {
        SmartCommand smartCommand;
        Command command("list", {}, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "list");
        REQUIRE(result.getArguments().empty());
        REQUIRE(result.getOptions().empty());
    }

    SECTION("Apply commands with create alias -> add autocomplete")
    {
        SmartCommand smartCommand;
        Command command("commands", { "create" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "commands");
        REQUIRE(result.getArguments().at(0) == "add");
    }

    SECTION("Apply commands with upper alias -> increase autocomplete")
    {
        SmartCommand smartCommand;
        Command command("commands", { "upper" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "commands");
        REQUIRE(result.getArguments().at(0) == "increase");
    }

    SECTION("Apply commands with unknown command -> unchanged")
    {
        SmartCommand smartCommand;
        Command command("commands", { "use" }, {});
        Command result = smartCommand.apply(command);
        REQUIRE(result.getName() == "commands");
        REQUIRE(result.getArguments().at(0) == "use");
    }
}
