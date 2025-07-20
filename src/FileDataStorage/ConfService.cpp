#include "ConfService.h"

ConfService::ConfService(IOService& ioService)
  : ioService(ioService)
{
}

void
ConfService::load(std::filesystem::path path)
{
    this->path = path;

    if (!isFileWritable(path)) {
        throw std::invalid_argument("Error: Unable to load the file: " + path.string());
    }
}

bool
ConfService::isFileWritable(std::filesystem::path path)
{
    if (!fileWritable) {
        std::ofstream file(path, std::ofstream::out | std::ofstream::app);
        fileWritable = file.is_open();
        file.close();
    }
    return fileWritable;
}

std::vector<std::vector<std::string>>
ConfService::read(std::optional<int> limitOpt)
{
    int limit = limitOpt.value_or(10000); // Use 10000 as default value
    std::vector<std::vector<std::string>> data;
    // data.reserve(limit); // Reserve space for n elements
    std::ifstream file(path);
    std::string line;
    int i = 0;
    while (std::getline(file, line)) {
        std::vector<std::string> row; // Moved the declaration inside the loop

        // Skip line if it starts with #
        if (line[0] == '#' || line.empty()) {
            continue;
        }

        const char* mystart = line.c_str(); // prepare to parse the line - start is position of begin of field
        bool instring{ false };
        // format is key: value or key: "value"
        for (const char* p = mystart; *p; p++) { // iterate through the string
            if (*p == '"') {                     // toggle flag if we're btw double quote
                instring = !instring;
            } else if (*p == ':' && !instring) { // if comma OUTSIDE double quote
                std::string field = std::string(mystart, p - mystart);
                // Remove quotes if they exist
                if (!field.empty() && field.front() == '"' && field.back() == '"') {
                    field = field.substr(1, field.size() - 2);
                }
                row.push_back(field); // keep the field
                mystart = p + 2;      // and start parsing next one (+2 to skip comma and space)
            }
        }
        std::string field = std::string(mystart);

        row.push_back(field);
        data.push_back(row);

        if (limit > 0 && data.size() > limit) {
            data.erase(data.begin());
        }
    }
    file.close();
    return data;
}

void
ConfService::write(std::vector<std::vector<std::string>> data)
{
    std::ofstream file(path, std::ofstream::out);

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << createItem(row[i]);
            if (i < row.size() - 1) {
                file << ": "; // Add comma separator between fields
            }
        }
        file << std::endl; // End of row
    }

    file.close();
}

void
ConfService::append(std::vector<std::vector<std::string>> data)
{
    std::ofstream file(path, std::ios::app);

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << createItem(row[i]);
            if (i < row.size() - 1) {
                file << ": "; // Add comma separator between fields
            }
        }
        file << std::endl; // End of row
    }

    file.close();
}

void
ConfService::empty()
{
    std::ofstream file(path, std::ofstream::out);
    file.close();
}

std::string
ConfService::createItem(const std::string& str)
{
    std::string item;
    if (str.find(':') != std::string::npos) {
        item = "\"" + escapeQuotes(str) + "\"";
    } else {
        item = str;
    }
    return item;
}

std::string
ConfService::escapeQuotes(const std::string& str)
{
    std::string escapedItem = str;
    size_t pos = escapedItem.find('"');
    while (pos != std::string::npos) {
        escapedItem.insert(pos, 1, '"');      // Double the quote
        pos = escapedItem.find('"', pos + 2); // Find next quote after the doubled quote
    }
    return escapedItem;
}
