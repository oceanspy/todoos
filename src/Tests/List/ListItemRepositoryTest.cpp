#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../IOService/IOService.h"
#include "../Mock/MockInit.h"
#include "../../FileDataStorage/JSONService.h"
#include "../Mock/MockInstallation.h"
#include "../../FileDataStorage/ConfService.h"


TEST_CASE("ListItemRepositoryTest", "[ListItemRepository]") {
    // Create mock objects
    IOService ioService("cli");
    ConfService confService = ConfService(ioService);
    JSONService jsonService = JSONService(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataStorageServicePtr = std::make_unique<JSONService>(ioService);
    std::unique_ptr<FileDataServiceInterface> fileDataConfigStorageServicePtr = std::make_unique<ConfService>(ioService);
    MockInit init(ioService, "_todoos_ListItemRepositoryTest");
    MockInstallation installation(ioService, jsonService, confService, init);
    std::string tempListName = "tempListName";
    std::string tempList2Name = "tempList2Name";
    installation.make();
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    Command command = Command("", {}, {}, "");
    ConfigService configService(ioService, init, configRepository, command);
    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository(configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    listItemRepository.load(tempListName);

    SECTION("Test get method") {
        std::vector <ListItemEntity> results = listItemRepository.get();

        REQUIRE(!results.empty());
        REQUIRE(results.size() == 2);
        REQUIRE(results[0].getId() == "aaaa");
        REQUIRE(results[0].getValue() == "test 1");
        REQUIRE(*results[0].priority().getName() == "high");
        REQUIRE(*results[0].status().getCommandName() == "to-do");
        REQUIRE(results[0].getCreatedAt() == 1712487259);
        REQUIRE(results[0].getUpdatedAt() == 1712487259);
        REQUIRE(results[1].getId() == "bbbb");
        REQUIRE(results[1].getValue() == "test 2");
        REQUIRE(*results[1].priority().getName() == "medium");
        REQUIRE(*results[1].status().getCommandName() == "started");
        REQUIRE(results[1].getCreatedAt() == 1712487272);
        REQUIRE(results[1].getUpdatedAt() == 1712487272);
    }

    SECTION("Test find method") {
        // Call the get method
        const std::string id = "aaaa";
        const std::string value = "test 1";
        const std::string priority = "high";
        const std::string status = "to-do";
        const std::time_t createdAt = 1712487259;
        const std::time_t updatedAt = 1712487259;

        ListItemEntity result = listItemRepository.find(id);

        REQUIRE(result.getId() == id);
        REQUIRE(result.getValue() == value);
        REQUIRE(*result.priority().getName() == priority);
        REQUIRE(*result.status().getCommandName() == status);
        REQUIRE(result.getCreatedAt() == createdAt);
        REQUIRE(result.getUpdatedAt() == updatedAt);
    }

    SECTION("Test update method") {
        std::string id = "aaaa";
        const std::string value = "test 1";

        ListItemEntity oldItem = listItemRepository.find(id);

        const std::string newValue = "test 1 updated";
        const std::string newPriority = "medium";
        const std::string newStatus = "reviewing";
        const std::time_t newCreatedAt = 1712487260;
        const std::time_t newUpdatedAt = 1712487260;

        ListItemEntity newItem;
        newItem.setId(id);
        newItem.setValue(newValue);
        newItem.setPriority(priorityService.getPriorityFromName(newPriority));
        newItem.setStatus(statusService.getStatusFromName(newStatus));

        listItemRepository.update(id, newItem);

        ListItemEntity result = listItemRepository.find(id);
        REQUIRE(result.getId() == id);
        REQUIRE(result.getValue() == newValue);
        REQUIRE(*result.priority().getName() == newPriority);
        REQUIRE(*result.status().getCommandName() == newStatus);

        listItemRepository.update(id, oldItem);

        ListItemEntity newItem2 = listItemRepository.find(id);
        newItem2.setPriority(priorityService.getPriorityFromName(newPriority));
        newItem2.setStatus(statusService.getStatusFromName(newStatus));
        listItemRepository.update(id, newItem2);

        ListItemEntity result2 = listItemRepository.find(id);
        REQUIRE(result2.getId() == id);
        REQUIRE(result2.getValue() == "test 1");
        REQUIRE(*result2.priority().getName() == newPriority);
        REQUIRE(*result2.status().getCommandName() == newStatus);

        listItemRepository.update(id, oldItem);
    }

    SECTION("Test create & delete method") {
        const std::string id = "cccc";
        const std::string newValue = "test 3";
        const std::string newPriority = "medium";
        const std::string newStatus = "reviewing";
        const std::time_t newCreatedAt = 1712487260;
        const std::time_t newUpdatedAt = 1712487260;

        ListItemEntity newItem;
        newItem.setId(id);
        newItem.setValue(newValue);
        newItem.setPriority(priorityService.getPriorityFromName(newPriority));
        newItem.setStatus(statusService.getStatusFromName(newStatus));
        newItem.setCreatedAt(newCreatedAt);
        newItem.setUpdatedAt(newUpdatedAt);

        listItemRepository.create(newItem);

        ListItemEntity result = listItemRepository.find(id);
        REQUIRE(result.getId() == id);
        REQUIRE(result.getValue() == newValue);
        REQUIRE(*result.priority().getName() == newPriority);
        REQUIRE(*result.status().getCommandName() == newStatus);
        REQUIRE(result.getCreatedAt() == newCreatedAt);
        REQUIRE(result.getUpdatedAt() == newUpdatedAt);

        listItemRepository.remove(id);

        std::vector <ListItemEntity> results = listItemRepository.get();

        REQUIRE(!results.empty());
        REQUIRE(results.size() == 2);
        REQUIRE(results[0].getId() == "aaaa");
        REQUIRE(results[0].getValue() == "test 1");
        REQUIRE(*results[0].priority().getName() == "high");
        REQUIRE(*results[0].status().getCommandName() == "to-do");
        REQUIRE(results[0].getCreatedAt() == 1712487259);
        REQUIRE(results[0].getUpdatedAt() == 1712487259);
        REQUIRE(results[1].getId() == "bbbb");
        REQUIRE(results[1].getValue() == "test 2");
        REQUIRE(*results[1].priority().getName() == "medium");
        REQUIRE(*results[1].status().getCommandName() == "started");
        REQUIRE(results[1].getCreatedAt() == 1712487272);
        REQUIRE(results[1].getUpdatedAt() == 1712487272);
    }
}