#include "CliIOChannel.h"

void
CliIOChannel::print(const std::string& message)
{
    std::cout << message << "\n";
}

void
CliIOChannel::printWithoutLineBreak(const std::string& message)
{
    std::cout << message;
}

void
CliIOChannel::error(const std::string& message)
{
    std::cerr << message << "\n";
}

void
CliIOChannel::br()
{
    std::cout << "\n";
}

std::string
CliIOChannel::ask(const std::string& message)
{
    std::string answer;
    std::cout << message;
    std::getline(std::cin, answer);
    return answer;
}
