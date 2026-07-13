#include "../../Describe/DescribeItemEdited.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

static std::filesystem::path tempDir()
{
    std::filesystem::path path = std::filesystem::temp_directory_path() / "todoos_tests_describe";
    std::filesystem::create_directories(path);
    return path;
}

static std::string writeTempFile(const std::string& content, const std::string& name = "src.md")
{
    std::filesystem::path path = tempDir() / name;
    std::ofstream file(path);
    file << content;
    return path.string();
}

static std::string destPath(const std::string& name = "dest.md")
{
    return (tempDir() / name).string();
}

static void cleanup()
{
    std::filesystem::remove_all(tempDir());
}

TEST_CASE("DescribeItemEdited", "[Describe][DescribeItemEdited]")
{
    std::filesystem::create_directories(tempDir());

    SECTION("valid file — title and description extracted correctly")
    {
        std::string src = writeTempFile(
            "| ID | List |\n"
            "| -- | ---- |\n"
            "| aaaa | mylist |\n"
            "\n"
            "<!-- todoos:title -->\n"
            "# My task title\n"
            "\n"
            "Some description content.\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE(edited.asTitle() == "My task title");
        REQUIRE(edited.hasDescription());
        REQUIRE(std::filesystem::exists(dest));
        cleanup();
    }

    SECTION("leading empty lines in body are stripped from dest file")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title\n"
            "\n"
            "\n"
            "Actual content.\n");
        std::string dest = destPath();

        DescribeItemEdited::createFromFile(src, dest);

        std::ifstream file(dest);
        std::string firstLine;
        std::getline(file, firstLine);
        REQUIRE(firstLine == "Actual content.");
        cleanup();
    }

    SECTION("body with only empty lines — no dest file written")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title\n"
            "\n"
            "\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE_FALSE(edited.hasDescription());
        REQUIRE_FALSE(std::filesystem::exists(dest));
        cleanup();
    }

    SECTION("header table deleted — marker still found, parsing succeeds")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title without header\n"
            "\n"
            "Description.\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE(edited.asTitle() == "Title without header");
        REQUIRE(edited.hasDescription());
        cleanup();
    }

    SECTION("second <!-- todoos:title --> in body treated as plain content")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title\n"
            "\n"
            "<!-- todoos:title -->\n"
            "More content.\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE(edited.asTitle() == "Title");
        REQUIRE(edited.hasDescription());

        std::ifstream file(dest);
        std::string line;
        std::getline(file, line);
        REQUIRE(line == "<!-- todoos:title -->");
        cleanup();
    }

    SECTION("h2 heading in body not mistaken for title")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Real Title\n"
            "\n"
            "## Section heading\n"
            "Content.\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE(edited.asTitle() == "Real Title");
        cleanup();
    }

    SECTION("marker missing — throws")
    {
        std::string src = writeTempFile(
            "# Title\n"
            "\n"
            "Description.\n");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("title line missing after marker — throws")
    {
        std::string src = writeTempFile("<!-- todoos:title -->\n");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("blank line between marker and title — throws")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "\n"
            "# Title\n");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("title is empty after '# ' — throws")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# \n");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("line after marker does not start with '# ' — throws")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "No hash here\n");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("empty file — throws")
    {
        std::string src = writeTempFile("");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("file with only newlines — throws")
    {
        std::string src = writeTempFile("\n\n\n");
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("src file does not exist — throws")
    {
        std::string src  = (tempDir() / "nonexistent.md").string();
        std::string dest = destPath();

        REQUIRE_THROWS_AS(DescribeItemEdited::createFromFile(src, dest), std::runtime_error);
        cleanup();
    }

    SECTION("destination directory does not exist — created automatically")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title\n"
            "\n"
            "Content.\n");
        std::string dest = (tempDir() / "subdir" / "nested" / "dest.md").string();

        REQUIRE_NOTHROW(DescribeItemEdited::createFromFile(src, dest));
        REQUIRE(std::filesystem::exists(dest));
        cleanup();
    }

    SECTION("existing dest file is overwritten")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title\n"
            "\n"
            "New content.\n");
        std::string dest = destPath();
        std::ofstream existing(dest);
        existing << "Old content.\n";
        existing.close();

        DescribeItemEdited::createFromFile(src, dest);

        std::ifstream file(dest);
        std::string line;
        std::getline(file, line);
        REQUIRE(line == "New content.");
        cleanup();
    }

    SECTION("very long title")
    {
        std::string longTitle(500, 'x');
        std::string src = writeTempFile("<!-- todoos:title -->\n# " + longTitle + "\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE(edited.asTitle() == longTitle);
        cleanup();
    }

    SECTION("title with unicode characters")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Tâche avec accents é à ü\n");
        std::string dest = destPath();

        DescribeItemEdited edited = DescribeItemEdited::createFromFile(src, dest);

        REQUIRE(edited.asTitle() == "Tâche avec accents é à ü");
        cleanup();
    }

    SECTION("path traversal in dest path does not throw — filesystem resolves it")
    {
        std::string src = writeTempFile(
            "<!-- todoos:title -->\n"
            "# Title\n"
            "\n"
            "Content.\n");
        std::string dest = (tempDir() / ".." / "todoos_tests_describe" / "safe_dest.md").string();

        REQUIRE_NOTHROW(DescribeItemEdited::createFromFile(src, dest));
        cleanup();
    }
}
