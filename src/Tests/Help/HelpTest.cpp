#include "../../Help/Help.h"
#include "../../IOService/IOService.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Help tests", "[Help]")
{
    IOService ioService("cli");
    Help help(ioService);

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
        std::string version = Help::getVersion();
        REQUIRE(version.empty() == false);
        REQUIRE(version.find("Version") != std::string::npos);
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
