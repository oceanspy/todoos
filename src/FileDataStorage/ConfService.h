#ifndef TODOOS_CONFSERVICE_H
#define TODOOS_CONFSERVICE_H

#include <filesystem>
#include <string>
#include <vector>
#include <optional>
#include <deque>
#include <fstream>

#include "FileDataServiceInterface.h"
#include "../IOService/IOService.h"

class ConfService : public FileDataServiceInterface {
public:
    explicit ConfService(IOService& ioService);
    void load(std::filesystem::path path) override;
    bool isFileWritable(std::filesystem::path path) override;
    std::vector < std::vector <std::string>> read(std::optional<int> limitOpt) override;
    void write(std::vector < std::vector <std::string>> data) override;
    void append(std::vector <std::vector <std::string>> data) override;
    void empty() override;
private:
    IOService& ioService;
    std::filesystem::path path;
    bool fileWritable = false;
    static std::string createItem(const std::string& str);
    static std::string escapeQuotes(const std::string& str);
};


#endif //TODOOS_CONFSERVICE_H
