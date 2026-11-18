#ifndef LISTITEMID_H
#define LISTITEMID_H

#include "../Helpers/StringHelpers.h"
#include "ListName.h"
#include <string>

class ListItemId
{
  public:
    static int getIdLength();
    static int getVariantIdLength();
    static std::string const generate(const std::string& generationType);
    // Valid ids are alphanumeric, 4 chars (active list) or 5 chars (archive / delete lists)
    static bool isValid(const std::string& id);
    // The 4-char part of an id (the id itself for an active-list id)
    static std::string baseOf(const std::string& id);
    static std::string withSuffix(const std::string& base, char suffix);

  private:
    static const int idLength = 4;
    static const int variantIdLength = 5;
    static const std::string idLetters;
    static const std::string idLettersLowercase;
    static const std::string idRandom;
};

#endif // LISTITEMID_H
