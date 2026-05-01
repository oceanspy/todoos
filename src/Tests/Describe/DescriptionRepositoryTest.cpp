#include "../../FileDataStorageRepositories/DescriptionRepository.h"
#include "../../IOService/IOService.h"
#include "../../List/ListName.h"
#include "../../Serializers/ConfSerializer.h"
#include "../../Serializers/JsonSerializer.h"
#include "../Mock/MockAppInitialization.h"
#include "../Mock/MockAppInstallation.h"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

TEST_CASE("DescriptionRepository", "[Describe][DescriptionRepository]")
{
    IOService ioService("cli");
    ConfSerializer confService(ioService);
    JsonSerializer jsonService(ioService);

    MockAppInitialization init(ioService, "_todoos_DescriptionRepositoryTest");
    MockAppInstallation installation(ioService, jsonService, confService, init);
    installation.wipe();
    installation.make();

    std::filesystem::path descriptionsDir = init.getAppDirPath() / "descriptions";
    DescriptionRepository descriptionRepository(descriptionsDir);
    ListName listName("tempListName", "default");

    SECTION("getFilePath returns correct path structure")
    {
        std::filesystem::path path = descriptionRepository.getFilePath("aaaa", listName);
        REQUIRE(path.filename() == "aaaa.md");
        REQUIRE(path.parent_path().filename() == "tempListName");
        REQUIRE(path.parent_path().parent_path().filename() == "descriptions");
    }

    SECTION("exists returns false when file does not exist")
    {
        REQUIRE_FALSE(descriptionRepository.exists("aaaa", listName));
    }

    SECTION("exists returns true after file is created")
    {
        std::filesystem::path path = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(path.parent_path());
        std::ofstream file(path);
        file << "Some content.\n";
        file.close();

        REQUIRE(descriptionRepository.exists("aaaa", listName));
        installation.wipe();
        installation.make();
    }

    SECTION("load returns file content")
    {
        std::filesystem::path path = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(path.parent_path());
        std::ofstream file(path);
        file << "Description content.\n";
        file.close();

        std::string content = descriptionRepository.load("aaaa", listName);
        REQUIRE(content.find("Description content.") != std::string::npos);
        installation.wipe();
        installation.make();
    }

    SECTION("load throws when file does not exist")
    {
        REQUIRE_THROWS_AS(descriptionRepository.load("nonexistent", listName), std::runtime_error);
    }

    SECTION("remove deletes existing file")
    {
        std::filesystem::path path = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(path.parent_path());
        std::ofstream file(path);
        file << "Content.\n";
        file.close();

        descriptionRepository.remove("aaaa", listName);

        REQUIRE_FALSE(std::filesystem::exists(path));
        installation.wipe();
        installation.make();
    }

    SECTION("remove on missing file does not throw")
    {
        REQUIRE_NOTHROW(descriptionRepository.remove("nonexistent", listName));
    }

    SECTION("path traversal in item id does not escape descriptions dir")
    {
        std::filesystem::path path = descriptionRepository.getFilePath("../../etc/passwd", listName);
        std::string pathStr = std::filesystem::weakly_canonical(path).string();
        std::string dirStr  = std::filesystem::weakly_canonical(descriptionsDir).string();
        REQUIRE(pathStr.find(dirStr) == 0);
    }

    SECTION("path traversal in list name does not escape descriptions dir")
    {
        ListName traversalList("../../etc", "default");
        std::filesystem::path path = descriptionRepository.getFilePath("aaaa", traversalList);
        std::string pathStr = std::filesystem::weakly_canonical(path).string();
        std::string dirStr  = std::filesystem::weakly_canonical(descriptionsDir).string();
        REQUIRE(pathStr.find(dirStr) == 0);
    }

    SECTION("getIds returns empty vector when descriptions directory does not exist")
    {
        ListName noDescList("noDescriptions", "default");
        std::vector<std::string> ids = descriptionRepository.getIds(noDescList);
        REQUIRE(ids.empty());
    }

    SECTION("getIds returns ids of existing description files")
    {
        std::filesystem::path path1 = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::path path2 = descriptionRepository.getFilePath("bbbb", listName);
        std::filesystem::create_directories(path1.parent_path());
        std::ofstream(path1) << "desc 1";
        std::ofstream(path2) << "desc 2";

        std::vector<std::string> ids = descriptionRepository.getIds(listName);
        REQUIRE(ids.size() == 2);
        REQUIRE(std::find(ids.begin(), ids.end(), "aaaa") != ids.end());
        REQUIRE(std::find(ids.begin(), ids.end(), "bbbb") != ids.end());

        installation.wipe();
        installation.make();
    }

    SECTION("getIds does not include removed description files")
    {
        std::filesystem::path path = descriptionRepository.getFilePath("aaaa", listName);
        std::filesystem::create_directories(path.parent_path());
        std::ofstream(path) << "desc";

        descriptionRepository.remove("aaaa", listName);

        std::vector<std::string> ids = descriptionRepository.getIds(listName);
        REQUIRE(std::find(ids.begin(), ids.end(), "aaaa") == ids.end());

        installation.wipe();
        installation.make();
    }

    installation.wipe();
}
