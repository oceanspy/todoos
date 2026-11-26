#include "../../Command/CommandValidation.h"
#include "../../Command/CommandOption.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandValidation Tests", "[CommandValidation]")
{
    SECTION("If command is empty, should return show command")
    {
        const char* argv[] = { "./program" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.isCommandEmpty() == false);
        REQUIRE(commandValidation.getCommandName() == "show");
    }

    SECTION("Get command name")
    {
        const char* argv[] = { "./program", "ls" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "ls");
    }

    SECTION("Get command: one argument, no option")
    {
        const char* argv[] = { "./program", "ls", "add" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
    }

    SECTION("Get command: no arguments, one option")
    {
        const char* argv[] = { "./program", "ls", "-l", "list" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: no arguments, one empty option")
    {
        const char* argv[] = { "./program", "ls", "-l" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "" } });
    }

    SECTION("Get command: no arguments, one empty option")
    {
        const char* argv[] = { "./program", "ls", "--list" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "" } });
    }

    SECTION("Get command: no arguments, one option -- long option name")
    {
        const char* argv[] = { "./program", "ls", "--list", "list" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{});
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: one argument, one option")
    {
        const char* argv[] = { "./program", "ls", "-l", "list", "add" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: one argument, one option -- different order")
    {
        const char* argv[] = { "./program", "ls", "add", "-l", "list" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: multiple arguments, one option")
    {
        const char* argv[] = { "./program", "ls", "add", "Value test", "-l", "list" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add", "Value test" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: multiple arguments, one option -- different order")
    {
        const char* argv[] = { "./program", "ls", "-l", "list", "add", "Value test" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add", "Value test" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: multiple arguments, one option -- different order")
    {
        const char* argv[] = { "./program", "ls", "-l", "list", "add", "Value", "test" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add", "Value", "test" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
    }

    SECTION("Get command: no arguments, one non-supported option")
    {
        const char* argv[] = { "./program", "ls", "-w", "test" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        REQUIRE_THROWS(commandValidation.make());
    }

    SECTION("Get command: add with dash")
    {
        const char* argv[] = { "./program", "add", "-- test" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "-- test" });
    }

    SECTION("Make command - options without values")
    {
        const char* argv[] = { "./program", "ls", "-l", "list" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "ls");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "list", "list" } });
        REQUIRE(commandValidation.getCommandArguments().empty() == true);
    }

    SECTION("Make command - options")
    {
        const char* argv[] = { "./program", "add", "- p truc" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "- p truc" });
    }

    SECTION("Make command - options")
    {
        const char* argv[] = { "./program", "add", "-p truc" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{});
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "-p truc" });
    }

    SECTION("Make command - options")
    {
        const char* argv[] = { "./program", "add", "-p", "truc" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "priority", "truc" } });
        REQUIRE(commandValidation.getCommandArguments().empty() == true);
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "find", "--archived", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "find", "chose", "--archived" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "find", "chose", "-A" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "find", "-A", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "--archived", "find", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "-A", "find", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: archive option")
    {
        const char* argv[] = { "./program", "--archived", "find", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "archived", "" } });
    }

    SECTION("Get command: delete option")
    {
        const char* argv[] = { "./program", "--deleted", "find", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "deleted", "" } });
    }

    SECTION("Get command: delete option")
    {
        const char* argv[] = { "./program", "-D", "find", "chose" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "find");
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "chose" });
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "deleted", "" } });
    }

    SECTION("Get command: described option (long)")
    {
        const char* argv[] = { "./program", "--described", "show" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "show");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "described", "" } });
    }

    SECTION("Get command: described option (short)")
    {
        const char* argv[] = { "./program", "-d", "show" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.getCommandName() == "show");
        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{ { "described", "" } });
    }

    //    No other option actually implemented
    //    SECTION("Get command: multiple arguments, multiple options") {
    //        const char* argv[] = {"./program", "ls", "-l", "list", "add", "Value", "test", "-A", "all"};
    //        int argc = sizeof(argv) / sizeof(argv[0]);
    //        CommandValidation commandValidation(argc, argv);
    //        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{"add", "Value", "test"});
    //        REQUIRE(commandValidation.getCommandOptions() == std::map<std::string, std::string>{{"list", "list"},
    //        {"all", "all"}});
    //    }

    SECTION("Make command - value with line break")
    {
        const char* argv[] = { "./program", "add", "value\ntest" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        commandValidation.make();
        REQUIRE(commandValidation.make() == true);
        REQUIRE(commandValidation.getCommandName() == "add");
        REQUIRE(commandValidation.getCommandOptions().empty() == true);
        REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "valuetest" });
    }

    SECTION("Autocompletion: a partial option as last word is kept as an argument")
    {
        for (const char* partial : { "-", "--", "--pri", "-w" }) {
            const char* argv[] = { "./program", "commands", "add", "foo", partial };
            int argc = sizeof(argv) / sizeof(argv[0]);
            CommandOption commandOption = CommandOption();
            CommandValidation commandValidation(commandOption, argc, argv);
            REQUIRE_NOTHROW(commandValidation.make());
            REQUIRE(commandValidation.getCommandName() == "commands");
            REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ "add", "foo", partial });
        }
    }

    SECTION("Autocompletion: an invalid option that is not the last word still throws")
    {
        const char* argv[] = { "./program", "commands", "add", "-w", "foo" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        REQUIRE_THROWS(commandValidation.make());
    }

    SECTION("Outside autocompletion, a partial option as last word still throws")
    {
        const char* argv[] = { "./program", "add", "foo", "--pri" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        CommandOption commandOption = CommandOption();
        CommandValidation commandValidation(commandOption, argc, argv);
        REQUIRE_THROWS(commandValidation.make());
    }

    SECTION("Strict options: -x takes a short name only, --xxx a long name only")
    {
        auto parse = [](std::vector<const char*> args) {
            args.insert(args.begin(), "./program");
            CommandOption commandOption = CommandOption();
            CommandValidation commandValidation(commandOption, (int)args.size(), args.data());
            commandValidation.make();
            return commandValidation.getCommandOptions();
        };

        REQUIRE(parse({ "show", "-l", "work" }) == std::map<std::string, std::string>{ { "list", "work" } });
        REQUIRE(parse({ "show", "--list", "work" }) == std::map<std::string, std::string>{ { "list", "work" } });
        REQUIRE_THROWS(parse({ "show", "--l", "work" }));
        REQUIRE_THROWS(parse({ "show", "-list", "work" }));
        REQUIRE_THROWS(parse({ "show", "---list", "work" }));
        REQUIRE_THROWS(parse({ "show", "-" }));
        REQUIRE_THROWS(parse({ "show", "--" }));
    }

    SECTION("Autocompletion: --h / --l are partial long options, not -h / -l")
    {
        for (const char* partial : { "--h", "--l", "-he" }) {
            const char* argv[] = { "./program", "commands", partial };
            int argc = sizeof(argv) / sizeof(argv[0]);
            CommandOption commandOption = CommandOption();
            CommandValidation commandValidation(commandOption, argc, argv);
            REQUIRE_NOTHROW(commandValidation.make());
            REQUIRE(commandValidation.getCommandArguments() == std::vector<std::string>{ partial });
            REQUIRE(commandValidation.getCommandOptions().empty());
        }
    }
}
