#include "../../Actions/ListItemAction/AddItemAction.h"
#include "../../Actions/PriorityAction/PriorityAction.h"
#include "../../Actions/ListItemAction/ArchiveItemAction.h"
#include "../../Actions/ListItemAction/RemoveItemAction.h"
#include "../../Actions/ListItemAction/RestoreItemAction.h"
#include "../../Actions/StatusAction/EditStatusAction.h"
#include "../../Actions/StatusAction/SetStatusAction.h"
#include "../../Actions/ListItemAction/ReassignItemAction.h"
#include "../../Actions/ParentAction/AttachItemAction.h"
#include "../../Actions/ParentAction/DetachItemAction.h"
#include "../../Command/CommandService.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Remove controller", "[CommandRouter][Remove]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("remove", { "aaaa" }, {}, "remove aaaa");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("remove with valid ID soft deletes item")
    {
        std::vector<ListItemEntity> itemsBefore = listItemService.get(listName);
        REQUIRE(itemsBefore.size() == 2);

        RemoveItemAction remove(ioService, listItemService, descriptionRepository);
        REQUIRE_NOTHROW(remove.execute(command, listName));

        std::vector<ListItemEntity> itemsAfter = listItemService.get(listName);
        REQUIRE(itemsAfter.size() == 1);
        REQUIRE(*itemsAfter[0].getId() == "bbbb");

        installation.wipe();
        installation.make();
    }

    SECTION("remove with empty arguments does not throw")
    {
        Command emptyCommand = Command("remove", {}, {}, "remove");
        RemoveItemAction remove(ioService, listItemService, descriptionRepository);
        REQUIRE_NOTHROW(remove.execute(emptyCommand, listName));

        // No items removed
        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("remove with force option alone still soft deletes")
    {
        std::map<std::string, std::string> options = { { "force", "" } };
        Command forceCommand = Command("remove", { "aaaa" }, options, "remove -f aaaa");
        RemoveItemAction remove(ioService, listItemService, descriptionRepository);
        REQUIRE_NOTHROW(remove.execute(forceCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        ListName deleteListName = ListName::createVariant(listName, "delete");
        std::vector<ListItemEntity> deletedItems = listItemService.get(deleteListName);
        REQUIRE(deletedItems.size() == 1);

        installation.wipe();
        installation.make();
    }

    SECTION("remove with force-permanently option permanently removes")
    {
        std::map<std::string, std::string> options = { { "force-permanently", "" } };
        Command forceCommand = Command("remove", { "aaaa" }, options, "remove -F aaaa");
        RemoveItemAction remove(ioService, listItemService, descriptionRepository);
        REQUIRE_NOTHROW(remove.execute(forceCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        // Item should NOT be in delete bin since it was force-removed
        ListName deleteListName = ListName::createVariant(listName, "delete");
        std::vector<ListItemEntity> deletedItems = listItemService.get(deleteListName);
        REQUIRE(deletedItems.size() == 0);

        installation.wipe();
        installation.make();
    }

    SECTION("archive with valid ID archives item")
    {
        Command archiveCommand = Command("archive", { "aaaa" }, {}, "archive aaaa");
        ArchiveItemAction archive(ioService, listItemService);
        REQUIRE_NOTHROW(archive.execute(archiveCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        ListName archiveListName = ListName::createVariant(listName, "archive");
        std::vector<ListItemEntity> archivedItems = listItemService.get(archiveListName);
        REQUIRE(archivedItems.size() == 1);
        REQUIRE((*archivedItems[0].getId()).length() == 5);
        REQUIRE((*archivedItems[0].getId()).substr(0, 4) == "aaaa");

        installation.wipe();
        installation.make();
    }

    SECTION("restore with valid ID restores item")
    {
        // First archive an item
        std::string archivedId = listItemService.archive("aaaa", listName);

        Command restoreCommand = Command("restore", { archivedId }, {}, "restore " + archivedId);
        RestoreItemAction restore(ioService, listItemService);
        REQUIRE_NOTHROW(restore.execute(restoreCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("remove a parent: refused without -f; with -f its children are soft deleted with it")
    {
        listItemService.attach("bbbb", "aaaa", listName);
        RemoveItemAction remove(ioService, listItemService, descriptionRepository);

        REQUIRE_NOTHROW(remove.execute(command, listName));
        REQUIRE(listItemService.get(listName).size() == 2);

        Command forceCommand("remove", { "aaaa" }, { { "force", "" } }, "remove -f aaaa");
        REQUIRE_NOTHROW(remove.execute(forceCommand, listName));
        REQUIRE(listItemService.get(listName).empty());
        ListName deleteListName = ListName::createVariant(listName, "delete");
        REQUIRE(listItemService.get(deleteListName).size() == 2);

        installation.wipe();
        installation.make();
    }

    SECTION("remove -F on a parent deletes it and its children for good")
    {
        listItemService.attach("bbbb", "aaaa", listName);
        RemoveItemAction remove(ioService, listItemService, descriptionRepository);
        Command forceCommand("remove", { "aaaa" }, { { "force-permanently", "" } }, "remove -F aaaa");
        REQUIRE_NOTHROW(remove.execute(forceCommand, listName));
        REQUIRE(listItemService.get(listName).empty());
        ListName deleteListName = ListName::createVariant(listName, "delete");
        REQUIRE(listItemService.get(deleteListName).empty());

        installation.wipe();
        installation.make();
    }

    SECTION("remove a parent and its child listed together with -f: the child goes with its parent")
    {
        listItemService.attach("bbbb", "aaaa", listName);
        RemoveItemAction remove(ioService, listItemService, descriptionRepository);
        Command forceCommand("remove", { "bbbb", "aaaa" }, { { "force", "" } }, "remove -f bbbb aaaa");
        REQUIRE_NOTHROW(remove.execute(forceCommand, listName));
        ListName deleteListName = ListName::createVariant(listName, "delete");
        std::vector<ListItemEntity> deleted = listItemService.get(deleteListName);
        REQUIRE(deleted.size() == 2);
        REQUIRE(std::any_of(deleted.begin(), deleted.end(), [](const ListItemEntity& i) { return i.hasParent(); }));

        installation.wipe();
        installation.make();
    }

    SECTION("reassign --to {newId}; a missing --to or several ids change nothing")
    {
        ReassignItemAction reassign(ioService, listItemService);

        Command missingTo("reassign", { "aaaa" }, {}, "reassign aaaa");
        REQUIRE_NOTHROW(reassign.execute(missingTo, listName));
        Command severalIds("reassign", { "aaaa", "bbbb" }, { { "to", "SRV1" } }, "reassign aaaa bbbb --to SRV1");
        REQUIRE_NOTHROW(reassign.execute(severalIds, listName));
        REQUIRE(*listItemService.find("aaaa", listName).getId() == "aaaa");

        Command reassignCommand("reassign", { "aaaa" }, { { "to", "SRV1" } }, "reassign aaaa --to SRV1");
        REQUIRE_NOTHROW(reassign.execute(reassignCommand, listName));
        REQUIRE(*listItemService.find("SRV1", listName).getValue() == "test 1");

        installation.wipe();
        installation.make();
    }

    SECTION("attach an already attached item: refused without --force, switched with it")
    {
        listItemService.attach("bbbb", "aaaa", listName);
        std::string otherParent = listItemService.add(listName, "other parent");
        AttachItemAction attach(ioService, listItemService);

        Command attachCommand("attach", { "bbbb" }, { { "to", otherParent } }, "attach bbbb --to " + otherParent);
        REQUIRE_NOTHROW(attach.execute(attachCommand, listName));
        REQUIRE(*listItemService.find("bbbb", listName).getParentId() == "aaaa");

        Command forcedCommand(
            "attach", { "bbbb" }, { { "to", otherParent }, { "force", "" } }, "attach bbbb --to " + otherParent + " -f");
        REQUIRE_NOTHROW(attach.execute(forcedCommand, listName));
        REQUIRE(*listItemService.find("bbbb", listName).getParentId() == otherParent);

        installation.wipe();
        installation.make();
    }

    SECTION("attach --to / detach; detaching a parent needs --force")
    {
        AttachItemAction attach(ioService, listItemService);
        Command attachCommand("attach", { "bbbb" }, { { "to", "aaaa" } }, "attach bbbb --to aaaa");
        REQUIRE_NOTHROW(attach.execute(attachCommand, listName));
        REQUIRE(*listItemService.find("bbbb", listName).getParentId() == "aaaa");

        DetachItemAction detach(ioService, listItemService);
        Command detachParent("detach", { "aaaa" }, {}, "detach aaaa");
        REQUIRE_NOTHROW(detach.execute(detachParent, listName));
        REQUIRE(*listItemService.find("bbbb", listName).getParentId() == "aaaa");

        Command detachParentForced("detach", { "aaaa" }, { { "force", "" } }, "detach aaaa --force");
        REQUIRE_NOTHROW(detach.execute(detachParentForced, listName));
        REQUIRE(*listItemService.find("bbbb", listName).getParentId() == "");

        REQUIRE_NOTHROW(attach.execute(attachCommand, listName));
        Command detachChild("detach", { "bbbb" }, {}, "detach bbbb");
        REQUIRE_NOTHROW(detach.execute(detachChild, listName));
        REQUIRE(*listItemService.find("bbbb", listName).getParentId() == "");

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Status controller", "[CommandRouter][Status]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest2");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("start", { "aaaa" }, {}, "start aaaa");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("markAs changes item status")
    {
        EditStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(command, listName, StatusService::STARTED));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "started");

        installation.wipe();
        installation.make();
    }

    SECTION("markAs with empty arguments does not throw")
    {
        Command emptyCommand = Command("start", {}, {}, "start");
        EditStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(emptyCommand, listName, StatusService::STARTED));
    }

    SECTION("set with valid status and ID changes status")
    {
        Command setCommand = Command("status", { "paused", "aaaa" }, {}, "status paused aaaa");
        SetStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(setCommand, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("set with invalid status does not throw")
    {
        Command badCommand = Command("status", { "invalidstatus", "aaaa" }, {}, "status invalidstatus aaaa");
        SetStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(badCommand, listName));
    }

    SECTION("set with missing arguments does not throw")
    {
        Command noArgsCommand = Command("status", {}, {}, "status");
        SetStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(noArgsCommand, listName));

        Command oneArgCommand = Command("status", { "paused" }, {}, "status paused");
        SetStatusAction status2(ioService, listItemService);
        REQUIRE_NOTHROW(status2.execute(oneArgCommand, listName));
    }

    SECTION("markAs --attached also changes the open children, whatever their status")
    {
        listItemService.editStatus("aaaa", listName, new int(StatusService::PAUSED));
        std::string paused = "paused";
        std::string triaged = "triaged";
        std::string completed = "completed";
        std::string cancelled = "cancelled";
        std::string pausedChild = listItemService.add(listName, "paused", nullptr, &paused, 0, "aaaa");
        std::string triagedChild = listItemService.add(listName, "triaged", nullptr, &triaged, 0, "aaaa");
        std::string completedChild = listItemService.add(listName, "completed", nullptr, &completed, 0, "aaaa");
        std::string cancelledChild = listItemService.add(listName, "cancelled", nullptr, &cancelled, 0, "aaaa");

        Command startAttached = Command("start", { "aaaa" }, { { "attached", "" } }, "start aaaa -a");
        EditStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(startAttached, listName, StatusService::STARTED));

        REQUIRE(*(*listItemService.find("aaaa", listName).status()).getName() == "started");
        REQUIRE(*(*listItemService.find(pausedChild, listName).status()).getName() == "started");
        REQUIRE(*(*listItemService.find(triagedChild, listName).status()).getName() == "started");
        REQUIRE(*(*listItemService.find(completedChild, listName).status()).getName() == "completed");
        REQUIRE(*(*listItemService.find(cancelledChild, listName).status()).getName() == "cancelled");

        installation.wipe();
        installation.make();
    }

    SECTION("markAs without --attached leaves the children alone")
    {
        listItemService.editStatus("aaaa", listName, new int(StatusService::PAUSED));
        std::string paused = "paused";
        std::string child = listItemService.add(listName, "child", nullptr, &paused, 0, "aaaa");

        EditStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(command, listName, StatusService::STARTED));

        REQUIRE(*(*listItemService.find("aaaa", listName).status()).getName() == "started");
        REQUIRE(*(*listItemService.find(child, listName).status()).getName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("markAs --attached: the open children follow even when the parent is already in the status")
    {
        listItemService.editStatus("aaaa", listName, new int(StatusService::STARTED));
        std::string started = "started";
        std::string paused = "paused";
        std::string startedChild = listItemService.add(listName, "started", nullptr, &started, 0, "aaaa");
        std::string pausedChild = listItemService.add(listName, "paused", nullptr, &paused, 0, "aaaa");

        Command startAttached = Command("start", { "aaaa" }, { { "attached", "" } }, "start aaaa -a");
        EditStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(startAttached, listName, StatusService::STARTED));

        REQUIRE(*(*listItemService.find("aaaa", listName).status()).getName() == "started");
        REQUIRE(*(*listItemService.find(startedChild, listName).status()).getName() == "started");
        REQUIRE(*(*listItemService.find(pausedChild, listName).status()).getName() == "started");

        installation.wipe();
        installation.make();
    }

    SECTION("set --attached also changes the open children, whatever their status")
    {
        listItemService.editStatus("aaaa", listName, new int(StatusService::PAUSED));
        std::string paused = "paused";
        std::string triaged = "triaged";
        std::string completed = "completed";
        std::string pausedChild = listItemService.add(listName, "paused", nullptr, &paused, 0, "aaaa");
        std::string triagedChild = listItemService.add(listName, "triaged", nullptr, &triaged, 0, "aaaa");
        std::string completedChild = listItemService.add(listName, "completed", nullptr, &completed, 0, "aaaa");

        Command setAttached =
            Command("status", { "blocked", "aaaa" }, { { "attached", "" } }, "status blocked aaaa -a");
        SetStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(setAttached, listName));

        REQUIRE(*(*listItemService.find("aaaa", listName).status()).getName() == "blocked");
        REQUIRE(*(*listItemService.find(pausedChild, listName).status()).getName() == "blocked");
        REQUIRE(*(*listItemService.find(triagedChild, listName).status()).getName() == "blocked");
        REQUIRE(*(*listItemService.find(completedChild, listName).status()).getName() == "completed");

        installation.wipe();
        installation.make();
    }

    SECTION("set completed --attached closes the open children, then auto-archives the group")
    {
        configService.edit("archiveWhenCompleted", "true");
        std::string paused = "paused";
        std::string cancelled = "cancelled";
        listItemService.add(listName, "open child", nullptr, &paused, 0, "aaaa");
        listItemService.add(listName, "cancelled child", nullptr, &cancelled, 0, "aaaa");
        const size_t before = listItemService.get(listName).size();

        Command setAttached =
            Command("status", { "completed", "aaaa" }, { { "attached", "" } }, "status completed aaaa -a");
        SetStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(setAttached, listName));

        REQUIRE(listItemService.get(listName).size() == before - 3);
        configService.edit("archiveWhenCompleted", "false");

        installation.wipe();
        installation.make();
    }

    SECTION("set without --attached leaves the children alone")
    {
        listItemService.editStatus("aaaa", listName, new int(StatusService::PAUSED));
        std::string paused = "paused";
        std::string child = listItemService.add(listName, "child", nullptr, &paused, 0, "aaaa");

        Command setCommand = Command("status", { "blocked", "aaaa" }, {}, "status blocked aaaa");
        SetStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.execute(setCommand, listName));

        REQUIRE(*(*listItemService.find("aaaa", listName).status()).getName() == "blocked");
        REQUIRE(*(*listItemService.find(child, listName).status()).getName() == "paused");

        installation.wipe();
        installation.make();
    }

    SECTION("reset with valid ID resets item")
    {
        // First change status
        listItemService.editStatus("aaaa", listName, new int(StatusService::STARTED));

        Command resetCommand = Command("reset", { "aaaa" }, {}, "reset aaaa");
        EditStatusAction status(ioService, listItemService);
        REQUIRE_NOTHROW(status.executeReset(resetCommand, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "queued");

        installation.wipe();
        installation.make();
    }
}

TEST_CASE("Priority controller", "[CommandRouter][Priority]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest3");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("increase", { "aaaa" }, {}, "increase aaaa");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService;
    StatusService statusService;
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();

    SECTION("increase priority")
    {
        PriorityAction priority(ioService, listItemService);
        REQUIRE_NOTHROW(priority.execute(command, listName, "increase"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "urgent");

        installation.wipe();
        installation.make();
    }

    SECTION("decrease priority")
    {
        Command decCommand = Command("decrease", { "aaaa" }, {}, "decrease aaaa");
        PriorityAction priority(ioService, listItemService);
        REQUIRE_NOTHROW(priority.execute(decCommand, listName, "decrease"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "medium");

        installation.wipe();
        installation.make();
    }

    SECTION("increase with multiplier")
    {
        Command multiCommand = Command("increase", { "2", "aaaa" }, {}, "increase 2 aaaa");
        PriorityAction priority(ioService, listItemService);
        REQUIRE_NOTHROW(priority.execute(multiCommand, listName, "increase"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "critical");

        installation.wipe();
        installation.make();
    }

    SECTION("set priority")
    {
        Command setCommand = Command("priority", { "low", "aaaa" }, {}, "priority low aaaa");
        PriorityAction priority(ioService, listItemService);
        REQUIRE_NOTHROW(priority.execute(setCommand, listName, "set"));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "low");

        installation.wipe();
        installation.make();
    }

    SECTION("set with empty arguments does not throw")
    {
        Command emptyCommand = Command("priority", {}, {}, "priority");
        PriorityAction priority(ioService, listItemService);
        REQUIRE_NOTHROW(priority.execute(emptyCommand, listName, "set"));
    }

    SECTION("set with missing ID does not throw")
    {
        Command oneArgCommand = Command("priority", { "high" }, {}, "priority high");
        PriorityAction priority(ioService, listItemService);
        REQUIRE_NOTHROW(priority.execute(oneArgCommand, listName, "set"));
    }
}

TEST_CASE("AddItemAction controller", "[CommandRouter][AddItemAction]")
{
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_CommandRouterTest4");
    MockAppInstallation installation(ioService, jsonService, confService, init);

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);

    SECTION("add via AddItemAction")
    {
        Command addCommand = Command("add", { "new", "test", "item" }, {}, "add new test item");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(addCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        // Find the new item
        bool found = false;
        for (auto& item : items) {
            if (*item.getValue() == "new test item") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }

    SECTION("add --to {id} adds a child; an unknown parent adds nothing")
    {
        Command addCommand = Command("add", { "child" }, { { "to", "aaaa" } }, "add child --to aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        std::string id = actions.execute(addCommand, listName);
        REQUIRE(*listItemService.find(id, listName).getParentId() == "aaaa");

        Command orphanCommand = Command("add", { "orphan" }, { { "to", "zzzz" } }, "add orphan --to zzzz");
        REQUIRE(actions.execute(orphanCommand, listName).empty());
        REQUIRE(listItemService.get(listName).size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("add with empty arguments does not throw")
    {
        Command emptyAddCommand = Command("add", {}, {}, "add");
        ConfigService configService(ioService, init, configRepository, cacheRepository, emptyAddCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(emptyAddCommand, listName));

        // No item should have been added
        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 2);
    }

    SECTION("add with priority option")
    {
        std::map<std::string, std::string> options = { { "priority", "critical" } };
        Command addWithPrio = Command("add", { "urgent", "task" }, options, "add urgent task -p critical");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addWithPrio);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(addWithPrio, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    SECTION("calculateValue joins arguments")
    {
        // This is tested indirectly via add - the value should be "word1 word2 word3"
        Command addCommand = Command("add", { "word1", "word2", "word3" }, {}, "add word1 word2 word3");
        ConfigService configService(ioService, init, configRepository, cacheRepository, addCommand);
        ListItemRepository listItemRepository(
            configService, fileDataStorageServicePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

        ListItemService listItemService(ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        ListRepository listRepository(configService, fileDataStorageServicePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createUsedListName();

        AddItemAction actions(ioService, commandService, listItemService);
        REQUIRE_NOTHROW(actions.execute(addCommand, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        bool found = false;
        for (auto& item : items) {
            if (*item.getValue() == "word1 word2 word3") {
                found = true;
                break;
            }
        }
        REQUIRE(found);

        installation.wipe();
        installation.make();
    }
}
