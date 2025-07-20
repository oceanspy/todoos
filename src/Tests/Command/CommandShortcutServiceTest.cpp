#include "../../Command/CommandShortcut.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandShortcutService Tests", "[CommandShortcutService]")
{
    SECTION("Get shortcut commands")
    {
        std::map<std::string, std::string> shortcuts = CommandShortcut::getShortcutCommands();
        REQUIRE(shortcuts["l"] == "list");
        REQUIRE(shortcuts["a"] == "add");
        REQUIRE(shortcuts["rm"] == "remove");
        REQUIRE(shortcuts["e"] == "edit");
        REQUIRE(shortcuts["inc"] == "increase");
        REQUIRE(shortcuts["dec"] == "decrease");
        REQUIRE(shortcuts["reset"] == "reset");
        REQUIRE(shortcuts["pause"] == "pause");
        REQUIRE(shortcuts["done"] == "complete");
        REQUIRE(shortcuts["cancel"] == "cancel");
        REQUIRE(shortcuts["settings"] == "config");
        REQUIRE(shortcuts["options"] == "config");
        REQUIRE(shortcuts["p"] == "priority");
        REQUIRE(shortcuts["append"] == "append");
        REQUIRE(shortcuts["prepend"] == "prepend");
        REQUIRE(shortcuts["empty"] == "empty");
    }

    SECTION("Get shortcuts for command")
    {
        std::vector<std::string> shortcuts = CommandShortcut::getShortcutsForCommand("list");
        REQUIRE(shortcuts.size() == 1);
        REQUIRE(std::find(shortcuts.begin(), shortcuts.end(), "l") != shortcuts.end());
    }

    SECTION("Translate command shortcut")
    {
        CommandShortcut shortcutService;
        REQUIRE(shortcutService.getCommandName("l") == "list");
        REQUIRE(shortcutService.getCommandName("p") == "priority");
        REQUIRE(shortcutService.getCommandName("unknown") == "unknown");
    }

    SECTION("Check if command exists")
    {
        CommandShortcut shortcutService;
        Command cmd1("add", {}, {});
        Command cmd3("unknown", {}, {});
        REQUIRE(shortcutService.isCommand(cmd1, "add") == true);
        REQUIRE(shortcutService.isCommand(cmd3, "unknown") == true);
    }
}
