#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../../Entities/ListItemEntity.h"

TEST_CASE("ListItemEntity tests", "[ListItemEntity]") {

    PriorityService priorityService;
    StatusService statusService;

    SECTION("Test setting and getting properties") {
        // Create a ListItemEntity object
        ListItemEntity listItemEntity;

        // Set properties
        std::string id = "aaaa";
        int position = 1;
        std::string listName = "test_list";
        std::string value = "test_value";
        std::string priority = "high";
        std::string status = "started";
        time_t dueAt = 0;
        time_t closedAt = 0;
        time_t createdAt = time(nullptr);
        time_t updatedAt = time(nullptr);

        listItemEntity.setId(id);
        listItemEntity.setPosition(position);
        listItemEntity.setListName(listName);
        listItemEntity.setValue(value);
        PriorityEntity priorityEntity = priorityService.getPriorityFromName(priority);
        listItemEntity.setPriority(priorityEntity);
        StatusEntity statusEntity = statusService.getStatusFromName(status);
        listItemEntity.setStatus(statusEntity);
        listItemEntity.setDueAt(dueAt);
        listItemEntity.setClosedAt(closedAt);
        listItemEntity.setCreatedAt(createdAt);
        listItemEntity.setUpdatedAt(updatedAt);

        // Verify that the values are set correctly
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE(*listItemEntity.getPosition() == position);
        REQUIRE(*listItemEntity.getListName() == listName);
        REQUIRE(*listItemEntity.getValue() == value);
        REQUIRE(*(*listItemEntity.priority()).getName() == priority);
        REQUIRE(*(*listItemEntity.status()).getName() == status);
        REQUIRE(*listItemEntity.getCreatedAt() == createdAt);
        REQUIRE(*listItemEntity.getUpdatedAt() == updatedAt);
    }

    SECTION("Test setting properties with set method") {
        // Set properties using set method
        std::string id = "aaaa";
        std::string listName = "test_list";
        std::string value = "test_value";
        std::string priority = "high";
        int priorityId = priorityService.getIdFromName(priority);
        std::string status = "started";
        int statusId = statusService.getIdFromName(status);
        time_t createdAt = time(nullptr);
        time_t updatedAt = time(nullptr);

        PriorityEntity priorityEntity = priorityService.getPriorityFromName(priority);
        StatusEntity statusEntity = statusService.getStatusFromName(status);
        ListItemEntity listItemEntity = ListItemEntity::set(id, listName, value, priorityEntity, statusEntity, 0, 0, createdAt, updatedAt);

        // Verify that the values are set correctly
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE(*listItemEntity.getListName() == listName);
        REQUIRE(*listItemEntity.getValue() == value);
        REQUIRE(*(*listItemEntity.priority()).getName() == priority);
        REQUIRE(*(*listItemEntity.status()).getName() == status);
        REQUIRE(*listItemEntity.getCreatedAt() == createdAt);
        REQUIRE(*listItemEntity.getUpdatedAt() == updatedAt);
    }


    SECTION("Test setting properties with invalid data") {
        // Create a ListItemEntity object
        ListItemEntity listItemEntity;

        // Set invalid properties
        std::string invalidId;
        std::string validId = "aaaa";
        int invalidPosition = -1;
        int validPosition = 1;
        std::string listName;
        std::string invalidValue;
        std::string validValue = "test value";
        time_t invalidCreatedAt = -1;
        time_t invalidUpdatedAt = -1;
        time_t validCreatedAt = time(nullptr);
        time_t validUpdatedAt = time(nullptr);


        // Attempt to set invalid properties
        REQUIRE_THROWS(listItemEntity.setId(invalidId));
        invalidId = "invalid_id";
        REQUIRE_THROWS(listItemEntity.setId(invalidId));
        listItemEntity.setId(validId);
        REQUIRE_THROWS(listItemEntity.setPosition(invalidPosition));
        listItemEntity.setPosition(validPosition);
        listItemEntity.setListName(listName);
        REQUIRE_THROWS(listItemEntity.setValue(invalidValue));
        listItemEntity.setValue(validValue);
        REQUIRE_THROWS(listItemEntity.setCreatedAt(invalidCreatedAt));
        listItemEntity.setCreatedAt(validCreatedAt);
        REQUIRE_THROWS(listItemEntity.setUpdatedAt(invalidUpdatedAt));
        listItemEntity.setUpdatedAt(validUpdatedAt);

        // Verify that the values remain unchanged (default values)
        REQUIRE(*listItemEntity.getId() == validId);
        REQUIRE(*listItemEntity.getPosition() == validPosition);
        REQUIRE(*listItemEntity.getListName() == listName);
        REQUIRE(*listItemEntity.getValue() == validValue);
        REQUIRE(*listItemEntity.getCreatedAt() == validCreatedAt);
        REQUIRE(*listItemEntity.getUpdatedAt() == validUpdatedAt);
    }
}