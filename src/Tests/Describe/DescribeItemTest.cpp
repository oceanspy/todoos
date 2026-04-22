#include "../../Describe/DescribeItem.h"
#include "../../IOService/IOService.h"
#include "../../List/ListItems/ListItemEntity.h"
#include "../../List/ListItems/PriorityService.h"
#include "../../List/ListItems/StatusService.h"
#include "../../List/ListName.h"
#include <catch2/catch_test_macros.hpp>

static ListItemEntity makeItem(PriorityService& priorityService,
                               StatusService& statusService,
                               ListName& listName,
                               const std::string& value = "My task title")
{
    PriorityEntity priority = priorityService.find(PriorityService::MEDIUM);
    StatusEntity status     = statusService.find(StatusService::QUEUED);
    return ListItemEntity::set("aaaa", value, priority, status, 0, 0, 1712487259, 1712487259, listName);
}

TEST_CASE("DescribeItem", "[Describe][DescribeItem]")
{
    IOService ioService("cli");
    PriorityService priorityService;
    StatusService statusService;
    ListName listName("mylist", "default");
    ListItemEntity listItem = makeItem(priorityService, statusService, listName);

    SECTION("asDescriptionItemFile contains marker")
    {
        DescribeItem describeItem = DescribeItem::create(listName, listItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("<!-- todoos:title -->") != std::string::npos);
    }

    SECTION("asDescriptionItemFile title line matches item value")
    {
        DescribeItem describeItem = DescribeItem::create(listName, listItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("# My task title") != std::string::npos);
    }

    SECTION("asDescriptionItemFile contains list name in table")
    {
        DescribeItem describeItem = DescribeItem::create(listName, listItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("mylist") != std::string::npos);
    }

    SECTION("asDescriptionItemFile contains item id in table")
    {
        DescribeItem describeItem = DescribeItem::create(listName, listItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("aaaa") != std::string::npos);
    }

    SECTION("existing description appended after title")
    {
        DescribeItem describeItem = DescribeItem::create(listName, listItem, "Some existing notes.");
        std::string output = describeItem.asDescriptionItemFile();
        size_t titlePos = output.find("# My task title");
        size_t descPos  = output.find("Some existing notes.");
        REQUIRE(titlePos != std::string::npos);
        REQUIRE(descPos != std::string::npos);
        REQUIRE(descPos > titlePos);
    }

    SECTION("empty existing description produces no trailing body")
    {
        DescribeItem describeItem = DescribeItem::create(listName, listItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        size_t titlePos = output.find("# My task title");
        std::string afterTitle = output.substr(titlePos + std::string("# My task title").size());
        REQUIRE(afterTitle.find_first_not_of("\n") == std::string::npos);
    }

    SECTION("title with markdown special characters")
    {
        ListItemEntity specialItem = makeItem(priorityService, statusService, listName, "Task with | pipe and *bold*");
        DescribeItem describeItem = DescribeItem::create(listName, specialItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("# Task with | pipe and *bold*") != std::string::npos);
    }

    SECTION("title with unicode characters")
    {
        ListItemEntity unicodeItem = makeItem(priorityService, statusService, listName, "Tâche avec accents é à ü");
        DescribeItem describeItem = DescribeItem::create(listName, unicodeItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("# Tâche avec accents é à ü") != std::string::npos);
    }

    SECTION("table columns adapt to content width")
    {
        ListName longList("a-very-long-list-name", "default");
        ListItemEntity longItem = makeItem(priorityService, statusService, longList, "Task");
        DescribeItem describeItem = DescribeItem::create(longList, longItem, "");
        std::string output = describeItem.asDescriptionItemFile();
        REQUIRE(output.find("a-very-long-list-name") != std::string::npos);
        size_t headerPos   = output.find("| List");
        size_t separatorPos = output.find("| -", headerPos);
        std::string separatorLine = output.substr(separatorPos, output.find('\n', separatorPos) - separatorPos);
        REQUIRE(separatorLine.find("---------------------") != std::string::npos);
    }
}
