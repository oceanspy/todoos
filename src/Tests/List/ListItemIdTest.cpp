#include "../../List/ListItemId.h"
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <set>

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
}
