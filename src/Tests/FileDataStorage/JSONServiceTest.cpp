#define CATCH_CONFIG_MAIN
#include "../../FileDataStorage/JSONService.h"
#include "../../IOService/IOService.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("JSONService tests", "[JSONService]")
{
    // Create a temporary directory for testing
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "_todoos_json_service_test";
    std::filesystem::create_directory(tempDir);

    // Create a temporary file path for testing
    std::filesystem::path tempFile = tempDir / "test.json";
    std::filesystem::path wrongPermissionTempFile = "/root/test.json";

    // Create a mock IOService for testing
    IOService ioService("cli");

    SECTION("Load method throws invalid_argument when file is not writable")
    {
        JSONService jsonService(ioService);
        REQUIRE_THROWS_AS(jsonService.load(wrongPermissionTempFile), std::invalid_argument);
    }

    SECTION("Read and write methods work correctly")
    {
        JSONService jsonService(ioService);

        // Prepare data for writing
        std::vector<std::vector<std::string>> testData = { { "1", "2", "3" }, { "4", "5", "6" }, { "7", "8", "9" } };

        // Write data to the file
        jsonService.load(tempFile);
        jsonService.write(testData);

        // Read data from the file
        std::vector<std::vector<std::string>> readData = jsonService.read(std::nullopt);

        // Verify that read data matches written data
        REQUIRE(readData == testData);
    }

    // Clean up: remove temporary directory
    std::filesystem::remove_all(tempDir);
}
