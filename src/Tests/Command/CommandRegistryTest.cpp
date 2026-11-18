#include "../../Command/CommandRegistry.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandRegistry Tests", "[CommandRegistry]")
{
    SECTION("Check if command exists")
    {
        CommandRegistry commandList;
        REQUIRE(commandList.isValid("commands") == true);
        REQUIRE(commandList.isValid("add") == true);
        REQUIRE(commandList.isValid("truc") == false);
    }

    SECTION("move and copy replace move-to and copy-to")
    {
        CommandRegistry commandList;
        REQUIRE(commandList.isValid("move") == true);
        REQUIRE(commandList.isValid("copy") == true);
        REQUIRE(commandList.isValid("move-to") == false);
        REQUIRE(commandList.isValid("copy-to") == false);
    }

    SECTION("--to and --force are accepted on move/copy and their shortcuts")
    {
        for (const std::string name : { "move", "mv", "copy", "cp" }) {
            Command to(name, { "aaaa" }, { { "to", "work" } }, name + " aaaa --to work");
            REQUIRE(CommandRegistry::isCommandValidWithOptions(to) == true);

            Command force(name, { "aaaa" }, { { "to", "work" }, { "force", "" } }, name + " aaaa --to work --force");
            REQUIRE(CommandRegistry::isCommandValidWithOptions(force) == true);
        }
    }

    SECTION("--to is rejected on other commands")
    {
        Command add("add", { "foo" }, { { "to", "work" } }, "add foo --to work");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(add) == false);

        Command remove("remove", { "aaaa" }, { { "to", "work" } }, "remove aaaa --to work");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(remove) == false);
    }
}
