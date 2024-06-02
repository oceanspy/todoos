#include <catch2/catch_test_macros.hpp>
#include "../../Command/CommandValidation.h"
#include "../../Command/CommandOption.h"

TEST_CASE("CommandValidation Tests", "[CommandValidation]") {
    SECTION("If command is empty, should return show command") {
        const char* argv[] = {"./program"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.isCommandEmpty() == false);
        REQUIRE(commandValidation.getCommandName() == "show");
    }

    SECTION("Get command name") {
        const char* argv[] = {"./program", "ls"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "ls");
    }

    SECTION("Get command: one argument, no option") {
        const char* argv[] = {"./program", "ls", "add"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
    }

    SECTION("Get command: no arguments, one option") {
        const char* argv[] = {"./program", "ls", "-l", "list"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: no arguments, one empty option") {
        const char* argv[] = {"./program", "ls", "-l"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", ""}});
    }

    SECTION("Get command: no arguments, one empty option") {
        const char* argv[] = {"./program", "ls", "--list"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", ""}});
    }

    SECTION("Get command: no arguments, one option -- long option name") {
        const char* argv[] = {"./program", "ls", "--list", "list"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: one argument, one option") {
        const char* argv[] = {"./program", "ls", "-l", "list", "add"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: one argument, one option -- different order") {
        const char* argv[] = {"./program", "ls", "add", "-l", "list"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: multiple arguments, one option") {
        const char* argv[] = {"./program", "ls", "add", "Value test", "-l", "list"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add", "Value test"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: multiple arguments, one option -- different order") {
        const char* argv[] = {"./program", "ls", "-l", "list", "add", "Value test"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add", "Value test"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: multiple arguments, one option -- different order") {
        const char* argv[] = {"./program", "ls", "-l", "list", "add", "Value", "test"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add", "Value", "test"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
    }

    SECTION("Get command: no arguments, one non-supported option") {
        const char* argv[] = {"./program", "ls", "-w", "test"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        REQUIRE_THROWS(commandValidation.make());
    }

    SECTION("Get command: add with dash") {
        const char* argv[] = {"./program", "add", "-- test"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"-- test"});
    }

    SECTION("Make command - options without values") {
        const char* argv[] = {"./program", "ls", "-l", "list"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "ls");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}});
        REQUIRE(commandValidation.getCommandArguments().empty() == true);
    }

    SECTION("Make command - options") {
        const char* argv[] = {"./program", "add", "- p truc"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"- p truc"});
    }

    SECTION("Make command - options") {
        const char* argv[] = {"./program", "add", "-p truc"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"-p truc"});
    }

    SECTION("Make command - options") {
        const char* argv[] = {"./program", "add", "-p", "truc"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"priority", "truc"}});
        REQUIRE(commandValidation.getCommandArguments().empty() == true);
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "find", "--archive", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "find", "chose", "--archive"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "find", "chose", "-a"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "find", "-a", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "--archive", "find", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "-a", "find", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: archive option") {
        const char* argv[] = {"./program", "--archive", "find", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"archive", ""}});
    }

    SECTION("Get command: delete option") {
        const char* argv[] = {"./program", "--delete", "find", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"delete", ""}});
    }

    SECTION("Get command: delete option") {
        const char* argv[] = {"./program", "-d", "find", "chose"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"chose"});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"delete", ""}});
    }

//    No other option actually implemented
//    SECTION("Get command: multiple arguments, multiple options") {
//        const char* argv[] = {"./program", "ls", "-l", "list", "add", "Value", "test", "-a", "all"};
//        int argc = sizeof(argv) / sizeof(argv[0]);
//        CommandValidation commandValidation(argc, argv);
//        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add", "Value", "test"});
//        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"}, {"all", "all"}});
//    }

    SECTION("Make command - value with line break") {
        const char* argv[] = {"./program", "add", "value\ntest"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions().empty() == true);
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"valuetest"});
    }
}