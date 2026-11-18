#include "ListItemId.h"

#include <cctype>
#include <stdexcept>

const std::string ListItemId::idLetters = "letters";
const std::string ListItemId::idLettersLowercase = "letters-lowercase";
const std::string ListItemId::idRandom = "random";

int
ListItemId::getIdLength()
{
    return idLength;
}

int
ListItemId::getVariantIdLength()
{
    return variantIdLength;
}

std::string const
ListItemId::generate(const std::string& generationType)
{
    std::string id;
    if (generationType == idLettersLowercase) {
        id = StringHelpers::randomLettersLowercase(idLength);
    } else if (generationType == idLetters) {
        id = StringHelpers::randomAlNumString(idLength);
    } else {
        id = StringHelpers::randomString(idLength);
    }

    return id;
}

bool
ListItemId::isValid(const std::string& id)
{
    if (id.length() != idLength && id.length() != variantIdLength) {
        return false;
    }

    for (char ch : id) {
        if (!std::isalnum(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return true;
}

std::string
ListItemId::baseOf(const std::string& id)
{
    return id.substr(0, idLength);
}

std::string
ListItemId::withSuffix(const std::string& base, char suffix)
{
    if (base.length() != idLength) {
        throw std::invalid_argument("Base id must be " + std::to_string(idLength) + " characters long.");
    }
    if (suffix < 'a' || suffix > 'z') {
        throw std::invalid_argument("Id suffix must be a lowercase letter.");
    }

    return base + suffix;
}
