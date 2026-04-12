cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

project(todoos VERSION 0.4.0)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 23)

add_executable(todoos main.cpp
        src/FileDataStorage/FileDataServiceInterface.h
        src/Command/CommandValidation.cpp
        src/Command/CommandValidation.h
        src/Help/Help.cpp
        src/Help/Help.h
        src/Command/Command.cpp
        src/Command/Command.h
        src/Command/CommandList.cpp
        src/Command/CommandList.h
        src/Command/CommandService.cpp
        src/Command/CommandService.h
        src/IOService/IOService.cpp
        src/IOService/IOService.h
        src/IOService/IOCliService/IOCliService.cpp
        src/IOService/IOCliService/IOCliService.h
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
        src/Init/Init.cpp
        src/Init/Init.h
        src/Init/Installation.cpp
        src/Init/Installation.h
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
        src/Actions/ListItemActions/ListItemActions.cpp
        src/Actions/ListItemActions/ListItemActions.h
        src/List/ListItemService.cpp
        src/List/ListItemService.h
        src/List/ListCountSummary.h
        src/FileDataStorageRepositories/ListItemRepository.cpp
        src/FileDataStorageRepositories/ListItemRepository.h
        src/FileDataStorage/CSVService.cpp
        src/FileDataStorage/CSVService.h
        src/Actions/Show/Show.cpp
        src/Actions/Show/Show.h
        src/CommandRouter/CommandRouter.cpp
        src/CommandRouter/CommandRouter.h
        src/Actions/Remove/Remove.cpp
        src/Actions/Remove/Remove.h
        src/Actions/Status/Status.cpp
        src/Actions/Status/Status.h
        src/Actions/Priority/Priority.cpp
        src/Actions/Priority/Priority.h
        src/FileDataStorage/JSONService.cpp
        src/FileDataStorage/JSONService.h
        src/Actions/Config/Config.cpp
        src/Actions/Config/Config.h
        src/Actions/ListActions/ListActions.cpp
        src/Actions/ListActions/ListActions.h
        src/Command/SmartCommand.cpp
        src/Command/SmartCommand.h
        src/FileDataStorageRepositories/ListRepository.cpp
        src/FileDataStorageRepositories/ListRepository.h
        src/Command/CommandShortcut.cpp
        src/Command/CommandShortcut.h
        src/Helpers/BashStyle.h
        src/UseCase/CommandAutoCompleteUseCase.cpp
        src/UseCase/CommandAutoCompleteUseCase.h
        src/Themes/Default/Default.cpp
        src/Themes/Default/Default.h
        src/Themes/Theme.h
        src/Themes/Theme.cpp
        src/Actions/Move/Move.cpp
        src/Actions/Move/Move.h
        src/Events/Event.cpp
        src/Events/Event.h
        src/Events/EventBus.cpp
        src/Events/EventBus.h
        src/Init/InitInterface.h
        src/Init/InstallationInterface.h
        src/Command/CommandOption.cpp
        src/Command/CommandOption.h
        src/Helpers/DateHelpers.cpp
        src/Helpers/DateHelpers.h
        src/Helpers/StringHelpers.cpp
        src/Helpers/StringHelpers.h
        src/FileDataStorage/ConfService.cpp
        src/FileDataStorage/ConfService.h
        src/Actions/Stats/Stats.cpp
        src/Actions/Stats/Stats.h
        src/Themes/ThemeService.cpp
        src/Themes/ThemeService.h
        src/Helpers/BashStyle.cpp
        src/UseCase/ShowUseCase.cpp
        src/UseCase/ShowUseCase.h
        src/UseCase/ListItemActionsUseCase.cpp
        src/UseCase/ListItemActionsUseCase.h
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
        src/UseCase/ListUseCase.cpp
        src/UseCase/ListUseCase.h
        src/UseCase/SwitchListUseCase.cpp
        src/UseCase/SwitchListUseCase.h
        src/UseCase/StatsUseCase.cpp
        src/UseCase/StatsUseCase.h
        src/UseCase/MoveUseCase.cpp
        src/UseCase/MoveUseCase.h
        src/UseCase/EmptyUseCase.cpp
        src/UseCase/EmptyUseCase.h
        src/UseCase/CleanUseCase.cpp
        src/UseCase/CleanUseCase.h
)

configure_file(src/version.h.in src/version.h)
target_include_directories(todoos PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(todoos PRIVATE nlohmann_json::nlohmann_json)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/Tests)
endif()
