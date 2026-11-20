#include "../../Describe/DescriptionService.h"
#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListName.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
void
writeFile(const std::filesystem::path& path, const std::string& content)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path);
    file << content;
}

std::string
readFile(const std::filesystem::path& path)
{
    std::ifstream file(path);
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}
}

TEST_CASE("DescriptionService", "[Describe][DescriptionService]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);

    MockAppInitialization init(ioService, "_todoos_DescriptionServiceTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    DescriptionRepository descriptionRepository(init.getAppDirPath() / "descriptions");
    DescriptionService descriptionService(init.getCacheDirPath(), descriptionRepository);
    ListName oldListName("tempListName", "default");
    ListName newListName("tempList2Name", "default");

    SECTION("getDraftFilePath points to the cache describe directory")
    {
        std::filesystem::path path = descriptionService.getDraftFilePath("aaaa", oldListName);
        REQUIRE(path == init.getCacheDirPath() / "describe" / "tempListName" / "aaaa.md");
    }

    SECTION("getDraftFilePath differs from the real description path")
    {
        REQUIRE(descriptionService.getDraftFilePath("aaaa", oldListName) !=
                descriptionRepository.getFilePath("aaaa", oldListName));
    }

    SECTION("renameList moves all descriptions to the new list")
    {
        writeFile(descriptionRepository.getFilePath("aaaa", oldListName), "A.\n");
        writeFile(descriptionRepository.getFilePath("bbbb", oldListName), "B.\n");
        std::filesystem::remove_all(descriptionRepository.getListDirPath(newListName));

        REQUIRE_NOTHROW(descriptionService.renameList(oldListName, newListName));

        REQUIRE_FALSE(std::filesystem::exists(descriptionRepository.getListDirPath(oldListName)));
        REQUIRE(readFile(descriptionRepository.getFilePath("aaaa", newListName)) == "A.\n");
        REQUIRE(readFile(descriptionRepository.getFilePath("bbbb", newListName)) == "B.\n");

        installation.wipe();
        installation.make();
    }

    SECTION("renameList merges into an existing directory of the new list")
    {
        writeFile(descriptionRepository.getFilePath("aaaa", oldListName), "A.\n");
        writeFile(descriptionRepository.getFilePath("cccc", newListName), "C.\n");

        REQUIRE_NOTHROW(descriptionService.renameList(oldListName, newListName));

        REQUIRE_FALSE(std::filesystem::exists(descriptionRepository.getListDirPath(oldListName)));
        REQUIRE(descriptionRepository.exists("aaaa", newListName));
        REQUIRE(descriptionRepository.exists("cccc", newListName));

        installation.wipe();
        installation.make();
    }

    SECTION("renameList does nothing when the old list has no descriptions")
    {
        std::filesystem::remove_all(descriptionRepository.getListDirPath(oldListName));

        REQUIRE_NOTHROW(descriptionService.renameList(oldListName, newListName));
        REQUIRE(descriptionRepository.getIds(newListName).empty());
    }
}
