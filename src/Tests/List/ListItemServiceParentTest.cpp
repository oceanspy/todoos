#include "../../List/ListItemService.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItemId.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <fstream>

TEST_CASE("ListItemServiceParentTest", "[ListItemService]")
{
    // Create mock objects
    IOService ioService("cli");
    ConfSerializer confService = ConfSerializer(ioService);
    JsonSerializer jsonService = JsonSerializer(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataStorageServicePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> fileDataConfigStorageServicePtr =
        std::make_unique<ConfSerializer>(ioService);
    MockAppInitialization init(ioService, "_todoos_ListItemServiceParentTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    std::string tempListNameStr = "tempListName";
    std::string tempList2NameStr = "tempList2Name";

    installation.wipe();
    installation.make();
    EventBus bus = EventBus();
    Command command = Command("", {}, {}, "");
    ConfigRepository configRepository(fileDataConfigStorageServicePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(fileDataConfigStorageServicePtr.get(), init.getCacheFilePath());
    ConfigService configService(ioService, init, configRepository, cacheRepository, command);

    PriorityService priorityService = PriorityService();
    StatusService statusService = StatusService();
    ListItemRepository listItemRepository(
        configService, fileDataStorageServicePtr.get(), priorityService, statusService);
    DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());

    ListItemService listItemService(
        ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
    ListRepository listRepository(configService, fileDataStorageServicePtr.get());
    ListService listService(ioService, configService, listRepository, bus);
    ListName listName = listService.createUsedListName();
    ListName listNameArchive = ListName::createVariant(listName, "archive");
    ListName listNameDelete = ListName::createVariant(listName, "delete");
    ListName tempListName = listService.createListName(tempListNameStr);
    ListName tempListNameArchive = ListName::createVariant(tempListName, "archive");
    ListName tempListNameDelete = ListName::createVariant(tempListName, "delete");
    ListName tempListName2 = listService.createListName(tempList2NameStr);
    ListName tempListName2Archive = ListName::createVariant(tempListName2, "archive");
    ListName tempListName2Delete = ListName::createVariant(tempListName2, "delete");

    // tempListName holds aaaa (high, queued) and bbbb (medium, started); tempList2Name holds cccc and dddd
    int completed = StatusService::COMPLETED;
    int queued = StatusService::QUEUED;
    auto parentOf = [&](const std::string& id, ListName& ln) { return *listItemService.find(id, ln).getParentId(); };
    auto idsOf = [&](ListName& ln) {
        std::vector<std::string> ids;
        for (const ListItemEntity& item : listItemRepository.get(ln)) {
            ids.push_back(*item.getId());
        }
        return ids;
    };
    auto writeDescription = [&descriptionRepository](const std::string& id, ListName& ln, const std::string& text) {
        std::filesystem::path descPath = descriptionRepository.getFilePath(id, ln);
        std::filesystem::create_directories(descPath.parent_path());
        std::ofstream(descPath) << text;
    };

    SECTION("attach / detach a child")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");
        REQUIRE(listItemService.isParent("aaaa", tempListName));
        REQUIRE(listItemService.getChildren("aaaa", tempListName).size() == 1);

        REQUIRE_THROWS_AS(listItemService.attach("bbbb", "aaaa", tempListName), std::invalid_argument);

        listItemService.detach("bbbb", tempListName);
        REQUIRE(parentOf("bbbb", tempListName).empty());
        REQUIRE_FALSE(listItemService.isParent("aaaa", tempListName));
        REQUIRE_THROWS_AS(listItemService.detach("bbbb", tempListName), std::invalid_argument);
    }

    SECTION("attach: an item attached to another parent switches only with force")
    {
        std::string otherParent = listItemService.add(tempListName, "other parent");
        listItemService.attach("bbbb", "aaaa", tempListName);

        REQUIRE_THROWS_AS(listItemService.attach("bbbb", otherParent, tempListName), std::invalid_argument);
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");

        listItemService.attach("bbbb", otherParent, tempListName, true);
        REQUIRE(parentOf("bbbb", tempListName) == otherParent);
        REQUIRE_FALSE(listItemService.isParent("aaaa", tempListName));

        // force does not lift the other rules
        REQUIRE_THROWS_AS(listItemService.attach("bbbb", otherParent, tempListName, true), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.attach("bbbb", "bbbb", tempListName, true), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.attach(otherParent, "aaaa", tempListName, true), std::invalid_argument);
    }

    SECTION("attach: one level only, same list, not itself")
    {
        REQUIRE_THROWS_AS(listItemService.attach("aaaa", "aaaa", tempListName), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.attach("aaaa", "cccc", tempListName), std::invalid_argument);

        listItemService.attach("bbbb", "aaaa", tempListName);
        std::string third = listItemService.add(tempListName, "third");
        // the target is a child
        REQUIRE_THROWS_AS(listItemService.attach(third, "bbbb", tempListName), std::invalid_argument);
        // the item has children
        REQUIRE_THROWS_AS(listItemService.attach("aaaa", third, tempListName), std::invalid_argument);
        REQUIRE(parentOf(third, tempListName).empty());
        REQUIRE(parentOf("aaaa", tempListName).empty());

        std::string archivedId = listItemService.archive(third, tempListName);
        REQUIRE_THROWS_AS(listItemService.attach(archivedId, "aaaa", tempListNameArchive), std::invalid_argument);
    }

    SECTION("add with a parent id")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        REQUIRE(parentOf(child, tempListName) == "aaaa");

        REQUIRE_THROWS_AS(listItemService.add(tempListName, "x", nullptr, nullptr, 0, "zzzz"), std::invalid_argument);
        REQUIRE_THROWS_AS(listItemService.add(tempListName, "x", nullptr, nullptr, 0, child), std::invalid_argument);
        REQUIRE(listItemService.get(tempListName).size() == 3);
    }

    SECTION("detachChildren detaches every child")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.attach("bbbb", "aaaa", tempListName);
        REQUIRE(listItemService.detachChildren("aaaa", tempListName) == 2);
        REQUIRE(parentOf(child, tempListName).empty());
        REQUIRE(parentOf("bbbb", tempListName).empty());
        REQUIRE(listItemService.detachChildren("aaaa", tempListName) == 0);
    }

    SECTION("archive a parent: children follow, attached to its new id; restore brings them back")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        writeDescription("bbbb", tempListName, "child description");

        std::string archivedParent = listItemService.archive("aaaa", tempListName);
        REQUIRE(listItemService.get(tempListName).empty());

        std::vector<ListItemEntity> archived = listItemService.getChildren(archivedParent, tempListNameArchive);
        REQUIRE(archived.size() == 1);
        std::string archivedChild = *archived[0].getId();
        REQUIRE(ListItemId::baseOf(archivedChild) == "bbbb");
        REQUIRE(descriptionRepository.exists(archivedChild, tempListName));

        REQUIRE(listItemService.restore(archivedParent, tempListName) == "aaaa");
        REQUIRE(listItemService.get(tempListNameArchive).empty());
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");
        REQUIRE(descriptionRepository.exists("bbbb", tempListName));
    }

    SECTION("soft delete a parent: children follow; archive <-> delete keeps the links")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        std::string archivedParent = listItemService.archive("aaaa", tempListName);
        std::string deletedParent = listItemService.moveToVariant(archivedParent, tempListNameArchive, tempListNameDelete);
        REQUIRE(deletedParent == archivedParent);
        REQUIRE(listItemService.getChildren(deletedParent, tempListNameDelete).size() == 1);

        listItemService.restore(deletedParent, tempListName);
        std::string deletedAgain = listItemService.softDelete("aaaa", tempListName);
        REQUIRE(listItemService.getChildren(deletedAgain, tempListNameDelete).size() == 1);
        REQUIRE(listItemService.get(tempListName).empty());
    }

    SECTION("a child archived on its own is restored under its parent, or top-level when the parent is gone")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        std::string archivedChild = listItemService.archive("bbbb", tempListName);
        REQUIRE(parentOf("aaaa", tempListName).empty());
        REQUIRE(listItemService.restore(archivedChild, tempListName) == "bbbb");
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");

        archivedChild = listItemService.archive("bbbb", tempListName);
        listItemService.removePermanently("aaaa", tempListName);
        listItemService.restore(archivedChild, tempListName);
        REQUIRE(parentOf("bbbb", tempListName).empty());
    }

    // ---- links of children archived / deleted on their own ----------------------------------

    auto variantParentOf = [&](const std::string& baseId, ListName& variantListName) {
        for (const ListItemEntity& item : listItemRepository.get(variantListName)) {
            if (ListItemId::baseOf(*item.getId()) == baseId) {
                return *item.getParentId();
            }
        }
        FAIL("no item with base id " + baseId);
        return std::string();
    };

    SECTION("a lone archived child follows its parent into the archive, and comes back with it")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        listItemService.archive("bbbb", tempListName);
        std::string archivedParent = listItemService.archive("aaaa", tempListName);

        REQUIRE(variantParentOf("bbbb", tempListNameArchive) == archivedParent);
        REQUIRE(listItemService.getChildren(archivedParent, tempListNameArchive).size() == 1);

        listItemService.restore(archivedParent, tempListName);
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");
        REQUIRE(listItemService.get(tempListNameArchive).empty());
    }

    SECTION("a lone deleted child follows its parent through archive and restore")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        std::string deletedChild = listItemService.softDelete("bbbb", tempListName);
        std::string archivedParent = listItemService.archive("aaaa", tempListName);
        REQUIRE(variantParentOf("bbbb", tempListNameDelete) == archivedParent);

        // restored alone: the deleted child stays in the delete list but points to the active parent
        std::string restoredParent = listItemService.restore(archivedParent, tempListName);
        REQUIRE(variantParentOf("bbbb", tempListNameDelete) == restoredParent);

        listItemService.restore(deletedChild, tempListName);
        REQUIRE(parentOf("bbbb", tempListName) == restoredParent);
    }

    SECTION("a parent deleted for good clears the link: a new item with its id does not adopt the child")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        std::string archivedChild = listItemService.archive("bbbb", tempListName);
        listItemService.removePermanently("aaaa", tempListName);
        REQUIRE(variantParentOf("bbbb", tempListNameArchive).empty());

        // a new, unrelated item gets the freed id
        ListItemEntity reused = listItemRepository.find("cccc", tempListName2);
        reused.setId("aaaa");
        listItemRepository.create(reused, tempListName);

        listItemService.restore(archivedChild, tempListName);
        REQUIRE(parentOf("bbbb", tempListName).empty());
    }

    SECTION("a parent moved to another list clears the link of its archived children")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        listItemService.archive("bbbb", tempListName);
        listItemService.move("aaaa", tempListName, tempListName2);
        REQUIRE(variantParentOf("bbbb", tempListNameArchive).empty());
    }

    SECTION("detachChildren also detaches the archived / deleted children")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.attach("bbbb", "aaaa", tempListName);
        listItemService.archive("bbbb", tempListName);
        listItemService.softDelete(child, tempListName);

        REQUIRE(listItemService.detachChildren("aaaa", tempListName) == 0);
        REQUIRE(variantParentOf("bbbb", tempListNameArchive).empty());
        REQUIRE(variantParentOf(child, tempListNameDelete).empty());
    }

    SECTION("removePermanently removes the children and their descriptions")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        writeDescription("bbbb", tempListName, "child description");
        REQUIRE(listItemService.removePermanently("aaaa", tempListName) == std::vector<std::string>{ "bbbb" });
        REQUIRE(listItemService.get(tempListName).empty());
        REQUIRE(listItemService.get(tempListNameDelete).empty());
        REQUIRE_FALSE(descriptionRepository.exists("bbbb", tempListName));
        REQUIRE_THROWS(listItemService.removePermanently("aaaa", tempListName));
    }

    SECTION("move a parent: children follow, a colliding child id is replaced")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        ListItemEntity colliding = listItemRepository.find("cccc", tempListName2);
        colliding.setId("bbbb");
        listItemRepository.create(colliding, tempListName2);

        auto movedChildren = listItemService.move("aaaa", tempListName, tempListName2);
        REQUIRE(movedChildren.size() == 1);
        REQUIRE(movedChildren[0].first == "bbbb");
        std::string newChildId = movedChildren[0].second;
        REQUIRE(newChildId != "bbbb");

        REQUIRE(listItemService.get(tempListName).empty());
        REQUIRE(parentOf(newChildId, tempListName2) == "aaaa");
        REQUIRE(parentOf("bbbb", tempListName2).empty()); // the existing item is untouched
        REQUIRE(listItemService.get(tempListName2).size() == 5);
    }

    SECTION("move / copy a child on its own: it becomes top-level in the target list")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        listItemService.copy("bbbb", tempListName, tempListName2);
        REQUIRE(parentOf("bbbb", tempListName2).empty());
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");

        REQUIRE(listItemService.move("bbbb", tempListName, tempListName2).empty());
        REQUIRE(parentOf("bbbb", tempListName2).empty());
        REQUIRE_FALSE(listItemService.isParent("aaaa", tempListName));
    }

    SECTION("copy a parent: children are copied too")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        auto copiedChildren = listItemService.copy("aaaa", tempListName, tempListName2);
        REQUIRE(copiedChildren.size() == 1);
        REQUIRE(parentOf(copiedChildren[0].second, tempListName2) == "aaaa");
        REQUIRE(parentOf("bbbb", tempListName) == "aaaa");
    }

    SECTION("overwriting a parent in the target list detaches its children there")
    {
        listItemService.attach("dddd", "cccc", tempListName2);
        ListItemEntity sameId = listItemRepository.find("aaaa", tempListName);
        sameId.setId("cccc");
        listItemRepository.create(sameId, tempListName);

        listItemService.move("cccc", tempListName, tempListName2);
        REQUIRE(parentOf("dddd", tempListName2).empty());
    }

    SECTION("clean: children and parents with children stay without --attached")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.editStatus("aaaa", tempListName, &completed);
        listItemService.editStatus(child, tempListName, &completed);
        std::string lonely = listItemService.add(tempListName, "lonely");
        listItemService.editStatus(lonely, tempListName, &completed);

        listItemService.archiveFinishedItems(tempListName, false);
        std::vector<std::string> ids = idsOf(tempListName);
        REQUIRE(ids.size() == 3);
        REQUIRE(std::find(ids.begin(), ids.end(), lonely) == ids.end());
    }

    SECTION("clean --attached: finished children go; a finished parent goes once no child is open")
    {
        std::string child1 = listItemService.add(tempListName, "child 1", nullptr, nullptr, 0, "aaaa");
        std::string child2 = listItemService.add(tempListName, "child 2", nullptr, nullptr, 0, "aaaa");
        listItemService.editStatus("aaaa", tempListName, &completed);
        listItemService.editStatus(child1, tempListName, &completed);

        listItemService.archiveFinishedItems(tempListName, false, true);
        std::vector<std::string> ids = idsOf(tempListName);
        REQUIRE(ids.size() == 3); // aaaa (child 2 is open), bbbb, child 2
        REQUIRE(std::find(ids.begin(), ids.end(), child1) == ids.end());

        listItemService.editStatus(child2, tempListName, &completed);
        listItemService.archiveFinishedItems(tempListName, false, true);
        REQUIRE(idsOf(tempListName) == std::vector<std::string>{ "bbbb" });

        // child 2 went with its parent and is attached to the parent's archive id
        for (const ListItemEntity& item : listItemRepository.get(tempListNameArchive)) {
            if (ListItemId::baseOf(*item.getId()) == child2) {
                REQUIRE(ListItemId::baseOf(*item.getParentId()) == "aaaa");
                REQUIRE((*item.getParentId()).length() == 5);
            }
        }
    }

    SECTION("clean --attached keeps a group with a described child unless --described")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.editStatus("aaaa", tempListName, &completed);
        listItemService.editStatus(child, tempListName, &completed);
        writeDescription(child, tempListName, "notes");

        listItemService.archiveFinishedItems(tempListName, false, true);
        REQUIRE(idsOf(tempListName).size() == 3);

        listItemService.archiveFinishedItems(tempListName, true, true);
        REQUIRE(idsOf(tempListName) == std::vector<std::string>{ "bbbb" });
    }

    SECTION("archiveAll archives parents with their children")
    {
        listItemService.attach("bbbb", "aaaa", tempListName);
        listItemService.archiveAll(tempListName);
        REQUIRE(listItemService.get(tempListName).empty());
        REQUIRE(listItemService.get(tempListNameArchive).size() == 2);
    }

    SECTION("auto-archive on completion waits for the open children, then archives the group")
    {
        configService.edit("archiveWhenCompleted", "true");
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");

        listItemService.editStatus("aaaa", tempListName, &completed);
        REQUIRE(idsOf(tempListName).size() == 3);

        listItemService.editStatus(child, tempListName, &completed);
        REQUIRE(idsOf(tempListName) == std::vector<std::string>{ "bbbb" });
        REQUIRE(listItemService.get(tempListNameArchive).size() == 2);
        configService.edit("archiveWhenCompleted", "false");
    }

    SECTION("auto-archive keeps a completed child while its parent is open")
    {
        configService.edit("archiveWhenCompleted", "true");
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.editStatus(child, tempListName, &completed);
        REQUIRE(idsOf(tempListName).size() == 3);
        listItemService.editStatus(child, tempListName, &queued);
        configService.edit("archiveWhenCompleted", "false");
    }

    SECTION("withoutChildrenOfListedParents drops the children of listed parents only")
    {
        std::string child = listItemService.add(tempListName, "child", nullptr, nullptr, 0, "aaaa");
        listItemService.attach("bbbb", "aaaa", tempListName);
        REQUIRE(listItemService.withoutChildrenOfListedParents({ child, "aaaa", "zzzz" }, tempListName) ==
                std::vector<std::string>{ "aaaa", "zzzz" });
        REQUIRE(listItemService.withoutChildrenOfListedParents({ child, "bbbb" }, tempListName) ==
                std::vector<std::string>{ child, "bbbb" });
    }

    installation.wipe();
}
