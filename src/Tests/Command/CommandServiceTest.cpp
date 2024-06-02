#include <catch2/catch_test_macros.hpp>
#include "../../Command/CommandService.h"

TEST_CASE("CommandService Tests", "[CommandService]") {
    SECTION("Check if command has subcommand") {
        CommandList commandList;
        CommandShortcut commandShortcutService = CommandShortcut();
        CommandOption commandOption = CommandOption();
        auto commandService = CommandService(commandList, commandOption);
        Command command1("ls", {}, {});
        Command command2("ls", {"-l", "file.txt"}, {});
        REQUIRE(commandService.hasSubCommand(command1) == false);
        REQUIRE(commandService.hasSubCommand(command2) == true);
    }

    SECTION("Get subcommand") {
        CommandList commandList;
        CommandShortcut commandShortcutService = CommandShortcut();
        CommandOption commandOption = CommandOption();
        auto commandService = CommandService(commandList, commandOption);
        Command command1("commands", {}, {});
        Command command2("commands", {"add", "test"}, {});
        Command command3("commands", {"priority", "aaaa", "high"}, {});
        Command command4("commands", {"add", "test"}, {{"list", "test"}});
        Command command5("commands", {"priority", "aaaa", "high"}, {{"list", "test"}});
        REQUIRE(commandService.hasSubCommand(command1) == false);
        REQUIRE(commandService.hasSubCommand(command2) == true);
        REQUIRE(commandService.getSubCommand(command1).getName() == "");
        REQUIRE(commandService.getSubCommand(command2).getName() == "add");
        REQUIRE(commandService.getSubCommand(command2).getArguments() == std::vector<std::string>{"test"});
        REQUIRE(commandService.getSubCommand(command2).getOptions().empty() == true);
        REQUIRE(commandService.getSubCommand(command3).getName() == "priority");
        REQUIRE(commandService.getSubCommand(command3).getArguments() == std::vector<std::string>{"aaaa", "high"});
        REQUIRE(commandService.getSubCommand(command3).getOptions().empty() == true);
        REQUIRE(commandService.getSubCommand(command4).getName() == "add");
        REQUIRE(commandService.getSubCommand(command4).getArguments() == std::vector<std::string>{"test"});
        REQUIRE(commandService.getSubCommand(command4).getOptions() == std::map<std::string, std::string>{{"list", "test"}});
        REQUIRE(commandService.getSubCommand(command5).getName() == "priority");
        REQUIRE(commandService.getSubCommand(command5).getArguments() == std::vector<std::string>{"aaaa", "high"});
        REQUIRE(commandService.getSubCommand(command5).getOptions() == std::map<std::string, std::string>{{"list", "test"}});
    }
}

