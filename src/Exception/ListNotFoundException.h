#include <exception>
#include <string>

class ListNotFoundException : public std::exception
{

  public:
    ListNotFoundException(const std::string& msg, const std::string& name, const std::string& variant);
    std::string getMessage();
    std::string getName();
    std::string getVariant();

  private:
    std::string message;
    std::string name;
    std::string variant;
};
