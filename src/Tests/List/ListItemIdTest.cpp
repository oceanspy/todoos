#include "../../List/ListItemId.h"
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <set>
#include <stdexcept>

TEST_CASE("ListItemId", "[ListItemId]")
{
    SECTION("getIdLength returns 4")
    {
        REQUIRE(ListItemId::getIdLength() == 4);
    }

    SECTION("generate with letters returns 4 alphanumeric characters")
    {
        std::string id = ListItemId::generate("letters");
        REQUIRE(id.length() == 4);
        for (char c : id) {
            REQUIRE(std::isalnum(static_cast<unsigned char>(c)));
        }
    }

    SECTION("generate with letters-lowercase returns 4 lowercase alphabetic characters")
    {
        std::string id = ListItemId::generate("letters-lowercase");
        REQUIRE(id.length() == 4);
        for (char c : id) {
            REQUIRE(std::islower(static_cast<unsigned char>(c)));
        }
    }

    SECTION("generate with random returns 4 characters")
    {
        std::string id = ListItemId::generate("random");
        REQUIRE(id.length() == 4);
    }

    SECTION("generate with unknown type falls back to random and returns 4 characters")
    {
        std::string id = ListItemId::generate("unknown");
        REQUIRE(id.length() == 4);
    }

    SECTION("generate produces varied output across multiple calls")
    {
        std::set<std::string> ids;
        for (int i = 0; i < 20; i++) {
            ids.insert(ListItemId::generate("letters-lowercase"));
        }
        REQUIRE(ids.size() > 1);
    }

    SECTION("getVariantIdLength returns 5")
    {
        REQUIRE(ListItemId::getVariantIdLength() == 5);
    }

    SECTION("isValid accepts 4 and 5 alphanumeric characters only")
    {
        REQUIRE(ListItemId::isValid("aB3d"));
        REQUIRE(ListItemId::isValid("aB3dz"));
        REQUIRE_FALSE(ListItemId::isValid("abc"));
        REQUIRE_FALSE(ListItemId::isValid("abcdef"));
        REQUIRE_FALSE(ListItemId::isValid("ab-d"));
        REQUIRE_FALSE(ListItemId::isValid("abcd "));
        REQUIRE_FALSE(ListItemId::isValid(""));
    }

    SECTION("baseOf returns the 4-char part")
    {
        REQUIRE(ListItemId::baseOf("abcdz") == "abcd");
        REQUIRE(ListItemId::baseOf("abcd") == "abcd");
    }

    SECTION("withSuffix appends a lowercase letter to a 4-char base")
    {
        REQUIRE(ListItemId::withSuffix("aB3d", 'a') == "aB3da");
        REQUIRE(ListItemId::withSuffix("aB3d", 'z') == "aB3dz");
        REQUIRE_THROWS_AS(ListItemId::withSuffix("abc", 'a'), std::invalid_argument);
        REQUIRE_THROWS_AS(ListItemId::withSuffix("abcde", 'a'), std::invalid_argument);
        REQUIRE_THROWS_AS(ListItemId::withSuffix("abcd", 'A'), std::invalid_argument);
        REQUIRE_THROWS_AS(ListItemId::withSuffix("abcd", '1'), std::invalid_argument);
    }
}
