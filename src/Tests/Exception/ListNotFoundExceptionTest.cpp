#include "../../Exception/ListNotFoundException.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ListNotFoundExceptionTest", "[ListNotFoundException]")
{
    SECTION("stores message, name, and variant")
    {
        ListNotFoundException ex("List not found", "myList", "default");
        REQUIRE(ex.getMessage() == "List not found");
        REQUIRE(ex.getName() == "myList");
        REQUIRE(ex.getVariant() == "default");
    }

    SECTION("with archive variant")
    {
        ListNotFoundException ex("Not found", "testList", "archive");
        REQUIRE(ex.getMessage() == "Not found");
        REQUIRE(ex.getName() == "testList");
        REQUIRE(ex.getVariant() == "archive");
    }

    SECTION("with empty strings")
    {
        ListNotFoundException ex("", "", "");
        REQUIRE(ex.getMessage() == "");
        REQUIRE(ex.getName() == "");
        REQUIRE(ex.getVariant() == "");
    }

    SECTION("is catchable as std::exception")
    {
        try {
            throw ListNotFoundException("error", "list", "default");
        } catch (const std::exception& e) {
            REQUIRE(true);
        }
    }
}
