#ifndef LISTITEMID_H
#define LISTITEMID_H

#include "../Helpers/StringHelpers.h"
#include "ListName.h"
#include <string>

class ListItemId
{
  public:
    static int getIdLength();
    static std::string const generate(const std::string& generationType);

  private:
    static const int idLength = 4;
    static const std::string idLetters;
    static const std::string idLettersLowercase;
    static const std::string idRandom;
};

#endif // LISTITEMID_H
