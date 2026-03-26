#include "../../Events/Event.h"
#include "../../Events/EventBus.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventTest", "[Event]")
{
    SECTION("Event stores type and payload")
    {
        Event event("testType", "testPayload");
        REQUIRE(event.getType() == "testType");
        REQUIRE(event.getPayload() == "testPayload");
    }

    SECTION("Event with empty strings")
    {
        Event event("", "");
        REQUIRE(event.getType() == "");
        REQUIRE(event.getPayload() == "");
    }

    SECTION("Event with special characters")
    {
        Event event("type:with:colons", "payload with spaces & symbols!");
        REQUIRE(event.getType() == "type:with:colons");
        REQUIRE(event.getPayload() == "payload with spaces & symbols!");
    }
}

TEST_CASE("EventBusTest", "[EventBus]")
{
    EventBus bus;

    SECTION("subscribe and publish triggers listener")
    {
        std::string received;
        bus.subscribe("testEvent", [&received](Event event) { received = event.getPayload(); });

        bus.publish(Event("testEvent", "hello"));
        REQUIRE(received == "hello");
    }

    SECTION("publish with no subscribers does not throw")
    {
        REQUIRE_NOTHROW(bus.publish(Event("noListeners", "data")));
    }

    SECTION("multiple subscribers receive the event")
    {
        int callCount = 0;
        bus.subscribe("multi", [&callCount](Event event) { callCount++; });
        bus.subscribe("multi", [&callCount](Event event) { callCount++; });
        bus.subscribe("multi", [&callCount](Event event) { callCount++; });

        bus.publish(Event("multi", "data"));
        REQUIRE(callCount == 3);
    }

    SECTION("different event types are independent")
    {
        std::string resultA;
        std::string resultB;
        bus.subscribe("typeA", [&resultA](Event event) { resultA = event.getPayload(); });
        bus.subscribe("typeB", [&resultB](Event event) { resultB = event.getPayload(); });

        bus.publish(Event("typeA", "dataA"));
        REQUIRE(resultA == "dataA");
        REQUIRE(resultB == "");

        bus.publish(Event("typeB", "dataB"));
        REQUIRE(resultB == "dataB");
    }

    SECTION("subscriber receives correct payload for multiple publishes")
    {
        std::vector<std::string> received;
        bus.subscribe("seq", [&received](Event event) { received.push_back(event.getPayload()); });

        bus.publish(Event("seq", "first"));
        bus.publish(Event("seq", "second"));
        bus.publish(Event("seq", "third"));

        REQUIRE(received.size() == 3);
        REQUIRE(received[0] == "first");
        REQUIRE(received[1] == "second");
        REQUIRE(received[2] == "third");
    }
}
