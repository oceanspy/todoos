#include "../../../List/ListItems/StatusEntity.h"
#include "../../../List/ListItems/StatusService.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("StatusEntityTest", "[StatusEntity]")
{
    SECTION("getters and setters")
    {
        StatusEntity entity;
        entity.setId(1);
        entity.setPosition(2);
        entity.setName("completed");
        entity.setCommandName("completed");
        entity.setIcon("✓");
        entity.setColor("GREEN");
        entity.setStyle("BOLD");
        entity.setClosed(true);
        entity.setCancelled(false);
        entity.setPassive(false);

        REQUIRE(*entity.getId() == 1);
        REQUIRE(*entity.getPosition() == 2);
        REQUIRE(*entity.getName() == "completed");
        REQUIRE(*entity.getCommandName() == "completed");
        REQUIRE(*entity.getIcon() == "✓");
        REQUIRE(*entity.isClosed() == true);
        REQUIRE(*entity.isCancelled() == false);
        REQUIRE(*entity.isPassive() == false);
    }

    SECTION("default values")
    {
        StatusEntity entity;
        REQUIRE(*entity.getId() == 0);
        REQUIRE(*entity.getPosition() == 0);
        REQUIRE(*entity.getName() == "");
        REQUIRE(*entity.isClosed() == false);
        REQUIRE(*entity.isCancelled() == false);
        REQUIRE(*entity.isPassive() == false);
    }

    SECTION("isClosed / isCancelled / isPassive")
    {
        StatusEntity entity;

        entity.setClosed(true);
        REQUIRE(*entity.isClosed() == true);
        entity.setClosed(false);
        REQUIRE(*entity.isClosed() == false);

        entity.setCancelled(true);
        REQUIRE(*entity.isCancelled() == true);
        entity.setCancelled(false);
        REQUIRE(*entity.isCancelled() == false);

        entity.setPassive(true);
        REQUIRE(*entity.isPassive() == true);
        entity.setPassive(false);
        REQUIRE(*entity.isPassive() == false);
    }

    SECTION("getColor and getStyle return bash codes")
    {
        StatusEntity entity;
        entity.setColor("GREEN");
        entity.setStyle("BOLD");

        REQUIRE(entity.getColor() == "\033[32m");
        REQUIRE(entity.getStyle() == "\033[1m");
    }

    SECTION("getColor and getStyle with empty style returns empty")
    {
        StatusEntity entity;
        entity.setColor("");
        entity.setStyle("");

        REQUIRE(entity.getColor() == "");
        REQUIRE(entity.getStyle() == "");
    }

    SECTION("setFromVector and makeVector round-trip")
    {
        std::vector<std::string> data = { "3",     "2",    "completed", "completed", "✓",
                                          "GREEN", "BOLD", "true",      "false",     "false" };

        StatusEntity entity = StatusEntity::setFromVector(data);
        REQUIRE(*entity.getId() == 3);
        REQUIRE(*entity.getPosition() == 2);
        REQUIRE(*entity.getName() == "completed");
        REQUIRE(*entity.getCommandName() == "completed");
        REQUIRE(*entity.getIcon() == "✓");
        REQUIRE(*entity.isClosed() == true);
        REQUIRE(*entity.isCancelled() == false);
        REQUIRE(*entity.isPassive() == false);

        std::vector<std::string> vectorResult = StatusEntity::makeVector(entity);
        REQUIRE(vectorResult[0] == "3");
        REQUIRE(vectorResult[1] == "2");
        REQUIRE(vectorResult[2] == "completed");
        REQUIRE(vectorResult[3] == "completed");
        REQUIRE(vectorResult[4] == "✓");
        REQUIRE(vectorResult[7] == "true");
        REQUIRE(vectorResult[8] == "false");
        REQUIRE(vectorResult[9] == "false");
    }

    SECTION("legacy alias: 'to-do' resolves to 'queued'")
    {
        StatusService statusService;
        StatusEntity entity = statusService.getStatusFromName("to-do");
        REQUIRE(*entity.getCommandName() == "queued");
        REQUIRE(*entity.getId() == StatusService::QUEUED);
    }

    SECTION("legacy alias: 'reviewing' resolves to 'triaged'")
    {
        StatusService statusService;
        StatusEntity entity = statusService.getStatusFromName("reviewing");
        REQUIRE(*entity.getCommandName() == "triaged");
        REQUIRE(*entity.getId() == StatusService::TRIAGED);
    }

    SECTION("setFromVector with false booleans")
    {
        std::vector<std::string> data = {
            "0", "0", "queued", "queued", "○", "WHITE", "RESET", "false", "false", "false"
        };

        StatusEntity entity = StatusEntity::setFromVector(data);
        REQUIRE(*entity.isClosed() == false);
        REQUIRE(*entity.isCancelled() == false);
        REQUIRE(*entity.isPassive() == false);
    }
}
