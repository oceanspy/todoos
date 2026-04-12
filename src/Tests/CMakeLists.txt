cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
find_package(Catch2 3 REQUIRED)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(TestsRun Helpers/HelpersTest.cpp
        IOService/IOServiceTest.cpp
        ../Helpers/DateHelpers.cpp
        ../Helpers/StringHelpers.cpp
        ../Helpers/TerminalStyle.cpp
        ../Help/HelpPrinter.cpp
        ../IOService/IOService.cpp
        ../IOService/CliIOChannel/CliIOChannel.cpp
        ../Exception/ListNotFoundException.cpp
        ../Config/ConfigEntity.cpp
        ../List/ListEntity.cpp
        ../List/ListItems/ListItemEntity.cpp
        ../List/ListItems/PriorityEntity.cpp
        ../List/ListItems/StatusEntity.cpp
        ../Serializers/ConfSerializer.cpp
        ../Serializers/JsonSerializer.cpp
        ../Serializers/CsvSerializer.cpp
        ../FileStorage/FileStorageService.cpp
        ../Command/Command.cpp
        ../Command/CommandRegistry.cpp
        ../Command/CommandOption.cpp
        ../Command/CommandValidation.cpp
        ../Command/CommandService.cpp
        ../Command/SmartCommand.cpp
        ../Command/CommandShortcut.cpp
        ../UseCase/CommandAutoCompleteUseCase.cpp
        ../List/ListItems/PriorityService.cpp
        ../List/ListItems/StatusService.cpp
        ../List/ListService.cpp
        ../List/ListName.cpp
        ../List/ListItemService.cpp
        ../List/ListItems/PriorityService.cpp
        ../List/ListItems/StatusService.cpp
        ../Config/ConfigService.cpp
        ../FileDataStorageRepositories/ConfigRepository.cpp
        ../FileDataStorageRepositories/ListRepository.cpp
        ../FileDataStorageRepositories/ListItemRepository.cpp
        ../Events/Event.cpp
        ../Events/Event.h
        ../Events/EventBus.cpp
        ../Events/EventBus.h
        ../Init/AppInstallationInterface.h
        Help/HelpPrinterTest.cpp
        Serializers/JsonSerializerTest.cpp
        Serializers/CsvSerializerTest.cpp
        Config/ConfigEntityTest.cpp
        List/ListItems/ListItemEntityTest.cpp
        List/ListEntityTest.cpp
        Command/CommandTest.cpp
        Command/CommandRegistryTest.cpp
        Command/CommandValidationTest.cpp
        Command/CommandServiceTest.cpp
        Command/SmartCommandTest.cpp
        Command/CommandShortcutServiceTest.cpp
        CommandAutocomplete/CommandAutocompleteServiceTest.cpp
        Mock/MockAppInstallation.cpp
        Mock/MockAppInstallation.h
        Mock/MockAppInitialization.cpp
        Mock/MockAppInitialization.h
        Config/ConfigRepositoryTest.cpp
        List/ListRepositoryTest.cpp
        List/ListItemServiceTest.cpp
        List/ListCountSummaryTest.cpp
        List/ListItemServiceCountSummaryTest.cpp
        List/ListServiceTest.cpp
        List/ListNameTest.cpp
        List/ListItemRepositoryTest.cpp
        Init/AppInstallationTest.cpp
        Serializers/ConfSerializerTest.cpp
        FileStorage/FileStorageTest.cpp
        Command/CommandOptionTest.cpp
        Events/EventTest.cpp
        List/ListItems/StatusEntityTest.cpp
        List/ListItems/PriorityEntityTest.cpp
        Helpers/TerminalStyleTest.cpp
        Exception/ListNotFoundExceptionTest.cpp
        Config/ConfigServiceTest.cpp
        Actions/ActionsTest.cpp
        Actions/ConfigTest.cpp
        Actions/ShowTest.cpp
        Actions/StatsTest.cpp
        Actions/MoveTest.cpp
        Actions/ListActionTest.cpp
        Actions/CommandRouterTest.cpp
        Themes/ThemeTest.cpp
        Themes/DefaultTest.cpp
        Themes/ThemeServiceTest.cpp
        ../CommandRouter/CommandRouter.cpp
        ../Actions/RemoveAction/RemoveAction.cpp
        ../Actions/StatusAction/StatusAction.cpp
        ../Actions/PriorityAction/PriorityAction.cpp
        ../Actions/ListItemAction/ListItemAction.cpp
        ../Actions/ShowAction/ShowAction.cpp
        ../Actions/ConfigAction/ConfigAction.cpp
        ../Actions/ListAction/ListAction.cpp
        ../Actions/MoveAction/MoveAction.cpp
        ../Actions/StatsAction/StatsAction.cpp
        ../Themes/ThemeService.cpp
        ../Themes/Theme.cpp
        ../Themes/Default/Default.cpp
        ../FileStorage/FileStorageService.cpp
        ../UseCase/ShowUseCase.cpp
        ../UseCase/ListItemActionsUseCase.cpp
        ../UseCase/FindUseCase.cpp
        ../UseCase/PriorityUseCase.cpp
        ../UseCase/ResetUseCase.cpp
        ../UseCase/StatusUseCase.cpp
        ../UseCase/RemoveUseCase.cpp
        ../UseCase/ArchiveUseCase.cpp
        ../UseCase/RestoreUseCase.cpp
        ../UseCase/ListUseCase.cpp
        ../UseCase/SwitchListUseCase.cpp
        ../UseCase/StatsUseCase.cpp
        ../UseCase/MoveUseCase.cpp
        ../UseCase/EmptyUseCase.cpp
        ../UseCase/CleanUseCase.cpp
        UseCase/UseCaseTest.cpp
        TestMain.cpp
)

target_include_directories(TestsRun PRIVATE ${CMAKE_BINARY_DIR}/src)

target_link_libraries(TestsRun PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TestsRun PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(TestsRun)
