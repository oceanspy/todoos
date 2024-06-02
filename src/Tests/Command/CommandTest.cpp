#include <catch2/catch_test_macros.hpp>
#include "../../Command/Command.h"

TEST_CASE("Creating Command objects", "[Command]") {
    SECTION("Create a command with no arguments and options") {
        Command cmd("ls", {}, {});
        REQUIRE(cmd.getName() == "ls");
        REQUIRE(cmd.getArguments().empty());
        REQUIRE(cmd.getOptions().empty());
    }

    SECTION("Create a command with arguments but no options") {
        std::vector<std::string> args = {"file1", "file2"};
        Command cmd("cat", args, {});
        REQUIRE(cmd.getName() == "cat");
        REQUIRE(cmd.getArguments() == args);
        REQUIRE(cmd.getOptions().empty());
    }

    SECTION("Create a command with options but no arguments") {
        std::map<std::string, std::string> opts = {{"-l", "long"}, {"-a", "all"}};
        Command cmd("ls", {}, opts);
        REQUIRE(cmd.getName() == "ls");
        REQUIRE(cmd.getArguments().empty());
        REQUIRE(cmd.getOptions() == opts);
    }

    SECTION("Create a command with both arguments and options") {
        Command cmd("grep", {"pattern", "file"}, {{"-i", "ignore-case"}});
        REQUIRE(cmd.getName() == "grep");
        REQUIRE(cmd.getArguments() == std::vector<std::string>{"pattern", "file"});
        REQUIRE(cmd.getOptions() == std::map<std::string, std::string>{{"-i", "ignore-case"}});
    }
}
