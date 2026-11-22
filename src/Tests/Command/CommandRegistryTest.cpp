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

    SECTION("--to names a parent item on add / attach")
    {
        for (const std::string name : { "add", "a", "attach" }) {
            Command to(name, { "aaaa" }, { { "to", "bbbb" } }, name + " aaaa --to bbbb");
            REQUIRE(CommandRegistry::isCommandValidWithOptions(to) == true);
        }
    }

    SECTION("parent / child options: detach --force, clean --attached, remove --force-permanently")
    {
        Command detach("detach", { "aaaa" }, { { "force", "" } }, "detach aaaa --force");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(detach) == true);
        Command reassign("reassign", { "aaaa" }, { { "to", "SRV1" } }, "reassign aaaa --to SRV1");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(reassign) == true);
        CommandRegistry registry;
        REQUIRE(registry.isValid("reassign"));
        Command attach("attach", { "aaaa" }, { { "force", "" } }, "attach aaaa --force");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(attach) == true);

        Command clean("clean", {}, { { "attached", "" } }, "clean -a");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(clean) == true);
        for (const std::string name :
             { "status", "queue", "start", "triage", "pause", "block", "complete", "done", "cancel" }) {
            Command statusAttached(name, { "aaaa" }, { { "attached", "" } }, name + " aaaa -a");
            REQUIRE(CommandRegistry::isCommandValidWithOptions(statusAttached) == true);
        }
        Command resetAttached("reset", { "aaaa" }, { { "attached", "" } }, "reset aaaa -a");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(resetAttached) == false);
        Command showAttached("find", { "foo" }, { { "attached", "" } }, "find foo -a");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(showAttached) == false);

        Command removeF("rm", { "aaaa" }, { { "force-permanently", "" } }, "rm aaaa -F");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(removeF) == true);
        Command moveF("move", { "aaaa" }, { { "force-permanently", "" } }, "move aaaa -F");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(moveF) == false);
    }

    SECTION("--id is accepted on add only")
    {
        Command add("add", { "server" }, { { "id", "SRV1" } }, "add server --id SRV1");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(add) == true);
        Command addShortcut("a", { "server" }, { { "id", "SRV1" } }, "a server --id SRV1");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(addShortcut) == true);
        Command edit("edit", { "aaaa", "text" }, { { "id", "SRV1" } }, "edit aaaa text --id SRV1");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(edit) == false);
    }

    SECTION("--to is rejected on other commands")
    {
        Command remove("remove", { "aaaa" }, { { "to", "work" } }, "remove aaaa --to work");
        REQUIRE(CommandRegistry::isCommandValidWithOptions(remove) == false);
    }
}
