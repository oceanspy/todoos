cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

project(todoos VERSION 0.4.1)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 23)

add_executable(todoos main.cpp
        src/Serializers/DataSerializerInterface.h
        src/Command/CommandValidation.cpp
        src/Command/CommandValidation.h
        src/Help/HelpPrinter.cpp
        src/Help/HelpPrinter.h
        src/Command/Command.cpp
        src/Command/Command.h
        src/Command/CommandRegistry.cpp
        src/Command/CommandRegistry.h
        src/Command/CommandService.cpp
        src/Command/CommandService.h
        src/IOService/IOService.cpp
        src/IOService/IOService.h
        src/IOService/CliIOChannel/CliIOChannel.cpp
        src/IOService/CliIOChannel/CliIOChannel.h
        src/List/ListItems/ListItemEntity.cpp
        src/List/ListItems/ListItemEntity.h
        src/List/ListItems/PriorityEntity.cpp
        src/List/ListItems/PriorityEntity.h
        src/List/ListItems/StatusEntity.cpp
        src/List/ListItems/StatusEntity.h
        src/List/ListItems/PriorityService.cpp
        src/List/ListItems/PriorityService.h
        src/List/ListItems/StatusService.cpp
        src/List/ListItems/StatusService.h
        src/List/ListEntity.cpp
        src/List/ListEntity.h
        src/List/ListService.cpp
        src/List/ListService.h
        src/List/ListName.cpp
        src/List/ListName.h
        src/Init/AppInitialization.cpp
        src/Init/AppInitialization.h
        src/Init/AppInstallation.cpp
        src/Init/AppInstallation.h
        src/Exception/ListNotFoundException.cpp
        src/Exception/ListNotFoundException.h
        src/Config/ConfigEntity.cpp
        src/Config/ConfigEntity.h
        src/Config/ConfigService.cpp
        src/Config/ConfigService.h
        src/FileStorage/FileStorageService.cpp
        src/FileStorage/FileStorageService.h
        src/FileDataStorageRepositories/ConfigRepository.cpp
        src/FileDataStorageRepositories/ConfigRepository.h
        src/Actions/ListItemAction/AddItemAction.cpp
        src/Actions/ListItemAction/AddItemAction.h
        src/Actions/ListItemAction/AppendItemAction.cpp
        src/Actions/ListItemAction/AppendItemAction.h
        src/Actions/ListItemAction/DeadlineItemAction.cpp
        src/Actions/ListItemAction/DeadlineItemAction.h
        src/Actions/ListItemAction/EditItemAction.cpp
        src/Actions/ListItemAction/EditItemAction.h
        src/Actions/ListItemAction/PrependItemAction.cpp
        src/Actions/ListItemAction/PrependItemAction.h
        src/List/ListItemService.cpp
        src/List/ListItemService.h
        src/List/ListCountSummary.h
        src/FileDataStorageRepositories/ListItemRepository.cpp
        src/FileDataStorageRepositories/ListItemRepository.h
        src/Serializers/CsvSerializer.cpp
        src/Serializers/CsvSerializer.h
        src/Actions/ShowAction/ShowAction.cpp
        src/Actions/ShowAction/ShowAction.h
        src/Command/CommandRouter.cpp
        src/Command/CommandRouter.h
        src/Actions/RemoveAction/RemoveAction.cpp
        src/Actions/RemoveAction/RemoveAction.h
        src/Actions/StatusAction/StatusAction.cpp
        src/Actions/StatusAction/StatusAction.h
        src/Actions/PriorityAction/PriorityAction.cpp
        src/Actions/PriorityAction/PriorityAction.h
        src/Serializers/JsonSerializer.cpp
        src/Serializers/JsonSerializer.h
        src/Actions/ConfigAction/ConfigAction.cpp
        src/Actions/ConfigAction/ConfigAction.h
        src/Actions/ListAction/ShowListAction.cpp
        src/Actions/ListAction/ShowListAction.h
        src/Actions/ListAction/AddListAction.cpp
        src/Actions/ListAction/AddListAction.h
        src/Actions/ListAction/RemoveListAction.cpp
        src/Actions/ListAction/RemoveListAction.h
        src/Actions/ListAction/RenameListAction.cpp
        src/Actions/ListAction/RenameListAction.h
        src/Actions/ListAction/CopyListAction.cpp
        src/Actions/ListAction/CopyListAction.h
        src/Actions/ListAction/MoveListAction.cpp
        src/Actions/ListAction/MoveListAction.h
        src/Command/SmartCommand.cpp
        src/Command/SmartCommand.h
        src/FileDataStorageRepositories/ListRepository.cpp
        src/FileDataStorageRepositories/ListRepository.h
        src/Command/CommandShortcut.cpp
        src/Command/CommandShortcut.h
        src/Helpers/TerminalStyle.h
        src/UseCase/CommandAutoCompleteUseCase.cpp
        src/UseCase/CommandAutoCompleteUseCase.h
        src/Themes/Default/Default.cpp
        src/Themes/Default/Default.h
        src/Themes/Theme.h
        src/Themes/Theme.cpp
        src/Actions/MoveAction/MoveItemAction.cpp
        src/Actions/MoveAction/MoveItemAction.h
        src/Actions/MoveAction/CopyItemAction.cpp
        src/Actions/MoveAction/CopyItemAction.h
        src/Actions/MoveAction/DuplicateItemAction.cpp
        src/Actions/MoveAction/DuplicateItemAction.h
        src/Events/Event.cpp
        src/Events/Event.h
        src/Events/EventBus.cpp
        src/Events/EventBus.h
        src/Init/AppInitializationInterface.h
        src/Init/AppInstallationInterface.h
        src/Command/CommandOption.cpp
        src/Command/CommandOption.h
        src/Helpers/DateHelpers.cpp
        src/Helpers/DateHelpers.h
        src/Helpers/StringHelpers.cpp
        src/Helpers/StringHelpers.h
        src/Serializers/ConfSerializer.cpp
        src/Serializers/ConfSerializer.h
        src/Actions/StatsAction/StatsAction.cpp
        src/Actions/StatsAction/StatsAction.h
        src/Themes/ThemeService.cpp
        src/Themes/ThemeService.h
        src/Helpers/TerminalStyle.cpp
        src/UseCase/ShowUseCase.cpp
        src/UseCase/ShowUseCase.h
        src/UseCase/AddItemUseCase.cpp
        src/UseCase/AddItemUseCase.h
        src/UseCase/EditItemUseCase.cpp
        src/UseCase/EditItemUseCase.h
        src/UseCase/AppendItemUseCase.cpp
        src/UseCase/AppendItemUseCase.h
        src/UseCase/PrependItemUseCase.cpp
        src/UseCase/PrependItemUseCase.h
        src/UseCase/DeadlineItemUseCase.cpp
        src/UseCase/DeadlineItemUseCase.h
        src/UseCase/FindUseCase.cpp
        src/UseCase/FindUseCase.h
        src/UseCase/PriorityUseCase.cpp
        src/UseCase/PriorityUseCase.h
        src/UseCase/ResetUseCase.cpp
        src/UseCase/ResetUseCase.h
        src/UseCase/StatusUseCase.cpp
        src/UseCase/StatusUseCase.h
        src/UseCase/RemoveUseCase.cpp
        src/UseCase/RemoveUseCase.h
        src/UseCase/ArchiveUseCase.cpp
        src/UseCase/ArchiveUseCase.h
        src/UseCase/RestoreUseCase.cpp
        src/UseCase/RestoreUseCase.h
        src/UseCase/ShowListUseCase.cpp
        src/UseCase/ShowListUseCase.h
        src/UseCase/AddListUseCase.cpp
        src/UseCase/AddListUseCase.h
        src/UseCase/RemoveListUseCase.cpp
        src/UseCase/RemoveListUseCase.h
        src/UseCase/RenameListUseCase.cpp
        src/UseCase/RenameListUseCase.h
        src/UseCase/CopyListUseCase.cpp
        src/UseCase/CopyListUseCase.h
        src/UseCase/MoveListUseCase.cpp
        src/UseCase/MoveListUseCase.h
        src/UseCase/SwitchListUseCase.cpp
        src/UseCase/SwitchListUseCase.h
        src/UseCase/StatsUseCase.cpp
        src/UseCase/StatsUseCase.h
        src/UseCase/MoveItemUseCase.cpp
        src/UseCase/MoveItemUseCase.h
        src/UseCase/CopyItemUseCase.cpp
        src/UseCase/CopyItemUseCase.h
        src/UseCase/DuplicateItemUseCase.cpp
        src/UseCase/DuplicateItemUseCase.h
        src/UseCase/EmptyUseCase.cpp
        src/UseCase/EmptyUseCase.h
        src/UseCase/CleanUseCase.cpp
        src/UseCase/CleanUseCase.h
        src/UseCase/CurrentListUseCase.cpp
        src/UseCase/CurrentListUseCase.h
)

configure_file(src/version.h.in src/version.h)
target_include_directories(todoos PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(todoos PRIVATE nlohmann_json::nlohmann_json)

# Improve binary by removing unused code from libraries
# target_compile_options(todoos PRIVATE -flto)
# target_link_options(todoos PRIVATE -flto)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/Tests)
endif()
