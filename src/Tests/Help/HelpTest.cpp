#include <catch2/catch_test_macros.hpp>
#include "../../Help/Help.h"
#include "../../IOService/IOService.h"

TEST_CASE("Help tests", "[Help]") {
    SECTION("Help test") {
        IOService ioService("cli");
        Help help(ioService);
        REQUIRE_NOTHROW(help.show());
        // No direct way to check output in CLI, can check for exceptions
    }
}