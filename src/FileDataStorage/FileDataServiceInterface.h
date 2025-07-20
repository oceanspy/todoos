#ifndef FILEDATASERVICEINTERFACE_H
#define FILEDATASERVICEINTERFACE_H

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class FileDataServiceInterface
{
  public:
    virtual void load(std::filesystem::path path) = 0;
    virtual bool isFileWritable(std::filesystem::path path) = 0;
    virtual std::vector<std::vector<std::string>> read(std::optional<int> limitOpt) = 0;
    virtual void write(std::vector<std::vector<std::string>> data) = 0;
    virtual void append(std::vector<std::vector<std::string>> data) = 0;
    virtual void empty() = 0;
    virtual ~FileDataServiceInterface() = default; // Virtual destructor to ensure proper cleanup
};

#endif // FILEDATASERVICEINTERFACE_H
