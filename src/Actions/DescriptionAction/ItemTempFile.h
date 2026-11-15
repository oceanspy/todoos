#pragma once

#include "../../Describe/DescribeItem.h"
#include "../../Describe/DescriptionService.h"
#include "../../IOService/IOService.h"
#include <filesystem>
#include <string>

class ItemTempFile
{
  public:
    ItemTempFile(IOService& ioService, const DescribeItem& describeItem, const DescriptionService& descriptionService);
    std::string execute();

    static void reopen(const std::string& filePath);
    static void repair(const std::string& filePath, const DescribeItem& describeItem);

  private:
    IOService& ioService;
    const DescribeItem& describeItem;
    const DescriptionService& descriptionService;

    static void openInEditor(const std::string& filePath);
    static std::string resolveEditor();
};
