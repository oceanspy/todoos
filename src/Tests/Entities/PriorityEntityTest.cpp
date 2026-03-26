#include "../../Entities/PriorityEntity.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PriorityEntityTest", "[PriorityEntity]")
{
    SECTION("getters and setters")
    {
        PriorityEntity entity;
        entity.setId(3);
        entity.setPosition(2);
        entity.setName("high");
        entity.setIcon("▲");
        entity.setColor("RED");

        REQUIRE(*entity.getId() == 3);
        REQUIRE(*entity.getPosition() == 2);
        REQUIRE(*entity.getName() == "high");
        REQUIRE(*entity.getIcon() == "▲");
    }

    SECTION("default values")
    {
        PriorityEntity entity;
        REQUIRE(*entity.getId() == 0);
        REQUIRE(*entity.getPosition() == 0);
        REQUIRE(*entity.getName() == "");
        REQUIRE(*entity.getIcon() == "");
    }

    SECTION("getColor returns bash code")
    {
        PriorityEntity entity;
        entity.setColor("RED");
        REQUIRE(entity.getColor() == "\033[31m");

        entity.setColor("YELLOW");
        REQUIRE(entity.getColor() == "\033[33m");
    }

    SECTION("getColor with empty returns empty")
    {
        PriorityEntity entity;
        entity.setColor("");
        REQUIRE(entity.getColor() == "");
    }

    SECTION("setFromVector and makeVector round-trip")
    {
        std::vector<std::string> data = { "3", "2", "high", "▲", "RED" };

        PriorityEntity entity = PriorityEntity::setFromVector(data);
        REQUIRE(*entity.getId() == 3);
        REQUIRE(*entity.getPosition() == 2);
        REQUIRE(*entity.getName() == "high");
        REQUIRE(*entity.getIcon() == "▲");

        std::vector<std::string> vectorResult = PriorityEntity::makeVector(entity);
        REQUIRE(vectorResult[0] == "3");
        REQUIRE(vectorResult[1] == "2");
        REQUIRE(vectorResult[2] == "high");
        REQUIRE(vectorResult[3] == "▲");
    }

    SECTION("setFromName returns empty entity")
    {
        PriorityEntity entity = PriorityEntity::setFromName("high");
        REQUIRE(*entity.getId() == 0);
        REQUIRE(*entity.getName() == "");
    }
}
