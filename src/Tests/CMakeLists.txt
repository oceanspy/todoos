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
        ../Helpers/BashStyle.cpp
        ../Help/Help.cpp
        ../IOService/IOService.cpp
        ../IOService/IOCliService/IOCliService.cpp
        ../Exception/ListNotFoundException.cpp
        ../Entities/ConfigEntity.cpp
        ../Entities/ListEntity.cpp
        ../Entities/ListItemEntity.cpp
        ../Entities/PriorityEntity.cpp
        ../Entities/StatusEntity.cpp
        ../FileDataStorage/ConfService.cpp
        ../FileDataStorage/JSONService.cpp
        ../FileDataStorage/CSVService.cpp
        ../FileStorage/FileStorageService.cpp
        ../Command/Command.cpp
        ../Command/CommandList.cpp
        ../Command/CommandOption.cpp
        ../Command/CommandValidation.cpp
        ../Command/CommandService.cpp
        ../Command/SmartCommand.cpp
        ../Command/CommandShortcut.cpp
        ../CLIAutocomplete/CLIAutocompleteService.cpp
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
        ../Init/InstallationInterface.h
        Help/HelpTest.cpp
        FileDataStorage/JSONServiceTest.cpp
        FileDataStorage/CSVServiceTest.cpp
        Entities/ConfigEntityTest.cpp
        Entities/ListItemEntitiyTest.cpp
        Entities/ListEntityTest.cpp
        Command/CommandTest.cpp
        Command/CommandListTest.cpp
        Command/CommandValidationTest.cpp
        Command/CommandServiceTest.cpp
        Command/SmartCommandTest.cpp
        Command/CommandShortcutServiceTest.cpp
        CLIAutocomplete/CLIAutocompleteServiceTest.cpp
        Mock/MockInstallation.cpp
        Mock/MockInstallation.h
        Mock/MockInit.cpp
        Mock/MockInit.h
        Config/ConfigRepositoryTest.cpp
        List/ListRepositoryTest.cpp
        List/ListItemServiceTest.cpp
        List/ListServiceTest.cpp
        List/ListNameTest.cpp
        List/ListItemRepositoryTest.cpp
        Init/InstallationTest.cpp
        FileDataStorage/ConfServiceTest.cpp
        FileStorage/FileStorageTest.cpp
)

target_link_libraries(TestsRun PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(TestsRun PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(TestsRun)
