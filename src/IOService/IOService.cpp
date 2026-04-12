#include "IOService.h"

#include "IOCliService/IOCliService.h"

IOService::IOService(const std::string& channel)
  : channel(channel)
{
}

void
IOService::print(const std::string& message)
{
    if (channel == "cli") {
        IOCliService::print(message);
    }

    lastOutput = "print";
}

void
IOService::message(const std::string& message)
{
    if (channel == "cli") {
        IOCliService::print(" " + message);
    }

    lastOutput = "print";
}

void
IOService::printWithoutLineBreak(const std::string& message)
{
    if (channel == "cli") {
        IOCliService::printWithoutLineBreak(message);
    }

    lastOutput = "printWithoutLineBreak";
}

void
IOService::success(std::string message)
{
    message = " ✅ " + message;
    if (channel == "cli") {
        IOCliService::print(message);
    }

    lastOutput = "print";
}

void
IOService::error(std::string message)
{
    message = " ❌ " + message;
    if (channel == "cli") {
        IOCliService::error(message);
    }

    lastOutput = "print";
}

void
IOService::info(std::string message)
{
    message = " ℹ️  " + message;
    if (channel == "cli") {
        IOCliService::print(message);
    }

    lastOutput = "print";
}

void
IOService::br()
{
    if (channel == "cli") {
        IOCliService::br();
    }

    lastOutput = "br";
}

void
IOService::brOrSkip()
{
    if (lastOutput == "br") {
        return;
    }

    if (channel == "cli") {
        IOCliService::br();
    }

    lastOutput = "br";
}

void
IOService::show(const std::vector<std::string>& messages)
{
    if (channel == "cli") {
        for (const std::string& message : messages) {
            IOCliService::print(message);
        }
    }

    lastOutput = "print";
}

void
IOService::title(const std::string& title)
{
    if (channel == "cli") {
        IOCliService::print(" # " + title);
    }

    lastOutput = "print";
}

std::string
IOService::ask(const std::string& message)
{
    if (channel == "cli") {
        return IOCliService::ask(" " + message);
    }

    lastOutput = "ask";

    return "";
}

int
IOService::getConsoleDisplayWidth()
{
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}
