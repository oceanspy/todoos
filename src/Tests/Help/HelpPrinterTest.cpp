#include "../../Help/HelpPrinter.h"
#include "../../IOService/IOService.h"
#include "version.h"
#include <catch2/catch_test_macros.hpp>
#include <regex>

TEST_CASE("HelpPrinter tests", "[HelpPrinter]")
{
    IOService ioService("cli");
    HelpPrinter help(ioService);

    SECTION("show does not throw")
    {
        REQUIRE_NOTHROW(help.show());
    }

    SECTION("commandNotFound does not throw")
    {
        REQUIRE_NOTHROW(help.commandNotFound());
    }

    SECTION("commandOptionNotSupported does not throw")
    {
        REQUIRE_NOTHROW(help.commandOptionNotSupported());
    }

    SECTION("listNotFound does not throw")
    {
        REQUIRE_NOTHROW(help.listNotFound("nonExistentList"));
    }

    SECTION("getVersion returns a version string")
    {
        std::string version = HelpPrinter::getVersion();
        REQUIRE(version.empty() == false);
        REQUIRE(version.find("Version") != std::string::npos);
    }

    SECTION("getVersion contains a semver-like number")
    {
        std::string version = HelpPrinter::getVersion();
        // Expect format "Version X.Y.Z"
        REQUIRE(version.find('.') != std::string::npos);
    }

    SECTION("getVersion matches compile-time APP_VERSION")
    {
        std::string version = HelpPrinter::getVersion();
        REQUIRE(version == std::string("Version ") + APP_VERSION);
    }

    SECTION("commandNotFoundSkipCommandAutocomplete skips output for 'commands'")
    {
        Command command("commands", {}, {});
        REQUIRE_NOTHROW(help.commandNotFoundSkipCommandAutocomplete(command));
    }

    SECTION("commandNotFoundSkipCommandAutocomplete shows error for unknown command")
    {
        Command command("unknown", {}, {});
        REQUIRE_NOTHROW(help.commandNotFoundSkipCommandAutocomplete(command));
    }

    SECTION("commandOptionNotSupportedSkipCommandAutocomplete skips output for 'commands'")
    {
        Command command("commands", {}, {});
        REQUIRE_NOTHROW(help.commandOptionNotSupportedSkipCommandAutocomplete(command));
    }

    SECTION("commandOptionNotSupportedSkipCommandAutocomplete shows error for regular command")
    {
        Command command("add", {}, {});
        REQUIRE_NOTHROW(help.commandOptionNotSupportedSkipCommandAutocomplete(command));
    }

    SECTION("help columns line up: descriptions and option hints start at the same column")
    {
        const std::regex colorCodes("\033\\[[0-9;]*m");
        std::vector<std::string> lines;
        for (const std::string& line : HelpPrinter::helpListForCli()) {
            lines.push_back(std::regex_replace(line, colorCodes, ""));
        }
        auto columnOf = [&lines](const std::string& text) -> long {
            for (const std::string& line : lines) {
                size_t at = line.find(text);
                if (at != std::string::npos) {
                    return static_cast<long>(at);
                }
            }
            FAIL("not in the help: " + text);
            return -1;
        };

        // commands of every section share the description column
        const long descColumn = columnOf("Switch active list");
        for (const std::string desc : { "Show items", "Remove items", "Duplicate items", "Attach to a parent",
                                        "Move to list", "Set priority", "Set deadline", "\xE2\x86\x92 Completed",
                                        "Archive done/cancelled" }) {
            REQUIRE(columnOf(desc) == descColumn);
        }

        // and the option hints column
        const long hintColumn = columnOf("[-p] [-s] [-x] [-A]");
        for (const std::string hint : { "[-f] [-F]", "[-f] switch parent", "[-f] all, on a parent",
                                        "[-d] include described", "[-a] include child" }) {
            REQUIRE(columnOf(hint) == hintColumn);
        }

        // flags and options share their description and "applies to" columns
        REQUIRE(columnOf("Show app version") == columnOf("Delete for good"));
        REQUIRE(columnOf("Delete for good") == columnOf("Target list "));
        REQUIRE(columnOf("(remove)") == columnOf("(add, attach)"));
    }
}
