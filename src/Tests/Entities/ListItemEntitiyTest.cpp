#define CATCH_CONFIG_MAIN
#include "../../Entities/ListItemEntity.h"
#include "../../FileDataStorage/ConfService.h"
#include "../../FileDataStorage/JSONService.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListService.h"
#include "../Mock/MockInit.h"
#include "../Mock/MockInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ListItemEntity tests", "[ListItemEntity]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_ListItemRepositoryTest");
    Command command = Command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);
    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    EventBus bus = EventBus();
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("Test setting and getting properties")
    {
        // Create a ListItemEntity object
        ListItemEntity listItemEntity(listName);

        // Set properties
        std::string id = "aaaa";
        int position = 1;
        std::string value = "test_value";
        std::string priority = "high";
        std::string status = "started";
        time_t dueAt = 0;
        time_t closedAt = 0;
        time_t createdAt = time(nullptr);
        time_t updatedAt = time(nullptr);

        listItemEntity.setId(id);
        listItemEntity.setPosition(position);
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
        REQUIRE((*listItemEntity.getListName()).getName() == "tempListName");
        REQUIRE(*listItemEntity.getValue() == value);
        REQUIRE(*(*listItemEntity.priority()).getName() == priority);
        REQUIRE(*(*listItemEntity.status()).getName() == status);
        REQUIRE(*listItemEntity.getCreatedAt() == createdAt);
        REQUIRE(*listItemEntity.getUpdatedAt() == updatedAt);
    }

    SECTION("Test setting properties with set method")
    {
        // Set properties using set method
        std::string id = "aaaa";
        std::string value = "test_value";
        std::string priority = "high";
        int priorityId = priorityService.getIdFromName(priority);
        std::string status = "started";
        int statusId = statusService.getIdFromName(status);
        time_t createdAt = time(nullptr);
        time_t updatedAt = time(nullptr);

        PriorityEntity priorityEntity = priorityService.getPriorityFromName(priority);
        StatusEntity statusEntity = statusService.getStatusFromName(status);
        ListItemEntity listItemEntity =
            ListItemEntity::set(id, value, priorityEntity, statusEntity, 0, 0, createdAt, updatedAt, listName);

        // Verify that the values are set correctly
        REQUIRE(*listItemEntity.getId() == id);
        REQUIRE((*listItemEntity.getListName()).getName() == "tempListName");
        REQUIRE(*listItemEntity.getValue() == value);
        REQUIRE(*(*listItemEntity.priority()).getName() == priority);
        REQUIRE(*(*listItemEntity.status()).getName() == status);
        REQUIRE(*listItemEntity.getCreatedAt() == createdAt);
        REQUIRE(*listItemEntity.getUpdatedAt() == updatedAt);
    }

    SECTION("Test setting properties with invalid data")
    {
        // Create a ListItemEntity object
        ListItemEntity listItemEntity(listName);

        // Set invalid properties
        std::string invalidId;
        std::string validId = "aaaa";
        int invalidPosition = -1;
        int validPosition = 1;
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
        REQUIRE_THROWS(listItemEntity.setValue(invalidValue));
        listItemEntity.setValue(validValue);
        REQUIRE_THROWS(listItemEntity.setCreatedAt(invalidCreatedAt));
        listItemEntity.setCreatedAt(validCreatedAt);
        REQUIRE_THROWS(listItemEntity.setUpdatedAt(invalidUpdatedAt));
        listItemEntity.setUpdatedAt(validUpdatedAt);

        // Verify that the values remain unchanged (default values)
        REQUIRE(*listItemEntity.getId() == validId);
        REQUIRE(*listItemEntity.getPosition() == validPosition);
        REQUIRE((*listItemEntity.getListName()).getName() == "tempListName");
        REQUIRE(*listItemEntity.getValue() == validValue);
        REQUIRE(*listItemEntity.getCreatedAt() == validCreatedAt);
        REQUIRE(*listItemEntity.getUpdatedAt() == validUpdatedAt);
    }
}
