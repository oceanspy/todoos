#include "CSVService.h"

CSVService::CSVService(IOService& ioService)
    : ioService(ioService)
{
}

void CSVService::load(std::filesystem::path path)
{
    this->path = path;

    if (!isFileWritable(path))
    {
        throw std::invalid_argument("Error: Unable to load the file: " + path.string());
    }
}

bool CSVService::isFileWritable(std::filesystem::path path)
{
    if (!fileWritable)
    {
        std::ofstream file(path, std::ofstream::out | std::ofstream::app);
        fileWritable = file.is_open();
        file.close();
    }
    return fileWritable;
}


std::vector < std::vector <std::string>> CSVService::read(std::optional<int> limitOpt)
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

        const char *mystart = line.c_str(); // prepare to parse the line - start is position of begin of field
        bool instring{false};
        for (const char *p = mystart; *p; p++) { // iterate through the string
            if (*p == '"') { // toggle flag if we're btw double quote
                instring = !instring;
            } else if (*p == ',' && !instring) { // if comma OUTSIDE double quote
                std::string field = std::string(mystart, p - mystart);

                // Remove quotes if they exist
                if (!field.empty() && field.front() == '"' && field.back() == '"') {
                    field = field.substr(1, field.size() - 2);
                }

                // if double quote, we simply them to one
                for (size_t i = 0; i < field.size(); i++) {
                    if (field[i] == '"') {
                        field.erase(i, 1);
                    }
                }

                row.push_back(field); // keep the field
                mystart = p + 1; // and start parsing next one
            }
        }
        std::string field = std::string(mystart);

        // Remove quotes if they exist
        if (!field.empty() && field.front() == '"' && field.back() == '"') {
            field = field.substr(1, field.size() - 2);
        }

        // if double quote, we simply them to one
        for (size_t i = 0; i < field.size(); i++) {
            if (field[i] == '"') {
                field.erase(i, 1);
            }
        }

        row.push_back(field);
        data.push_back(row);

        if (limit > 0 && data.size() > limit) {
            data.erase(data.begin());
        }
    }
    file.close();
    return data;
}

void CSVService::write(std::vector <std::vector <std::string>> data)
{
    std::ofstream file(path, std::ofstream::out);

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << createItem(row[i]);
            if (i < row.size() - 1) {
                file << ","; // Add comma separator between fields
            }
        }
        file << std::endl; // End of row
    }

    file.close();
}

void CSVService::append(std::vector <std::vector <std::string>> data)
{
    std::ofstream file(path, std::ios::app);

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << createItem(row[i]);
            if (i < row.size() - 1) {
                file << ","; // Add comma separator between fields
            }
        }
        file << std::endl; // End of row
    }

    file.close();
}

void CSVService::empty()
{
    std::ofstream file(path, std::ofstream::out);
    file.close();
}

std::string CSVService::createItem(const std::string& str) {
    std::string item;
    item = escapeQuotes(str);

    // if string contains comma, we have to wrap it in double quotes
    if (item.find(',') != std::string::npos || item.find('"') != std::string::npos) {
        item = "\"" + item + "\"";
    }

    return item;
}

std::string CSVService::escapeQuotes(const std::string& str) {
    std::string item = str;
    for (size_t i = 0; i < item.size(); i++) {
        if (item[i] == '"') {
            item.insert(i, 1, '"');
            i++;
        }
    }

    return item;
}