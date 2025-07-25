#ifndef IOSERVICE_H
#define IOSERVICE_H

#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

class IOService
{
  public:
    explicit IOService(const std::string& channel);
    void print(const std::string& message);
    void message(const std::string& message);
    void printWithoutLineBreak(const std::string& message);
    void success(std::string message);
    void error(std::string message);
    void info(std::string message);
    void br();
    void title(const std::string& title);
    void show(const std::vector<std::string>& messages);
    std::string ask(const std::string& message);
    static int getConsoleDisplayWidth();

  protected:
    const std::string& channel;
};

#endif // IOSERVICE_H
