#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../../FileDataStorage/CSVService.h"
#include "../../IOService/IOService.h"
#include <filesystem>

TEST_CASE("CSVService tests", "[CSVService]") {
    // Create a temporary directory for testing
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "_todoos_csv_service_test";
    std::filesystem::create_directory(tempDir);

    // Create a temporary file path for testing
    std::filesystem::path tempFile = tempDir / "test.csv";
    std::filesystem::path wrongPermissionTempFile = "/root/test.csv";

    // Create a mock IOService for testing
    IOService ioService("cli");

    SECTION("Load method throws invalid_argument when file is not writable") {
        CSVService csvService(ioService);
        REQUIRE_THROWS_AS(csvService.load(wrongPermissionTempFile), std::invalid_argument);
    }

    SECTION("Read and write methods work correctly") {
        CSVService csvService(ioService);

        // Prepare data for writing
        std::vector<std::vector<std::string>> testData = {{"1", "2", "3"}, {"4", "5", "6"}, {"7", "8", "9"}};

        // Write data to the file
        csvService.load(tempFile);
        csvService.write(testData);

        // Read data from the file
        std::vector<std::vector<std::string>> readData = csvService.read(std::nullopt);

        // Verify that read data matches written data
        REQUIRE(readData == testData);
    }

    SECTION("Read and write methods work correctly") {
        CSVService csvService(ioService);
        std::vector<std::vector<std::string>> testData = {{"John", "He said, \"Hello, World!\""}, {"Jane", "She replied, \"Nice to meet you.\""}};

        csvService.load(tempFile);
        csvService.write(testData);

        std::vector<std::vector<std::string>> readData = csvService.read(std::nullopt);
        REQUIRE(readData == testData);
    }

    SECTION("Append element to CSV") {
        CSVService csvService(ioService);
        std::vector<std::vector<std::string>> testData1 = {{"John", "He said, \"Hello, World!\""}};
        std::vector<std::vector<std::string>> testData2 = {{"Jane", "She replied, \"Nice to meet you.\""}};

        csvService.load(tempFile);
        csvService.write(testData1);
        csvService.append(testData2);

        std::vector<std::vector<std::string>> readData = csvService.read(std::nullopt);
        std::vector<std::vector<std::string>> expectedData = {testData1[0], testData2[0]};
        REQUIRE(readData == expectedData);
    }

    SECTION("Empty method removes all data from file") {
        CSVService csvService(ioService);
        std::vector<std::vector<std::string>> testData = {{"John", "He said, \"Hello, World!\""}, {"Jane", "She replied, \"Nice to meet you.\""}};

        csvService.load(tempFile);
        csvService.write(testData);

        csvService.empty();
        std::vector<std::vector<std::string>> readData = csvService.read(std::nullopt);
        REQUIRE(readData.empty());
    }

    // Clean up: remove temporary directory
    std::filesystem::remove_all(tempDir);
}