#include "../../Help/HelpPrinter.h"
#include "../../IOService/IOService.h"
#include "version.h"
#include <catch2/catch_test_macros.hpp>

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
}
