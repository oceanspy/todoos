#include "IOCliService.h"

void IOCliService::print(const std::string& message)
{
    std::cout << message << "\n";
}

void IOCliService::printWithoutLineBreak(const std::string& message)
{
    std::cout << message;
}

void IOCliService::error(const std::string& message)
{
    std::cerr << message << "\n";
}

void IOCliService::br()
{
    std::cout << "\n";
}

std::string IOCliService::ask(const std::string& message)
{
    std::string answer;
    std::cout << message;
    std::cin >> answer;
    return answer;
}