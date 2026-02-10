#include "../../List/ListName.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ListNameTest", "[ListService]")
{
    SECTION("ListName")
    {
        ListName listName = ListName("test", "default");
        REQUIRE(listName.getName() == "test");
        REQUIRE(listName.getVariant() == "default");

        REQUIRE_THROWS(ListName::createVariant(listName, "unknown"));
    }
}
