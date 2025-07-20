#ifndef JSONSERVICE_H
#define JSONSERVICE_H

#include "../IOService/IOService.h"
#include "FileDataServiceInterface.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

class JSONService : public FileDataServiceInterface
{
  public:
    explicit JSONService(IOService& ioService);
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

#endif // JSONSERVICE_H
