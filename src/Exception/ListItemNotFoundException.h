#include "../List/ListName.h"
#include <exception>
#include <string>

class ListItemNotFoundException : public std::exception
{

  public:
    ListItemNotFoundException(const std::string msg, const std::string id, const ListName listName);
    const char* what() const noexcept override;
    std::string& getMessage();
    std::string& getId();
    ListName& getListName();

  private:
    std::string message;
    std::string id;
    ListName listName;
};
