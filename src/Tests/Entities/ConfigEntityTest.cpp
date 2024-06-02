#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../../Entities/ConfigEntity.h"


TEST_CASE("ConfigEntity tests", "[ConfigEntity]") {
    SECTION("Test setting and getting key, value, and arguments") {
        // Create a ConfigEntity object
        ConfigEntity configEntity;

        // Set key, value, and arguments
        std::string key = "test_key";
        std::string value = "test_value";
        configEntity.set(key, value);

        // Verify that the values are set correctly
        REQUIRE(configEntity.getKey() == key);
        REQUIRE(configEntity.getValue() == value);

        // Set key and verify
        std::string newKey = "new_key";
        configEntity.setKey(newKey);
        REQUIRE(configEntity.getKey() == newKey);

        // Set value and verify
        std::string newValue = "new_value";
        configEntity.setValue(newValue);
        REQUIRE(configEntity.getValue() == newValue);
    }
}