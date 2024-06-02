#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../../FileDataStorage/ConfService.h"
#include "../../IOService/IOService.h"
#include <filesystem>

TEST_CASE("ConfService tests", "[ConfService]") {
    // Create a temporary directory for testing
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "_todoos_conf_service_test";
    std::filesystem::create_directory(tempDir);

    // Create a temporary file path for testing
    std::filesystem::path tempFile = tempDir / "test.conf";
    std::filesystem::path wrongPermissionTempFile = "/root/test.conf";

    // Create a mock IOService for testing
    IOService ioService("cli");

    SECTION("Load method throws invalid_argument when file is not writable") {
        ConfService confService(ioService);
        REQUIRE_THROWS_AS(confService.load(wrongPermissionTempFile), std::invalid_argument);
    }

    SECTION("Read and write methods work correctly") {
        ConfService confService(ioService);

        // Prepare data for writing
        std::vector<std::vector<std::string>> testData = {{"key1", "value1"}, {"key2", "value2"}, {"key3", "value3"}, {"key4", "/path/to/some/file"}};

        // Write data to the file
        confService.load(tempFile);
        confService.write(testData);

        // Read data from the file
        std::vector<std::vector<std::string>> readData = confService.read(std::nullopt);

        // Verify that read data matches written data
        REQUIRE(readData == testData);
    }

    // Clean up: remove temporary directory
    std::filesystem::remove_all(tempDir);
}