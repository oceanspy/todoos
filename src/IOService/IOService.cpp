#include "IOService.h"

#include <utility>

#include "IOCliService/IOCliService.h"


IOService::IOService(std::string channel)
    : channel(std::move(channel))
{

}

void IOService::print(const std::string& message)
{
    if (channel == "cli")
    {
        IOCliService::print(message);
    }
}

void IOService::message(const std::string& message)
{
    if (channel == "cli")
    {
        IOCliService::print(" " + message);
    }
}

void IOService::printWithoutLineBreak(const std::string& message)
{
    if (channel == "cli")
    {
        IOCliService::printWithoutLineBreak(message);
    }
}

void IOService::success(std::string message)
{
    message = " ✅ " + message;
    if (channel == "cli")
    {
        IOCliService::error(message);
    }
}

void IOService::error(std::string message)
{
    message = " ❌ " + message;
    if (channel == "cli")
    {
        IOCliService::error(message);
    }
}

void IOService::info(std::string message)
{
    message = " ℹ️  " + message;
    if (channel == "cli")
    {
        IOCliService::error(message);
    }
}

void IOService::br()
{
    if (channel == "cli")
    {
        IOCliService::br();
    }
}

void IOService::show(const std::vector <std::string>& messages)
{
    if (channel == "cli")
    {
        for (const std::string& message : messages)
        {
            IOCliService::print(message);
        }
    }
}

std::string IOService::ask(const std::string& message)
{
    if (channel == "cli")
    {
        return IOCliService::ask(" " + message);
    }

    return "";
}

int IOService::getConsoleDisplayWidth() {
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}