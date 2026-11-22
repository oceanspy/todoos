#include "../../Command/CommandRouter.h"
#include "../../Command/CommandOption.h"
#include "../../Command/CommandRegistry.h"
#include "../../Command/CommandService.h"
#include "../../Config/ConfigService.h"
#include "../../Describe/DescriptionService.h"
#include "../../Events/EventBus.h"
#include "../../FileDataStorageRepositories/ConfigRepository.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../FileDataStorageRepositories/ListItemRepository.h"
#include "../../FileDataStorageRepositories/ListRepository.h"
#include "../../FileStorage/FileStorageService.h"
#include "../../Help/HelpPrinter.h"
#include "../../List/ListItemService.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListService.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../../Themes/ThemeService.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <map>

TEST_CASE("CommandRouter", "[CommandRouter]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);
    std::unique_ptr<DataSerializerInterface> storagePtr = std::make_unique<JsonSerializer>(ioService);
    std::unique_ptr<DataSerializerInterface> configStoragePtr = std::make_unique<ConfSerializer>(ioService);

    MockAppInitialization init(ioService, "_todoos_CommandRouterRouteTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    EventBus bus;
    CommandRegistry commandList;
    CommandOption commandOption;
    CommandService commandService(commandList, commandOption);
    HelpPrinter help(ioService);

    ConfigRepository configRepository(configStoragePtr.get(), init.getConfigFilePath());
    ConfigRepository cacheRepository(configStoragePtr.get(), init.getCacheFilePath());

    PriorityService priorityService;
    StatusService statusService;

    // ---- show ------------------------------------------------------------------

    SECTION("show command routes to ShowUseCase without throwing")
    {
        Command command("show", {}, {}, "show");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListName listName = listService.createUsedListName();
        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_NOTHROW(router.execute(command, listName));
    }

    // ---- add -------------------------------------------------------------------

    SECTION("add command routes to ListItemActionsUseCase and adds item")
    {
        Command command("add", { "routed", "item" }, {}, "add routed item");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_NOTHROW(router.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 3);

        installation.wipe();
        installation.make();
    }

    // ---- remove ----------------------------------------------------------------

    SECTION("remove command routes to RemoveUseCase and removes item")
    {
        Command command("remove", { "aaaa" }, {}, "remove aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_NOTHROW(router.execute(command, listName));

        std::vector<ListItemEntity> items = listItemService.get(listName);
        REQUIRE(items.size() == 1);

        installation.wipe();
        installation.make();
    }

    // ---- start -----------------------------------------------------------------

    SECTION("start command routes to StatusUseCase and changes status")
    {
        Command command("start", { "aaaa" }, {}, "start aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_NOTHROW(router.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.status()).getCommandName() == "started");

        installation.wipe();
        installation.make();
    }

    // ---- increase --------------------------------------------------------------

    SECTION("increase command routes to PriorityUseCase and raises priority")
    {
        Command command("increase", { "aaaa" }, {}, "increase aaaa");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();

        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_NOTHROW(router.execute(command, listName));

        ListItemEntity item = listItemService.find("aaaa", listName);
        REQUIRE(*(*item.priority()).getName() == "urgent");

        installation.wipe();
        installation.make();
    }

    // ---- stats -----------------------------------------------------------------

    SECTION("stats command routes to StatsUseCase without throwing")
    {
        Command command("stats", {}, {}, "stats");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListName listName = listService.createUsedListName();
        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_NOTHROW(router.execute(command, listName));
    }

    // ---- unknown command -------------------------------------------------------

    SECTION("unknown command throws std::invalid_argument")
    {
        // CommandRegistry::getCommandId throws for unregistered command names
        Command command("unknowncommandxyz", {}, {}, "unknowncommandxyz");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);

        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);

        ListName listName = listService.createUsedListName();
        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        REQUIRE_THROWS_AS(router.execute(command, listName), std::invalid_argument);
    }

    // ---- parent / child and reassign, routed as typed --------------------------------

    // Runs a command through the router, with fresh services (like a new todoos call)
    auto route = [&](Command command) {
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        DescriptionRepository descriptionRepository(configService.getDescriptionsDirPath());
        DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
        ListItemService listItemService(
            ioService, configService, listItemRepository, descriptionRepository, priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        FileStorageService fileStorageService(ioService, configService);
        ThemeService themeService(ioService, configService, listService, listItemService);
        ListName listName = listService.createUsedListName();
        CommandRouter router(ioService,
                             help,
                             commandService,
                             configService,
                             fileStorageService,
                             listService,
                             listItemService,
                             themeService,
                             descriptionRepository,
                             descriptionService);
        router.execute(command, listName);
    };
    // Reads the stored items of a list with fresh services: {id -> parent id}
    auto parentsIn = [&](const std::string& listNameStr) {
        Command command("show", {}, {}, "show");
        ConfigService configService(ioService, init, configRepository, cacheRepository, command);
        ListItemRepository listItemRepository(configService, storagePtr.get(), priorityService, statusService);
        std::unique_ptr<DataSerializerInterface> listStoragePtr = std::make_unique<JsonSerializer>(ioService);
        ListRepository listRepository(configService, listStoragePtr.get());
        ListService listService(ioService, configService, listRepository, bus);
        ListName listName = listService.createListName(listNameStr);
        std::map<std::string, std::string> parents;
        for (const ListItemEntity& item : listItemRepository.get(listName)) {
            parents[*item.getId()] = *item.getParentId();
        }
        return parents;
    };
    const std::string usedList = "tempListName"; // the mock installation's active list

    SECTION("add --to, attach, attach -f and detach -f, routed as typed")
    {
        route(Command("add", { "child" }, { { "to", "aaaa" } }, "add child --to aaaa"));
        auto parents = parentsIn(usedList);
        REQUIRE(parents.size() == 3);
        REQUIRE(std::count_if(parents.begin(), parents.end(), [](const auto& p) { return p.second == "aaaa"; }) == 1);

        route(Command("attach", { "bbbb" }, { { "to", "aaaa" } }, "attach bbbb --to aaaa"));
        REQUIRE(parentsIn(usedList)["bbbb"] == "aaaa");

        // already attached: refused without -f
        route(Command("add", { "other" }, {}, "add other"));
        std::string other;
        for (const auto& [id, parent] : parentsIn(usedList)) {
            if (id != "aaaa" && id != "bbbb" && parent.empty()) {
                other = id;
            }
        }
        REQUIRE_FALSE(other.empty());
        route(Command("attach", { "bbbb" }, { { "to", other } }, "attach bbbb --to " + other));
        REQUIRE(parentsIn(usedList)["bbbb"] == "aaaa");
        route(Command("attach", { "bbbb" }, { { "to", other }, { "force", "" } }, "attach bbbb --to " + other + " -f"));
        REQUIRE(parentsIn(usedList)["bbbb"] == other);

        // detach a parent: refused without -f
        route(Command("detach", { "aaaa" }, {}, "detach aaaa"));
        parents = parentsIn(usedList);
        REQUIRE(std::count_if(parents.begin(), parents.end(), [](const auto& p) { return p.second == "aaaa"; }) == 1);
        route(Command("detach", { "aaaa" }, { { "force", "" } }, "detach aaaa -f"));
        parents = parentsIn(usedList);
        REQUIRE(std::count_if(parents.begin(), parents.end(), [](const auto& p) { return p.second == "aaaa"; }) == 0);

        installation.wipe();
        installation.make();
    }

    SECTION("remove a parent: refused without -f, the group goes with -f")
    {
        route(Command("attach", { "bbbb" }, { { "to", "aaaa" } }, "attach bbbb --to aaaa"));
        route(Command("remove", { "aaaa" }, {}, "remove aaaa"));
        REQUIRE(parentsIn(usedList).size() == 2);

        route(Command("rm", { "aaaa" }, { { "force", "" } }, "rm aaaa -f"));
        REQUIRE(parentsIn(usedList).empty());

        installation.wipe();
        installation.make();
    }

    SECTION("move a parent: its child follows to the target list")
    {
        route(Command("attach", { "bbbb" }, { { "to", "aaaa" } }, "attach bbbb --to aaaa"));
        route(Command("mv", { "aaaa" }, { { "to", "tempList2Name" } }, "mv aaaa --to tempList2Name"));
        REQUIRE(parentsIn(usedList).empty());
        auto target = parentsIn("tempList2Name");
        REQUIRE(target.size() == 4);
        REQUIRE(target["aaaa"].empty());
        REQUIRE(target["bbbb"] == "aaaa");

        installation.wipe();
        installation.make();
    }

    SECTION("add --id, routed as typed: a free id is used, a taken one is replaced")
    {
        route(Command("add", { "server" }, { { "id", "SRV1" } }, "add server --id SRV1"));
        REQUIRE(parentsIn(usedList).count("SRV1") == 1);

        // taken: the item is still added, with a generated id
        route(Command("add", { "other" }, { { "id", "SRV1" } }, "add other --id SRV1"));
        REQUIRE(parentsIn(usedList).size() == 4);

        // badly formatted: refused, nothing added
        route(Command("add", { "bad" }, { { "id", "ab" } }, "add bad --id ab"));
        REQUIRE(parentsIn(usedList).size() == 4);

        // with --to: a child with a chosen id
        route(Command("add", { "child" }, { { "id", "CHD1" }, { "to", "SRV1" } }, "add child --id CHD1 --to SRV1"));
        REQUIRE(parentsIn(usedList)["CHD1"] == "SRV1");

        installation.wipe();
        installation.make();
    }

    SECTION("reassign --to, routed as typed: the children follow the new id")
    {
        route(Command("attach", { "bbbb" }, { { "to", "aaaa" } }, "attach bbbb --to aaaa"));
        route(Command("reassign", { "aaaa" }, { { "to", "SRV1" } }, "reassign aaaa --to SRV1"));
        auto parents = parentsIn(usedList);
        REQUIRE(parents.count("aaaa") == 0);
        REQUIRE(parents.count("SRV1") == 1);
        REQUIRE(parents["bbbb"] == "SRV1");

        // refused: id taken
        route(Command("reassign", { "SRV1" }, { { "to", "bbbb" } }, "reassign SRV1 --to bbbb"));
        REQUIRE(parentsIn(usedList).count("SRV1") == 1);

        installation.wipe();
        installation.make();
    }
}
