#define CATCH_CONFIG_MAIN
#include "../../IOService/IOService.h"
#include "../../IOService/CliIOChannel/CliIOChannel.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("IOService CLI channel tests", "[IOService][CLI]")
{
    SECTION("Print test")
    {
        IOService io("cli");
        std::string message = "Test message";
        REQUIRE_NOTHROW(io.print(message));
        INFO("Printed message: " << message);
        CAPTURE(message);
        // No direct way to check output in CLI, can check for exceptions
    }

    SECTION("Error test")
    {
        IOService io("cli");
        std::string message = "Test error message";
        REQUIRE_NOTHROW(io.error(message));
        INFO("Error message: " << message);
        CAPTURE(message);
        // No direct way to check output in CLI, can check for exceptions
    }

    SECTION("Br test")
    {
        IOService io("cli");
        REQUIRE_NOTHROW(io.br());
        INFO("New line printed.");
        // No direct way to check output in CLI, can check for exceptions
    }

    SECTION("Show test")
    {
        IOService io("cli");
        std::vector<std::string> messages = { "Message 1", "Message 2" };
        REQUIRE_NOTHROW(io.show(messages));
        for (const auto& message : messages) {
            INFO("Printed message: " << message);
            CAPTURE(message);
        }
        // No direct way to check output in CLI, can check for exceptions
    }

    // Not possible to test interactive input/output in automated tests
    // TODO: Implement a way to test interactive input/output (via Mock IOService ?)
    //
    //    SECTION("Ask test") {
    //        IOService io("cli");
    //        std::string message = "Test question";
    //        // Can't directly test interactive input, can check for exceptions
    //        REQUIRE_NOTHROW(io.ask(message));
    //        INFO("Asked question: " << message);
    //        CAPTURE(message);
    //    }
}

TEST_CASE("IOService additional CLI channel tests", "[IOService][CLI]")
{
    SECTION("message does not throw")
    {
        IOService io("cli");
        REQUIRE_NOTHROW(io.message("Test message"));
    }

    SECTION("printWithoutLineBreak does not throw")
    {
        IOService io("cli");
        REQUIRE_NOTHROW(io.printWithoutLineBreak("Test"));
    }

    SECTION("success does not throw")
    {
        IOService io("cli");
        REQUIRE_NOTHROW(io.success("Operation completed"));
    }

    SECTION("info does not throw")
    {
        IOService io("cli");
        REQUIRE_NOTHROW(io.info("Information message"));
    }

    SECTION("title does not throw")
    {
        IOService io("cli");
        REQUIRE_NOTHROW(io.title("Test Title"));
    }

    SECTION("getConsoleDisplayWidth returns non-negative")
    {
        int width = IOService::getConsoleDisplayWidth();
        REQUIRE(width >= 0);
    }
}

TEST_CASE("IOService non-CLI channel silently ignores", "[IOService][Other]")
{
    IOService io("other");

    SECTION("message does not throw on non-CLI channel")
    {
        REQUIRE_NOTHROW(io.message("Test"));
    }

    SECTION("printWithoutLineBreak does not throw on non-CLI channel")
    {
        REQUIRE_NOTHROW(io.printWithoutLineBreak("Test"));
    }

    SECTION("success does not throw on non-CLI channel")
    {
        REQUIRE_NOTHROW(io.success("Test"));
    }

    SECTION("info does not throw on non-CLI channel")
    {
        REQUIRE_NOTHROW(io.info("Test"));
    }

    SECTION("title does not throw on non-CLI channel")
    {
        REQUIRE_NOTHROW(io.title("Test"));
    }

    SECTION("ask returns empty on non-CLI channel")
    {
        std::string result = io.ask("question?");
        REQUIRE(result == "");
    }
}

TEST_CASE("IOService other channel tests", "[IOService][Other]")
{
    SECTION("Print test")
    {
        IOService io("other");
        std::string message = "Test message";
        REQUIRE_NOTHROW(io.print(message));
        INFO("Printed message: " << message);
        CAPTURE(message);
        // No output expected as channel is not CLI
    }

    SECTION("Error test")
    {
        IOService io("other");
        std::string message = "Test error message";
        REQUIRE_NOTHROW(io.error(message));
        INFO("Error message: " << message);
        CAPTURE(message);
        // No output expected as channel is not CLI
    }

    SECTION("Br test")
    {
        IOService io("other");
        REQUIRE_NOTHROW(io.br());
        INFO("New line printed.");
        // No output expected as channel is not CLI
    }

    SECTION("Show test")
    {
        IOService io("other");
        std::vector<std::string> messages = { "Message 1", "Message 2" };
        REQUIRE_NOTHROW(io.show(messages));
        for (const auto& message : messages) {
            INFO("Printed message: " << message);
            CAPTURE(message);
        }
        // No output expected as channel is not CLI
    }

    //    SECTION("Ask test") {
    //        IOService io("other");
    //        std::string message = "Test question";
    //        REQUIRE(io.ask(message) == "");
    //        INFO("Asked question: " << message);
    //        CAPTURE(message);
    //        // No interaction expected as channel is not CLI
    //    }
}
