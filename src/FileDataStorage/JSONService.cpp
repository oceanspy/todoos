#include "JSONService.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

JSONService::JSONService(IOService& ioService)
    : ioService(ioService)
{
}

void JSONService::load(std::filesystem::path path)
{
    this->path = path;

    if (!isFileWritable(path))
    {
        throw std::invalid_argument("Error: Unable to load the file: " + path.string());
    }

    // if file empty
    if (std::filesystem::is_empty(path))
    {
        // Create an empty JSON array
        write({});
    }
}

bool JSONService::isFileWritable(std::filesystem::path path)
{
    if (!fileWritable)
    {
        std::ofstream file(path, std::ofstream::out | std::ofstream::app);
        fileWritable = file.is_open();
        file.close();
    }
    return fileWritable;
}


std::vector < std::vector <std::string>> JSONService::read(std::optional<int> limitOpt)
{
    int limit = limitOpt.value_or(10000); // Use 10000 as default value
    std::vector<std::vector<std::string>> data;

    // Read the JSON file
    std::ifstream file(path);

    // Parse the JSON
    json j;
    try {
        file >> j;
    } catch (json::parse_error& e) {
        throw std::invalid_argument("Error: Unable to parse the file: " + path.string());
    }

    // Check if the JSON is an array
    if (!j.is_array()) {
        throw std::invalid_argument("JSON Error: Root element is not an array");
    }

    // Iterate over the array elements
    int i = 0;
    for (const auto& innerJson : j) {
        std::vector<std::string> innerVec;

        if (!innerJson.is_array()) {
            throw std::invalid_argument("JSON Error: Inner element is not an array");
        }

        for (const auto& str : innerJson) {
            // Check if each element is a string
            if (!str.is_string()) {
                throw std::invalid_argument("JSON Error: Element is not a string");
            }
            innerVec.push_back(str.get<std::string>());
        }
        data.push_back(innerVec);

        if (limit > 0 && ++i >= limit) {
            data.erase(data.begin());
        }
    }
    file.close();

    return data;
}

void JSONService::write(std::vector <std::vector <std::string>> data)
{
    json json_data = json::array();
    for (const auto& innerVec : data) {
        json inner_json;
        for (const auto& str : innerVec) {
            inner_json.push_back(str);
        }
        json_data.push_back(inner_json);
    }

    std::ofstream file(path, std::ofstream::out);
    file << json_data.dump(4); // Use dump(4) for pretty printing with indentation

    file.close();
}

void JSONService::append(std::vector <std::vector <std::string>> data)
{
    std::vector <std::vector <std::string>> existingData = read(std::nullopt);
    existingData.insert(existingData.end(), data.begin(), data.end());
    write(existingData);
}

void JSONService::empty()
{
    std::ofstream file(path, std::ofstream::out);
    file.close();
}

std::string JSONService::createItem(const std::string& str) {
    std::string item;
    if (str.find(',') != std::string::npos) {
        item = "\"" + escapeQuotes(str) + "\"";
    } else {
        item = str;
    }
    return item;
}

std::string JSONService::escapeQuotes(const std::string& str) {
    std::string escapedItem = str;
    size_t pos = escapedItem.find('"');
    while (pos != std::string::npos) {
        escapedItem.insert(pos, 1, '"'); // Double the quote
        pos = escapedItem.find('"', pos + 2); // Find next quote after the doubled quote
    }
    return escapedItem;
}