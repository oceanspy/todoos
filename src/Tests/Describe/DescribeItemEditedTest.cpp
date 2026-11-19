#include "../../Describe/DescribeItemEdited.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

static std::filesystem::path
tempDir()
{
    std::filesystem::path path = std::filesystem::temp_directory_path() / "todoos_tests_describe";
    std::filesystem::create_directories(path);
    return path;
}

static std::string
writeTempFile(const std::string& content, const std::string& name = "src.md")
{
    std::filesystem::path path = tempDir() / name;
    std::ofstream file(path);
    file << content;
    return path.string();
}

static void
cleanup()
{
    std::filesystem::remove_all(tempDir());
}

TEST_CASE("DescribeItemEdited", "[Describe][DescribeItemEdited]")
{
    std::filesystem::create_directories(tempDir());

    SECTION("valid file — title and description extracted correctly")
    {
        std::string src = writeTempFile("| ID | List |\n"
                                        "| -- | ---- |\n"
                                        "| aaaa | mylist |\n"
                                        "\n"
                                        "<!-- todoos:title -->\n"
                                        "# My task title\n"
                                        "\n"
                                        "Some description content.\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asTitle() == "My task title");
        REQUIRE(edited.hasDescription());
        REQUIRE(edited.asDescription() == "Some description content.\n");
        cleanup();
    }

    SECTION("leading empty lines in body are stripped from description")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# Title\n"
                                        "\n"
                                        "\n"
                                        "Actual content.\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asDescription() == "Actual content.\n");
        cleanup();
    }

    SECTION("body with only empty lines — no description")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# Title\n"
                                        "\n"
                                        "\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE_FALSE(edited.hasDescription());
        REQUIRE(edited.asDescription().empty());
        cleanup();
    }

    SECTION("header table deleted — marker still found, parsing succeeds")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# Title without header\n"
                                        "\n"
                                        "Description.\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asTitle() == "Title without header");
        REQUIRE(edited.hasDescription());
        cleanup();
    }

    SECTION("second <!-- todoos:title --> in body treated as plain content")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# Title\n"
                                        "\n"
                                        "<!-- todoos:title -->\n"
                                        "More content.\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asTitle() == "Title");
        REQUIRE(edited.hasDescription());
        REQUIRE(edited.asDescription() == "<!-- todoos:title -->\nMore content.\n");
        cleanup();
    }

    SECTION("h2 heading in body not mistaken for title")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# Real Title\n"
                                        "\n"
                                        "## Section heading\n"
                                        "Content.\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asTitle() == "Real Title");
        cleanup();
    }

    SECTION("marker missing — throws")
    {
        std::string src = writeTempFile("# Title\n"
                                        "\n"
                                        "Description.\n");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("title line missing after marker — throws")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("blank line between marker and title — throws")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "\n"
                                        "# Title\n");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("title is empty after '# ' — throws")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# \n");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("line after marker does not start with '# ' — throws")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "No hash here\n");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("empty file — throws")
    {
        std::string src = writeTempFile("");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("file with only newlines — throws")
    {
        std::string src = writeTempFile("\n\n\n");

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("src file does not exist — throws")
    {
        std::string src = (tempDir() / "nonexistent.md").string();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src), std::runtime_error);
        cleanup();
    }

    SECTION("very long title")
    {
        std::string longTitle(500, 'x');
        std::string src = writeTempFile("<!-- todoos:title -->\n# " + longTitle + "\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asTitle() == longTitle);
        cleanup();
    }

    SECTION("title with unicode characters")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n"
                                        "# Tâche avec accents é à ü\n");

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src);

        REQUIRE(edited.asTitle() == "Tâche avec accents é à ü");
        cleanup();
    }
}
