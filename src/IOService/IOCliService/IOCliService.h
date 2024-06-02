#ifndef IOCLISERVICE_H
#define IOCLISERVICE_H

#include <iostream>
#include <string>

class IOCliService {
public:
    static void print(const std::string& message);
    static void printWithoutLineBreak(const std::string& message);
    static void error(const std::string& message);
    static std::string ask(const std::string& message);
    static void br();
};



#endif //IOCLISERVICE_H
