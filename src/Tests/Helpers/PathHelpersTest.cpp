#include "../../Helpers/PathHelpers.h"
#include "ScopedHomeEnv.h"
#include <catch2/catch_test_macros.hpp>
#include <pwd.h>
#include <stdexcept>
#include <unistd.h>

TEST_CASE("PathHelpers::getHomeDir", "[PathHelpers]")
{
    SECTION("Returns HOME when it is set")
    {
        ScopedHomeEnv home("/home/testuser");
        REQUIRE(PathHelpers::getHomeDir() == std::filesystem::path("/home/testuser"));
    }

    SECTION("Falls back to the passwd entry when HOME is unset or empty")
    {
        const passwd* pw = getpwuid(getuid());
        REQUIRE(pw != nullptr);
        std::filesystem::path expected = pw->pw_dir;

        SECTION("HOME is unset")
        {
            ScopedHomeEnv home(nullptr);
            REQUIRE(PathHelpers::getHomeDir() == expected);
        }

        SECTION("HOME is empty")
        {
            ScopedHomeEnv home("");
            REQUIRE(PathHelpers::getHomeDir() == expected);
        }
    }
}

TEST_CASE("PathHelpers::resolveHomeDir", "[PathHelpers]")
{
    SECTION("HOME takes priority over the passwd entry")
    {
        REQUIRE(PathHelpers::resolveHomeDir("/home/env", "/home/pw") == std::filesystem::path("/home/env"));
    }

    SECTION("Falls back to the passwd entry when HOME is unset or empty")
    {
        REQUIRE(PathHelpers::resolveHomeDir(nullptr, "/home/pw") == std::filesystem::path("/home/pw"));
        REQUIRE(PathHelpers::resolveHomeDir("", "/home/pw") == std::filesystem::path("/home/pw"));
    }

    SECTION("Throws when no home directory is available")
    {
        REQUIRE_THROWS_AS(PathHelpers::resolveHomeDir(nullptr, nullptr), std::runtime_error);
        REQUIRE_THROWS_AS(PathHelpers::resolveHomeDir("", nullptr), std::runtime_error);
        REQUIRE_THROWS_AS(PathHelpers::resolveHomeDir(nullptr, ""), std::runtime_error);
        REQUIRE_THROWS_AS(PathHelpers::resolveHomeDir("", ""), std::runtime_error);
    }
}
