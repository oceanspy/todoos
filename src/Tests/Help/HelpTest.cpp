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
}
