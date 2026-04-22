#pragma once

#include "../../Describe/DescribeItem.h"
#include <filesystem>
#include <string>

class ItemTempFile
{
  public:
    ItemTempFile(const DescribeItem& describeItem, const std::filesystem::path& cacheDirPath);
    std::string execute();

    static void reopen(const std::string& filePath);
    static void repair(const std::string& filePath, const DescribeItem& describeItem);

  private:
    const DescribeItem& describeItem;
    std::filesystem::path cacheDirPath;

    static void openInEditor(const std::string& filePath);
    static std::string resolveEditor();
};
