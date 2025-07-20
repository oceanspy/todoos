#ifndef CSVSERVICE_H
#define CSVSERVICE_H

#include <deque>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "../IOService/IOService.h"
#include "FileDataServiceInterface.h"

class CSVService : public FileDataServiceInterface
{
  public:
    explicit CSVService(IOService& ioService);
    void load(std::filesystem::path path) override;
    bool isFileWritable(std::filesystem::path path) override;
    std::vector<std::vector<std::string>> read(std::optional<int> limitOpt) override;
    void write(std::vector<std::vector<std::string>> data) override;
    void append(std::vector<std::vector<std::string>> data) override;
    void empty() override;

  private:
    IOService& ioService;
    std::filesystem::path path;
    bool fileWritable = false;
    static std::string createItem(const std::string& str);
    static std::string escapeQuotes(const std::string& str);
};

#endif // CSVSERVICE_H
