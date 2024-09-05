#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../../Entities/ListEntity.h"


TEST_CASE("ListEntity tests", "[ListEntity]") {
    SECTION("Test setting and getting name, theme, and sorting") {
        // Create a ListEntity object
        ListEntity listEntity;

        // Set name, theme, and sorting
        std::string name = "test_name";
        std::string type = "test_type";
        std::string sorting = "test_sorting";
        listEntity.setName(name);
        listEntity.setType(type);
        listEntity.setSorting(sorting);

        // Verify that the values are set correctly
        REQUIRE(*listEntity.getName() == name);
        REQUIRE(*listEntity.getType() == type);
        REQUIRE(*listEntity.getSorting() == sorting);

        // Set name and verify
        std::string newName = "new_name";
        listEntity.setName(newName);
        REQUIRE(*listEntity.getName() == newName);

        // Set theme and verify
        std::string newTheme = "new_theme";
        listEntity.setType(newTheme);
        REQUIRE(*listEntity.getType() == newTheme);

        // Set theme and verify
        std::string newType2 = "new_THEME";
        listEntity.setType(newType2);
        REQUIRE(*listEntity.getType() == newType2);

        // Set sorting and verify
        std::string newSorting = "new_sorting";
        listEntity.setSorting(newSorting);
        REQUIRE(*listEntity.getSorting() == newSorting);
    }
}
