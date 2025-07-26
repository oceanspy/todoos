#ifndef LISTNAME_H
#define LISTNAME_H

#include <string>

class ListName
{
  public:
    ListName(std::string name, std::string variant);
    static ListName createVariant(ListName& listName, std::string listVariant);
    std::string getName() const;
    std::string getVariant() const;
    static bool isVariantExists(std::string& variant);

  private:
    std::string name;
    std::string variant;
};

#endif // LISTNAME_H
