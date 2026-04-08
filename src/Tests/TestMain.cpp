#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <clocale>

struct LocaleSetup : Catch::EventListenerBase
{
    using Catch::EventListenerBase::EventListenerBase;

    void testRunStarting(Catch::TestRunInfo const&) override { std::setlocale(LC_CTYPE, ""); }
};

CATCH_REGISTER_LISTENER(LocaleSetup)
